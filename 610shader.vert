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

    float kConst;
} scene;
layout(binding = 1) uniform BodyUniforms {
    mat4 modeling;
    uvec4 texIndices;
    vec4 cSpecular;
} body;

layout(location = 0) in vec3 attrXYZ;
layout(location = 1) in vec2 attrST;
layout(location = 2) in vec3 attrNOP;

layout(location = 0) out vec2 st;
layout(location = 1) out vec3 dNormal;
layout(location = 2) out vec3 uLightPositional;
layout(location = 3) out vec3 uToCamera;
layout(location = 4) out float distance;

void main() {
    // compute the normal vector from vertex
    vec4 homogWorldNormal = body.modeling * vec4(attrNOP, 0.0);
    dNormal = vec3(homogWorldNormal);

    st = attrST;
    vec3 xyzWorld = vec3(body.modeling * vec4(attrXYZ, 1.0));
    uLightPositional = normalize(vec3(scene.pLightPositional) - xyzWorld);

// unit vector pointing to camera
    uToCamera = normalize(vec3(scene.pCamera) - xyzWorld);
    distance = length(vec3(scene.pLightPositional) - xyzWorld);


// sets the position of our fragment
    gl_Position = scene.camera * (body.modeling * vec4(attrXYZ, 1.0));

}

