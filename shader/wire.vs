#version 330

layout(location=0) in vec3 in_Position;
layout(location=3) in vec4 in_Color;
out vec4 vs_out_Color;
out vec3 vs_out_Position;

void main()
{
	vs_out_Position = in_Position;
	vs_out_Color = in_Color;
}
