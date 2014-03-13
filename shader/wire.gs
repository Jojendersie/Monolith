#version 330

#define OUT_VERTS 4

in vec4 vs_out_Color[2];
in vec3 vs_out_Position[2];

out vec4 gs_Color;
out vec2 gs_TexCoord;

layout(lines) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;

layout(std140) uniform Object
{
	mat4 c_mWorldViewProjection;
	mat4 c_mWorldView;
	vec4 c_vCorner000;
	vec4 c_vCorner001;
	vec4 c_vCorner010;
	vec4 c_vCorner011;
	vec4 c_vCorner100;
	vec4 c_vCorner101;
	vec4 c_vCorner110;
	vec4 c_vCorner111;
};

void main(void)
{
	// Transform to projection space - the rest is done directly in screen space
	vec4 l1 = vec4(vs_out_Position[0], 1) * c_mWorldViewProjection;
	vec4 l2 = vec4(vs_out_Position[1], 1) * c_mWorldViewProjection;
	
	// Compute a vector perpendicular vector to create a beam
	vec2 direction = normalize(l2.xy - l1.xy);
	// Cross product with view direction
	vec4 perpendicular = vec4(-direction.y * 0.1, direction.x * 0.1, 0, 0);

	gs_Color = vs_out_Color[0];
	gl_Position = l1 + perpendicular * sqrt(l1.w);
	gs_TexCoord = vec2(-1.0, -1.0);
	EmitVertex();
	gl_Position = l1 - perpendicular * sqrt(l1.w);
	gs_TexCoord = vec2(-1.0, 1.0);
	EmitVertex();
	gs_Color = vs_out_Color[1];
	gl_Position = l2 + perpendicular * sqrt(l2.w);
	gs_TexCoord = vec2(1.0, -1.0);
	EmitVertex();
	gl_Position = l2 - perpendicular * sqrt(l2.w);
	gs_TexCoord = vec2(1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}