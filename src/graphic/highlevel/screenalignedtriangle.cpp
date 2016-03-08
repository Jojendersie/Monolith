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
		ScreenTriVertex* screenTriangle = (ScreenTriVertex*)malloc(3 * sizeof(screenTriangle));
		screenTriangle[0].position[0] = -1.0f;
		screenTriangle[0].position[1] = -1.0f;
		screenTriangle[1].position[0] = 3.0f;
		screenTriangle[1].position[1] = -1.0f;
		screenTriangle[2].position[0] = -1.0f;
		screenTriangle[2].position[1] = 3.0f;

		m_vertexBuffer = new VertexArrayBuffer(VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST, {{VertexAttribute::VEC2, 11}});
		m_vertexBuffer->GetBuffer(0)->SetData((void*&)screenTriangle, sizeof(ScreenTriVertex) * 3);
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