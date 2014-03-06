#version 330

#define OUT_VERTS 4

in vec4 vs_out_Color[1];
in vec2 vs_out_Position[1];
in vec2 vs_out_TexCoord[1];
in vec2 vs_out_Size[1];
in float vs_out_Thickness[1];
in float vs_out_Scale[1];

out vec2 gs_texCoord;
out float gs_thickness;
out vec4 gs_color;

layout(points) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;

layout(std140) uniform Global
{
	float c_fAspect;	// Width / Height
	float c_fFBWidth;	// Framebuffer width
	float c_fFBHeight;	// Framebuffer height
	float c_fTime;		// Total game time in seconds
};

void main(void)
{
	float w = vs_out_Size[0].x * vs_out_Scale[0];
	float h = vs_out_Size[0].y * vs_out_Scale[0] / c_fAspect;

	// Pass through
	gs_thickness = vs_out_Thickness[0];
	gs_color = vs_out_Color[0];

	gs_texCoord = vs_out_TexCoord[0];
	gl_Position = vec4(vs_out_Position[0], 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0] + vec2(vs_out_Size[0].x,0);
	gl_Position = vec4(vs_out_Position[0] + vec2(w,0), 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0] + vec2(0, vs_out_Size[0].y);
	gl_Position = vec4(vs_out_Position[0] + vec2(0,h), 0, 1);
	EmitVertex();
	gs_texCoord = vs_out_TexCoord[0] + vs_out_Size[0];
	gl_Position = vec4(vs_out_Position[0] + vec2(w,h), 0, 1);
	EmitVertex();
	EndPrimitive();
}