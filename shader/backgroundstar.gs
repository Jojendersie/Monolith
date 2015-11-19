#version 330
#extension GL_ARB_explicit_uniform_location : enable

#define OUT_VERTS 1

in vec3 vs_out_Position[1];
in vec4 vs_out_Color[1];
in float vs_out_Size[1];

layout(location = 0) uniform bool c_ambientStars;


out vec4 gs_color;

layout(points) in;
layout(points, max_vertices = OUT_VERTS) out; //triangle_strip

#include "globalubo.glsl"

void main(void)
{
	// Pass through
	gs_color = vs_out_Color[0];
	
	// Transform background and galaxy stars differently
	vec3 position;
	if(c_ambientStars)
		position = vs_out_Position[0].xyz * c_mCameraRotation;
	else
		// Positions are transformed before calling the shader
		position = vs_out_Position[0].xyz;

	vec4 projectedPos = vec4(position * c_vProjection.xyz + vec3(0, 0, c_vProjection.w), position.z);
	
	// Always visible (no farplane)
	if (projectedPos[2] > 0) projectedPos[2] = 0;

	gl_Position = projectedPos;
	//currently ignored; call glEnable (GL_PROGRAM_POINT_SIZE)
	gl_PointSize = vs_out_Size[0];

	EmitVertex();
	EndPrimitive();
}