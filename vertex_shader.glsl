#version 330

const int POS_LOC = 8;
const int CLR_LOC = 9;

layout( location = 8 ) in vec3 vPosition;
layout( location = 9 ) in vec3 vColor;

uniform mat4 mvp;

out vec3 fragmentColor;

void main(){
    gl_Position = mvp * vec4( vPosition, 1.0 );
    fragmentColor = vColor;
}
