#version 330

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec3 vColor;

uniform mat4 mvp;

out vec3 fragmentColor;

void main(){
    gl_Position = mvp * vec4( vPosition, 1.0 );
    fragmentColor = vec3(0, dot(normalize(vNormal), normalize(vec3(1.0, 0.0, 0.0))), 0);
}
