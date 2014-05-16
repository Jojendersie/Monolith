#define saturate(x) clamp(x, 0, 1)

#define PI 3.14159265359
#define PI_2 6.28318530718

// Basically removes the w division from z again.
// Good explanation: http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
float LinearizeZBufferDepth(float depthFromZBuffer)
{
	// TODO: Precompute stuff
	return (2.0 * c_NearPlane * c_FarPlane) / (c_FarPlane + c_NearPlane - (2.0 * depthFromZBuffer - 1.0) * (c_FarPlane - c_NearPlane));
}