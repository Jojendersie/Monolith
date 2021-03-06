#pragma once

namespace Graphic {

	/// \brief Helper class for drawing screen aligned triangles
	class ScreenAlignedTriangle
	{
	public:
		/// \brief Draws a screen aligned triangle.
		/// Will overwrite the current set vertex buffer.
		static void Draw();

		~ScreenAlignedTriangle();

	private:
		ScreenAlignedTriangle();

		static ScreenAlignedTriangle& GetInstance()
		{
			static ScreenAlignedTriangle m_instance;
			return m_instance;
		};

		class VertexArrayBuffer* m_vertexBuffer;
	};

} // namespace Graphic