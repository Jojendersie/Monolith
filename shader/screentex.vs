#version 330


layout(location=11) in vec2 in_Position;
layout(location=12) in vec2 in_Position2;
layout(location=13) in vec2 in_TexCoord;
layout(location=14) in vec2 in_Size;

out vec2 vs_out_Position;
out vec2 vs_out_Position2;
out vec2 vs_out_TexCoord;
out vec2 vs_out_Size;

void main()
{
	// Pass through everything
	vs_out_Position = in_Position;
	vs_out_Position2 = in_Position2;
	vs_out_TexCoord = in_TexCoord;
	vs_out_Size = in_Size;
}
