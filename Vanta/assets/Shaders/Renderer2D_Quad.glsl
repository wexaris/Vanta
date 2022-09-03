#type vertex
#version 430 core

uniform mat4 uViewProjection;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in uint aTexID;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

struct VertexOutput {
	vec4 Color;
	vec2 TexCoords;
	float TilingFactor;
};

layout(location = 0) out VertexOutput Output;
layout(location = 3) out flat uint vTexID;
layout(location = 4) out flat int vEntityID;

void main() {
    Output.Color = aColor;
    Output.TexCoords = aTexCoords;
    Output.TilingFactor = aTilingFactor;

    vTexID = aTexID;
    vEntityID = aEntityID;

    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 430 core

uniform sampler2D uTextures[16];

struct VertexOutput {
	vec4 Color;
	vec2 TexCoords;
	float TilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat uint vTexID;
layout(location = 4) in flat int vEntityID;

layout(location = 0) out vec4 fColor;
layout(location = 1) out int fEntity;

void main() {
    vec4 texColor = texture(uTextures[vTexID], Input.TexCoords * Input.TilingFactor);

    if (texColor.a == 0.0)
		discard;

    fColor = Input.Color * texColor;
    fEntity = vEntityID;
}
