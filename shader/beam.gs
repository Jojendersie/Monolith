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
	float c_fLineWidth;
	float c_fBlendSlope;
	float c_fBlendOffset;
};

void main(void)
{
	// Transform to projection space - the rest is done directly in screen space
	vec4 l1 = vec4(vs_out_Position[0], 1) * c_mWorldViewProjection;
	vec4 l2 = vec4(vs_out_Position[1], 1) * c_mWorldViewProjection;
	
	// Do a manual clipping.
	float threshold = -5.0; // TODO: use nearplane
	vec4 direction = normalize(l2 - l1);
	// Compute original distance for texture coordinate reprojection.
	float len = length(l2.xyz - l1.xyz) * 0.5;
	// Reproject end points to the near-thresholdplane
	if( l1.z < threshold )
		l1 -= direction * (l1.z-threshold) / direction.z;
	float len2 = length(l2.xyz - l1.xyz);
	float tex1 = 1.0 - len2 / len;
	if( l2.z < threshold )
		l2 -= direction * (l2.z-threshold) / direction.z;
	float tex2 = length(l2.xyz - l1.xyz) / len2 * 2.0 - 1.0;

	// Compute a vector perpendicular vector to create a beam
	vec2 dir = normalize(l2.xy / l2.w - l1.xy / l1.w);
	// Cross product with view direction
	vec4 perpendicular = vec4(-dir.y * c_fLineWidth, dir.x * c_fLineWidth, 0, 0);

	gs_Color = vs_out_Color[0];
	gl_Position = l1 + perpendicular;
	gs_TexCoord = vec2(tex1, -1.0);
	EmitVertex();
	
	gl_Position = l1 - perpendicular;
	gs_TexCoord = vec2(tex1, 1.0);
	EmitVertex();
	
	gs_Color = vs_out_Color[1];
	gl_Position = l2 + perpendicular;
	gs_TexCoord = vec2(tex2, -1.0);
	EmitVertex();
	
	gl_Position = l2 - perpendicular;
	gs_TexCoord = vec2(tex2, 1.0);
	EmitVertex();
	EndPrimitive();
}