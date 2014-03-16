#pragma once

namespace Graphic {

	/// \brief A class to enable scissor testing within a scope.
	/// \details Just creating an instance is enabling the scissor test until
	///		the object dies again. In destructor the previous settings are
	///		restored. Do use this for local variables only!
	class ScissorRect
	{
	public:
		/// \brief Create a rectangle in screen space. Everything outside is
		///		discarded during rendering.
		/// \details (_x, _y) is the lower left corner of the rectangle.
		ScissorRect( float _x, float _y, float _w, float _h );
		~ScissorRect();

	private:
		bool m_wasEnabled;
		int m_previousRect[4];
	};

} // namespace Graphic