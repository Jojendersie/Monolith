#version 330


layout(location=0) in vec3 in_Position;
layout(location=3) in vec4 in_Color;
layout(location=11) in float in_Size;

out vec4 vs_out_Color;
out vec3 vs_out_Position;
out float vs_out_Size;

void main()
{
	// Pass through everything
	// Color is overwritten any way
	vs_out_Color = in_Color;
	vs_out_Position = in_Position;
	vs_out_Size = in_Size;
}
