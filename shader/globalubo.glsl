layout(std140) uniform Global
{
	float c_fAspect;			// Width / Height
	float c_fBBWidth;			// Backbuffer width
	float c_fBBHeight;			// Backbuffer height
	float c_fBBInverseWidth;	// Backbuffer 1/width
	float c_fBBInverseHeight;	// Backbuffer 1/height
	float c_fTime;				// Total game time in seconds
};

layout(std140) uniform Camera
{
	mat3x3 c_mInverseView;
	vec4 c_vProjection;
	vec4 c_vInverseProjection;
	float c_NearPlane;
	float c_FarPlane;
};