#type vertex
#version 430 core

uniform mat4 uViewProjection;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in uint aTexID;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

out vec4 vColor;
out flat uint vTexID;
out vec2 vTexCoords;
out float vTilingFactor;
out flat int vEntityID;

void main() {
    vColor = aColor;
    vTexID = aTexID;
    vTexCoords = aTexCoords;
    vTilingFactor = aTilingFactor;
    vEntityID = aEntityID;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 430 core

const uint MAX_TEXTURES = 16u;

uniform sampler2D uTextures[MAX_TEXTURES];

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec4 vColor;
in flat uint vTexID;
in vec2 vTexCoords;
in float vTilingFactor;
in flat int vEntityID;

void main() {
    vec4 texColor = texture(uTextures[vTexID], vTexCoords * vTilingFactor);
    color = vColor * texColor;
    entity = vEntityID;
}
