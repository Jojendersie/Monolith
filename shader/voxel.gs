#version 330

#define OUT_VERTS 12

in uint vs_out_VoxelCode[1];
in uint vs_out_MaterialCode[1];
out vec3 gs_normal;
out vec3 gs_color;
out vec3 gs_viewDirection;

layout(points) in;
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

layout(std140) uniform Camera
{
	mat4 c_mView;
	mat4 c_mProjection;
	mat4 c_mViewProjection;
	vec4 c_vInverseProjection;
	vec3 c_vCameraPosition;
};

void main(void)
{
	float x = float((vs_out_VoxelCode[0] >> 9 ) & uint(0x1f)) + 0.5;
	float y = float((vs_out_VoxelCode[0] >> 14) & uint(0x1f)) + 0.5;
	float z = float((vs_out_VoxelCode[0] >> 19) & uint(0x1f)) + 0.5;
	float type = float(vs_out_VoxelCode[0] >> 24);

	vec4 vPos = vec4( x, y, z, 1 ) * c_mWorldViewProjection;

	// Discard voxel outside the viewing volume
	float w = vPos.w + max(max(length(c_vCorner000), length(c_vCorner100)), max(length(c_vCorner010), length(c_vCorner001)));
	if( vPos.z < -w || vPos.z > w ||
		vPos.x < -w || vPos.x > w ||
		vPos.y < -w || vPos.y > w )
		return;

	gs_viewDirection = normalize(-(vec4(x, y, z, 1) * c_mWorldView).xyz);
////gs_normal = normalize((vec4(x,y,z,1) * c_mWorldView).xyz - c_vCameraPosition);

	// Decode rgb color from material
	float color_y  = float((vs_out_MaterialCode[0]) & uint(0xff)) / 255.0;
	float color_pb = (((vs_out_MaterialCode[0] >> 18) & uint(0x1f)) - 16.0) / 255.0;
	float color_pr = (((vs_out_MaterialCode[0] >> 23) & uint(0x1f)) - 16.0) / 255.0;
	gs_color = vec3(color_y + 0.22627 * color_pb + 11.472 * color_pr,
					color_y - 3.0268 * color_pb - 5.8708 * color_pr,
					color_y + 14.753 * color_pb + 0.0082212 * color_pr);
	gs_color = min(vec3(1,1,1), max(vec3(0,0,0), gs_color));

	// To determine the culling the projective position is inverse transformed
	// back to view space (which is a single mad operation). This direction to
	// the voxel center is used to approximate the view direction to the faces
	// where the scale plays no rule (no normalization required).
	// The 8 corner direction vectors can be added such that the result shows
	// in normal direction in project space. This direction must be rescaled
	// too to be in view space. Then culling is decided by a dot product.
	vec3 vZDir = vPos.xyz * c_vInverseProjection.xyz + vec3(0,0,c_vInverseProjection.w);
	
	if( dot((c_vCorner000.xyz+c_vCorner110.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x10)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,0,-1,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner000.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner100.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner010.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner110.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x20)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,0,1,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner011.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner111.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner001.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner101.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}//*/

	if( dot((c_vCorner010.xyz+c_vCorner111.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x08)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,1,0,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner010.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner110.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner011.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner111.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x04)) != uint(0) )
		{
			gs_normal = normalize((vec4(0,-1,0,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner100.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner000.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner101.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner001.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}//*/

	if( dot((c_vCorner000.xyz+c_vCorner011.xyz)*c_vInverseProjection.xyz, vZDir) < 0 ) {
		if( (vs_out_VoxelCode[0] & uint(0x01)) != uint(0) )
		{
			gs_normal = normalize((vec4(-1,0,0,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner000.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner000 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner010.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner010 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner001.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner001 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner011.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner011 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	} else {
		if( (vs_out_VoxelCode[0] & uint(0x02)) != uint(0) )
		{
			gs_normal = normalize((vec4(1,0,0,0) * c_mWorldView).xyz);
		//gs_normal = normalize(c_vCorner110.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner110 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner100.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner100 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner111.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner111 + vPos;
			EmitVertex();
		//gs_normal = normalize(c_vCorner101.xyz * c_vInverseProjection.xyz);
			gl_Position = c_vCorner101 + vPos;
			EmitVertex();
			EndPrimitive();
		}
	}//*/
}