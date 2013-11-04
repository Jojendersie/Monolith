#version 330


layout(location=3) in vec4 in_Color;
layout(location=11) in vec2 in_Position;
layout(location=12) in vec2 in_TexCoord;
layout(location=13) in vec2 in_Size;
layout(location=14) in float in_Thickness;
layout(location=15) in float in_Scale;

out vec4 vs_out_Color;
out vec2 vs_out_Position;
out vec2 vs_out_TexCoord;
out vec2 vs_out_Size;
out float vs_out_Thickness;
out float vs_out_Scale;

out uint vs_out_VoxelCode;

void main()
{
	// Pass through everything
	vs_out_Color = in_Color;
	vs_out_Position = in_Position;
	vs_out_TexCoord = in_TexCoord;
	vs_out_Size = in_Size;
	vs_out_Thickness = in_Thickness;
	vs_out_Scale = in_Scale;
}
