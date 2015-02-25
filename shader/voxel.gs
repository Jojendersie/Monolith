#version 330

#define OUT_VERTS 12

in uint vs_out_VoxelCode[1];
in uint vs_out_MaterialCode[1];
flat out uint gs_materialCode;
flat out vec3 gs_normal;
flat out vec4 gs_viewDir_phase;

layout(points) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;

#include "globalubo.glsl"

layout(std140) uniform Object
{
	mat4 c_mWorldView;
	vec4 c_vCorner000;
	vec4 c_vCorner001;
	vec4 c_vCorner010;
	vec4 c_vCorner011;
	vec4 c_vCorner100;
	vec4 c_vCorner101;
	vec4 c_vCorner110;
	vec4 c_vCorner111;
	float c_fMaxOffset;
};


// Standard linear congruential generator to hash an integer
uint hash(uint i)
{
	return i * uint(1103515245) + uint(12345);
}

void main(void)
{
	float x = float((vs_out_VoxelCode[0] >> 9 ) & uint(0x1f)) + 0.5;
	float y = float((vs_out_VoxelCode[0] >> 14) & uint(0x1f)) + 0.5;
	float z = float((vs_out_VoxelCode[0] >> 19) & uint(0x1f)) + 0.5;

	vec4 vPos = vec4(x, y, z, 1) * c_mWorldView;
	vec3 vViewPos = vPos.xyz;
	vPos = vec4(vPos.xyz * c_vProjection.xyz + vec3(0,0,c_vProjection.w), vPos.z);

	// Discard voxel outside the viewing volume
	float w = vPos.w + c_fMaxOffset;//max(max(length(c_vCorner000), length(c_vCorner001)), max(length(c_vCorner010), length(c_vCorner011)));
	if( abs(vPos.z) > w ||
		abs(vPos.x) > w ||
		abs(vPos.y) > w )
		return;

	gs_materialCode = vs_out_MaterialCode[0];
	gs_viewDir_phase.xyz = normalize(-vViewPos);
	gs_viewDir_phase.w = mod(hash(vs_out_VoxelCode[0]), uint(6283)) * 0.001;
	
	// To determine the culling the projective position is inverse transformed
	// back to view space (which is a single mad operation). This direction to
	// the voxel center is used to approximate the view direction to the faces
	// where the scale plays no rule (no normalization required).
	// The 8 corner direction vectors can be added such that the result shows
	// in normal direction in project space. This direction must be rescaled
	// too to be in view space. Then culling is decided by a dot product.
	vec3 vZDir = vViewPos;//vPos.xyz * c_vInverseProjection.xyz + vec3(0,0,c_vInverseProjection.w);
	
	if( dot((c_vCorner000.xyz+c_vCorner110.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x10)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,0,-1,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x20)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,0,1,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}

	if( dot((c_vCorner010.xyz+c_vCorner111.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x08)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,1,0,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x04)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,-1,0,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}

	if( dot((c_vCorner000.xyz+c_vCorner011.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x01)) != uint(0) )
		{
			gs_normal = normalize((vec4(-1,0,0,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x02)) != uint(0) )
		{
			gs_normal = normalize((vec4(1,0,0,0) * c_mWorldView).xyz);
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}
}