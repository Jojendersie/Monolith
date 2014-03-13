#version 330

layout(location=0) in vec3 in_Position;
layout(location=3) in vec4 in_Color;
out vec4 vs_out_Color;
out vec3 vs_out_Position;

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

void main()
{
	//vs_out_Position = in_Position;
	gl_Position = vec4(in_Position, 1) * c_mWorldViewProjection;
	vs_out_Color = in_Color;
}
