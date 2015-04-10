#version 330

#define OUT_VERTS 1

in vec4 vs_out_Color[1];
in vec3 vs_out_Position[1];
in float vs_out_Size[1];


out vec4 gs_color;

layout(points) in;
layout(points, max_vertices = OUT_VERTS) out; //triangle_strip

#include "globalubo.glsl"

void main(void)
{
	// Pass through
	gs_color = vs_out_Color[0];
	//currently ignored; call glEnable (GL_PROGRAM_POINT_SIZE)
	gl_PointSize = 5;
//	vec4 projectedPos = vec4(vs_out_Position[0],1);
	vec4 projectedPos = vec4(vs_out_Position[0].xyz * c_vProjection.xyz + vec3(0, 0, c_vProjection.w), vs_out_Position[0].z);
	
	//always visible
	projectedPos[2] = 0;
	gl_Position = projectedPos;
	EmitVertex();
	gl_Position = projectedPos + vec4(0, -1, 0, 0);
	EmitVertex();
	gl_Position = projectedPos + vec4(1.0, 0.0, 0, 0);
	EmitVertex();
	gl_Position = projectedPos + vec4(1.0, -1.0, 0, 0);
	EmitVertex();
	EndPrimitive();
}