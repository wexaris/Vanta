#type vertex
#version 450 core

layout(std140, binding = 0) uniform Camera {
    mat4 uViewProjection;
};

struct VertexOutput {
    vec4 Color;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in int aEntityID;

layout(location = 0) out VertexOutput Output;
layout(location = 1) out flat int vEntityID;

void main() {
    Output.Color = aColor;
    vEntityID = aEntityID;

    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 450 core

struct VertexOutput {
    vec4 Color;
};

layout(location = 0) in VertexOutput Input;
layout(location = 1) in flat int vEntityID;

layout(location = 0) out vec4 fColor;
layout(location = 1) out int fEntity;

void main() {
    fColor = Input.Color;
    fEntity = vEntityID;
}
