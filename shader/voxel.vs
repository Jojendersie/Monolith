#version 330


layout(location=7) in uint in_VoxelCode;
out uint vs_out_VoxelCode;

void main()
{
	vs_out_VoxelCode = in_VoxelCode;
}
