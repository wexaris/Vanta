#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

uniform mat4 uViewProjection;

out vec4 vColor;

void main() {
    vColor = aColor;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 330 core

in vec4 vColor;

uniform sampler2D uTexture;

void main() {
    gl_FragColor = vColor;
}
