#version 330

#include "globalubo.glsl"

layout(location=7) in uint in_VoxelCode;
layout(location=8) in uint in_MaterialCode;
flat out uint gs_materialCode;	// The material code
flat out vec3 gs_normal;
flat out vec4 gs_viewDir_phase;

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

void main()
{	
	float x = float((in_VoxelCode >> 6) & uint(0x3f)) + 0.5;
	float y = float((in_VoxelCode >> 12) & uint(0x3f)) + 0.5;
	float z = float((in_VoxelCode >> 18) & uint(0x3f)) + 0.5;
	
	vec4 vPos = vec4(x, y, z, 1) * c_mWorldView;
	vec3 vViewPos = vPos.xyz;
	gl_Position = vec4(vPos.xyz * c_vProjection.xyz + vec3(0,0,c_vProjection.w), vPos.z);
	
	gs_normal = normalize((vec4(0,0,-1,0) * c_mWorldView).xyz);
	gs_materialCode = in_MaterialCode;
	gs_viewDir_phase.xyz = normalize(-vViewPos);
	gs_viewDir_phase.w = mod(hash(in_VoxelCode), uint(6283)) * 0.001;
}
