#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;

layout (std140, binding = 0) uniform Camera 
{
	mat4 u_ViewProjection;
}

layout (std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
}

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	
}

layout (location = 0) out VertexOutput o_Output;

void main()
{
	o_Output.Color = a_Color;
	o_Output.TexCoord = a_TexCoord;
	o_Output.TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}