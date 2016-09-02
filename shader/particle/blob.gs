#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in vec4 vs_out_Color[1];
in float vs_out_Size[1];

layout(std140) uniform Object
{
	mat4 c_mWorldViewProjection;
};

#include "../globalubo.glsl"

out vec4 gs_color;
out vec2 gs_texCoord;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void)
{
	// Pass through
	gs_color = vs_out_Color[0];
	
	vec4 position = vec4(vs_out_Position[0].xyz, 1) * c_mWorldViewProjection;
//	position.xyz /= abs(position.w);
	
	float size = vs_out_Size[0];
	gs_texCoord = vec2(-1.0, -1.0);
	gl_Position = position + vec4(-size, -size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(1.0, -1.0);
	gl_Position = position + vec4( size, -size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(-1.0, 1.0);
	gl_Position = position + vec4(-size,  size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(1.0, 1.0);
	gl_Position = position + vec4( size,  size * c_fAspect, 0, 0);
	EmitVertex();
	EndPrimitive();
}