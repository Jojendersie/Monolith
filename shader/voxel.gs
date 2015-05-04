#version 330

#define OUT_VERTS 12

in uint vs_out_VoxelCode[1];
in uint vs_out_MaterialCode[1];
flat out vec3 gs_objectPosition;
flat out ivec3 gs_voxel_material_mipmap;
out vec3 gs_texCoord;
//flat out float gs_sideLength;

layout(points) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;

#include "globalubo.glsl"

layout(std140) uniform Object
{
	mat4 c_mWorldView;
	mat4 c_mInverseWorldView;
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
	float x = float((vs_out_VoxelCode[0] >> 6 ) & uint(0x3f)) + 0.5;
	float y = float((vs_out_VoxelCode[0] >> 12) & uint(0x3f)) + 0.5;
	float z = float((vs_out_VoxelCode[0] >> 18) & uint(0x3f)) + 0.5;

	vec4 vPos = vec4(x, y, z, 1) * c_mWorldView;
	vec3 vViewPos = vPos.xyz;
	vPos = vec4(vPos.xyz * c_vProjection.xyz + vec3(0,0,c_vProjection.w), vPos.z);

	// Discard voxel outside the viewing volume
	float w = vPos.w + c_fMaxOffset;//max(max(length(c_vCorner000), length(c_vCorner001)), max(length(c_vCorner010), length(c_vCorner011)));
	if( abs(vPos.z) > w ||
		abs(vPos.x) > w ||
		abs(vPos.y) > w )
		return;

	gs_objectPosition = vec3(x, y, z);
	gs_voxel_material_mipmap.y = int(vs_out_MaterialCode[0]);
	gs_voxel_material_mipmap.x = int(vs_out_VoxelCode[0]);
	gs_voxel_material_mipmap.z = clamp(int(log2(vPos.z * c_vInverseProjection.y / 16)), 0, 4);
	
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
			gl_Position = c_vCorner000 + vPos;
			gs_texCoord = vec3(0,0,0);
			EmitVertex();
			gl_Position = c_vCorner100 + vPos;
			gs_texCoord = vec3(1,0,0);
			EmitVertex();
			gl_Position = c_vCorner010 + vPos;
			gs_texCoord = vec3(0,1,0);
			EmitVertex();
			gl_Position = c_vCorner110 + vPos;
			gs_texCoord = vec3(1,1,0);
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x20)) != uint(0) )
		{
			gl_Position = c_vCorner011 + vPos;
			gs_texCoord = vec3(0,1,1);
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			gs_texCoord = vec3(1,1,1);
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			gs_texCoord = vec3(0,0,1);
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			gs_texCoord = vec3(1,0,1);
			EmitVertex();
			EndPrimitive();
		}
	}

	if( dot((c_vCorner010.xyz+c_vCorner111.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x08)) != uint(0) )
		{
			gl_Position = c_vCorner010 + vPos;
			gs_texCoord = vec3(0,1,0);
			EmitVertex();
			gl_Position = c_vCorner110 + vPos;
			gs_texCoord = vec3(1,1,0);
			EmitVertex();
			gl_Position = c_vCorner011 + vPos;
			gs_texCoord = vec3(0,1,1);
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			gs_texCoord = vec3(1,1,1);
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x04)) != uint(0) )
		{
			gl_Position = c_vCorner100 + vPos;
			gs_texCoord = vec3(1,0,0);
			EmitVertex();
			gl_Position = c_vCorner000 + vPos;
			gs_texCoord = vec3(0,0,0);
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			gs_texCoord = vec3(1,0,1);
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			gs_texCoord = vec3(0,0,1);
			EmitVertex();
			EndPrimitive();
		}
	}

	if( dot((c_vCorner000.xyz+c_vCorner011.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x01)) != uint(0) )
		{
			gl_Position = c_vCorner000 + vPos;
			gs_texCoord = vec3(0,0,0);
			EmitVertex();
			gl_Position = c_vCorner010 + vPos;
			gs_texCoord = vec3(0,1,0);
			EmitVertex();
			gl_Position = c_vCorner001 + vPos;
			gs_texCoord = vec3(0,0,1);
			EmitVertex();
			gl_Position = c_vCorner011 + vPos;
			gs_texCoord = vec3(0,1,1);
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x02)) != uint(0) )
		{
			gl_Position = c_vCorner110 + vPos;
			gs_texCoord = vec3(1,1,0);
			EmitVertex();
			gl_Position = c_vCorner100 + vPos;
			gs_texCoord = vec3(1,0,0);
			EmitVertex();
			gl_Position = c_vCorner111 + vPos;
			gs_texCoord = vec3(1,1,1);
			EmitVertex();
			gl_Position = c_vCorner101 + vPos;
			gs_texCoord = vec3(1,0,1);
			EmitVertex();
			EndPrimitive();
		}
	}
}