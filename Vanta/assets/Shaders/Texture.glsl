#type vertex
#version 430 core

uniform mat4 uViewProjection;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in uint aTexID;
layout(location = 3) in vec2 aTexCoords;

out vec4 vColor;
flat out uint vTexID;
out vec2 vTexCoords;

void main() {
    vColor = aColor;
    vTexID = aTexID;
    vTexCoords = aTexCoords;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 430 core

const uint MAX_TEXTURES = 16u;

uniform sampler2D uTextures[MAX_TEXTURES];

in vec4 vColor;
flat in uint vTexID;
in vec2 vTexCoords;

out vec4 color;

void main() {
    vec4 texColor = texture(uTextures[vTexID], vTexCoords);
    color = vColor * texColor;
}
