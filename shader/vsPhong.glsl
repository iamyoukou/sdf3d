#version 330
layout( location = 0 ) in vec3 vtxCoord;
// layout( location = 1 ) in vec3 vtxColor;
layout( location = 2 ) in vec3 vtxNormal;

// out vec2 uv;
out vec3 lightDir;
out vec3 viewDir;
out vec3 normal;
// out vec3 color;

uniform mat4 M, V, P;
uniform vec3 lightPos;
uniform vec3 eyePoint;

void main(){
    //projection plane
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );

    // uv = texUv;

    lightDir = (M * vec4(lightPos - vtxCoord, 1.0)).xyz;
    lightDir = normalize(lightDir);

    // eyePoint is already in world space, so no need to multiply M
    // only vertex need to be multiplied by M
    viewDir = (vec4(eyePoint, 1.0) - M * vec4(vtxCoord, 1.0)).xyz;

    normal = (vec4(vtxNormal, 1.0) * inverse(M)).xyz;

    // color = vtxColor;
}
