#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform SceneUniforms {
    mat4 camera;
    vec4 uLightDirectional;
    vec4 cLightDirectional;

    vec4 pLightPositional;
    vec4 cLightPositional;

    vec4 cLightAmbient;
} scene;
layout(binding = 1) uniform BodyUniforms {
    mat4 modeling;
    uvec4 texIndices;
} body;
layout(binding = 2) uniform sampler2D samplers[3];

layout(location = 0) in vec2 st;
layout(location = 1) in vec3 dNormal;
layout(location = 2) in vec3 uLightPositional;

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

    outColor = (iDiffuseDirectional) * (scene.cLightDirectional * rgbaTex);
    outColor = outColor + ((iDiffusePositional) * (scene.cLightPositional * rgbaTex));

    // ambient light added 
    outColor = outColor + (scene.cLightAmbient * rgbaTex);
}

