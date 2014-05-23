#include "scissor.hpp"
#include "opengl.hpp"
#include "device.hpp"
#include "..\..\math\math.hpp"

namespace Graphic {

// ************************************************************************* //
ScissorRect::ScissorRect( float _x, float _y, float _w, float _h )
{
	// Store old state to restore it later
	m_wasEnabled = GL_RET_CALL(glIsEnabled, GL_SCISSOR_TEST) == GL_TRUE;
	GL_CALL(glGetIntegerv, GL_SCISSOR_BOX, m_previousRect); // TODO: Avoid glgetter

	Math::IVec2 size = Device::GetBackbufferSize();
	// Translate coordinates to absolute pixel coordinates and set directly.
	GL_CALL(glScissor, GLint((_x * 0.5f + 0.5f) * size[0]),
			   GLint((_y * 0.5f + 0.5f) * size[1]),
			   GLsizei(_w * 0.5f * size[0]),
			   GLsizei(_h * 0.5f * size[1]) );

	GL_CALL(glEnable, GL_SCISSOR_TEST);
}

// ************************************************************************* //
ScissorRect::~ScissorRect()
{
	if( !m_wasEnabled )
		// When disabled there is no need to reset the previous rectangle
		GL_CALL(glDisable, GL_SCISSOR_TEST);
	else
		GL_CALL(glScissor, m_previousRect[0], m_previousRect[1], m_previousRect[2], m_previousRect[3]);
}

} // namespace Graphic