#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform SceneUniforms {
    mat4 camera;
    vec4 uLightDirectional;
    vec4 cLightDirectional;

    vec4 pLightPositional;
    vec4 cLightPositional;

    vec4 cLightAmbient;

    vec4 pCamera;
} scene;
layout(binding = 1) uniform BodyUniforms {
    mat4 modeling;
    uvec4 texIndices;
    vec4 cSpecular;
} body;
layout(binding = 2) uniform sampler2D samplers[3];

layout(location = 0) in vec2 st;
layout(location = 1) in vec3 dNormal;
layout(location = 2) in vec3 uLightPositional;
layout(location = 3) in vec3 uToCamera;

layout(location = 0) out vec4 outColor;

void main() {
    // uLightPositional got interpolated inside fragment shader, might not be
    // a unit vector here, we just normalize it again.
    
    vec3 uLightPositional = normalize(uLightPositional);

    vec4 rgbaTex = texture(samplers[body.texIndices[0]], st);
    vec3 uNormal = normalize(dNormal);

    float iDiffuseDirectional = max(0.0, dot(uNormal, vec3(scene.uLightDirectional)));

    // iDiffuse for positional light 
    float iDiffusePositional = max(0.0, dot(uNormal, uLightPositional));

    //specular reflection

    //renormalize after interpolation
    vec3 uToCamera = normalize(uToCamera);

    //computed reflected light for both directional and positional sources
    vec3 uLightReflectedDirectional = (2* dot(uNormal, vec3(scene.uLightDirectional)) * uNormal) - vec3(scene.uLightDirectional);
    vec3 uLightReflectedPositional = (2* dot(uNormal,uLightPositional) * uNormal) - uLightPositional;

    float iSpecDirectional = max(0, dot(uLightReflectedDirectional, uToCamera));
    float iSpecPositional = max(0, dot(uLightReflectedPositional, uToCamera));

    if(iDiffuseDirectional <= 0){
        iSpecDirectional = 0;
    }

    if(iDiffusePositional <= 0){
        iSpecPositional = 0;
    }

    vec4 specLightDirectional = pow(iSpecDirectional, 64.0f) * (scene.cLightDirectional * body.cSpecular);

    vec4 specLightPositional = pow(iSpecPositional, 64.0f) * (scene.cLightPositional * body.cSpecular);

    //computing outColors

    outColor =  iDiffuseDirectional * scene.cLightDirectional * rgbaTex;
    outColor = outColor + (iDiffusePositional * scene.cLightPositional * rgbaTex);

    // ambient light added 
    outColor = outColor + (scene.cLightAmbient * rgbaTex);

    //specular light added 
    outColor += specLightDirectional + specLightPositional;
}

