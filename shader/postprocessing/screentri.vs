#version 330

layout(location=11) in vec2 in_Position;

out vec2 vs_out_TexCoord;

void main()
{
	gl_Position.xy = in_Position;
	gl_Position.zw = vec2(0.0, 1.0);

	vs_out_TexCoord = in_Position * 0.5 + 0.5;
}
