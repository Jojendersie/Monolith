#version 330

#define OUT_VERTS 4

in vec2 vs_out_Position[1];
in vec2 vs_out_Position2[1];
in vec2 vs_out_TexCoord[1];
in vec2 vs_out_Size[1];

out vec2 gs_texCoord;

layout(points) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;


void main(void)
{
	gs_texCoord = vs_out_TexCoord[0] + vec2(0, vs_out_Size[0].y);
	gl_Position = vec4(vs_out_Position[0], 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0];
	gl_Position = vec4(vs_out_Position[0] + vec2(0,-vs_out_Position2[0].y), 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0] + vs_out_Size[0];
	gl_Position = vec4(vs_out_Position[0] + vec2(vs_out_Position2[0].x,0), 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0] + vec2(vs_out_Size[0].x,0);
	gl_Position = vec4(vs_out_Position[0] + vec2(vs_out_Position2[0].x,-vs_out_Position2[0].y), 0, 1);
	EmitVertex();
	EndPrimitive();
}