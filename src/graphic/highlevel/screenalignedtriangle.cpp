#include "screenalignedtriangle.h"
#include "graphic/core/device.hpp"
#include "graphic/core/vertexbuffer.hpp"

namespace Graphic
{
	struct ScreenTriVertex
	{
		float position[2];
	};

	ScreenAlignedTriangle::ScreenAlignedTriangle()
	{
		ScreenTriVertex screenTriangle[3];
		screenTriangle[0].position[0] = -1.0f;
		screenTriangle[0].position[1] = -1.0f;
		screenTriangle[1].position[0] = 3.0f;
		screenTriangle[1].position[1] = -1.0f;
		screenTriangle[2].position[0] = -1.0f;
		screenTriangle[2].position[1] = 3.0f;

		m_vertexBuffer = new VertexBuffer("2", screenTriangle, sizeof(screenTriangle), VertexBuffer::PrimitiveType::TRIANGLE_LIST);
	}

	ScreenAlignedTriangle::~ScreenAlignedTriangle()
	{
		delete m_vertexBuffer;
	}

	void ScreenAlignedTriangle::Draw()
	{
		Device::DrawVertices(*GetInstance().m_vertexBuffer, 0, 3);
	}
}