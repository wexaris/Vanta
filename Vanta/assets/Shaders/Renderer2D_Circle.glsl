#type vertex
#version 430 core

uniform mat4 uViewProjection;

layout(location = 0) in vec3 aWorldPosition;
layout(location = 1) in vec3 aLocalPosition;
layout(location = 2) in vec4 aColor;
layout(location = 3) in float aThickness;
layout(location = 4) in float aFade;
layout(location = 5) in int aEntityID;

struct VertexOutput {
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout(location = 0) out VertexOutput Output;
layout(location = 4) out flat int vEntityID;

void main() {
    Output.LocalPosition = aLocalPosition;
    Output.Color = aColor;
    Output.Thickness = aThickness;
    Output.Fade = aFade;

    vEntityID = aEntityID;

    gl_Position = uViewProjection * vec4(aWorldPosition, 1.0);
}


#type fragment
#version 430 core

struct VertexOutput {
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int vEntityID;

layout(location = 0) out vec4 fColor;
layout(location = 1) out int fEntity;

void main() {
    // Calculate distance and fill circle with white
    float dist = 1.0 - length(Input.LocalPosition);
    float circle = smoothstep(0.0, Input.Fade, dist);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, dist);

    if (circle == 0.0)
		discard;

    fColor = vec4(Input.Color.xyz, Input.Color.a * Input.Color);
    fEntity = vEntityID;
}
