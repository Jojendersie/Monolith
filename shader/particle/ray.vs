#version 330

layout(location=0) in vec3 in_Position;
layout(location=3) in vec4 in_Color;
layout(location=1) in float in_Size;
layout(location=4) in vec3 in_Direction;

out vec4 vs_out_Color;
out vec3 vs_out_Position;
out vec3 vs_out_Direction;
out float vs_out_Size;

void main()
{
	vs_out_Position = in_Position;
	vs_out_Color = in_Color;
	vs_out_Direction = in_Direction;
	vs_out_Size = in_Size;
}
