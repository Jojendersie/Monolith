#version 330


layout(location=7) in uint in_VoxelCode;
layout(location=8) in uint in_MaterialCode;
out uint vs_out_VoxelCode;
out uint vs_out_MaterialCode;

void main()
{
	vs_out_VoxelCode = in_VoxelCode;
	vs_out_MaterialCode = in_MaterialCode;
}
