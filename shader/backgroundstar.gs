#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in vec4 vs_out_Color[1];
in float vs_out_Size[1];

layout(location = 0) uniform bool c_ambientStars;


out vec4 gs_color;
out vec2 gs_texCoord;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

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

	projectedPos = projectedPos / abs(position.z);
	float size = vs_out_Size[0] / 500.0;
	gs_texCoord = vec2(-1.0, -1.0);
	gl_Position = projectedPos + vec4(-size, -size * c_fAspect, 0, 1);
	EmitVertex();
	gs_texCoord = vec2(1.0, -1.0);
	gl_Position = projectedPos + vec4( size, -size * c_fAspect, 0, 1);
	EmitVertex();
	gs_texCoord = vec2(-1.0, 1.0);
	gl_Position = projectedPos + vec4(-size,  size * c_fAspect, 0, 1);
	EmitVertex();
	gs_texCoord = vec2(1.0, 1.0);
	gl_Position = projectedPos + vec4( size,  size * c_fAspect, 0, 1);
	EmitVertex();
	EndPrimitive();
}