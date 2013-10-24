#include "samplerstate.hpp"
#include "../opengl.hpp"
#include <cassert>

namespace Graphic {

	SamplerState::SamplerState( EDGE_TREATMENT _EdgeMode, SAMPLE _MinFilter, SAMPLE _MagFilter, SAMPLE _MipFilter )
	{
		glGenSamplers(1, &m_iSampler);

		//glBindSampler(_dwLocation, _dwSampler);?
		glSamplerParameteri(m_iSampler, GL_TEXTURE_WRAP_S, (int)_EdgeMode);
		glSamplerParameteri(m_iSampler, GL_TEXTURE_WRAP_T, (int)_EdgeMode);
		glSamplerParameteri(m_iSampler, GL_TEXTURE_WRAP_R, (int)_EdgeMode);

		if( _MipFilter == SAMPLE::POINT )
			glSamplerParameteri(m_iSampler, GL_TEXTURE_MIN_FILTER,
			(_MinFilter == SAMPLE::POINT)? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST );
		else if( _MipFilter == SAMPLE::LINEAR )
			glSamplerParameteri(m_iSampler, GL_TEXTURE_MIN_FILTER,
			(_MinFilter == SAMPLE::POINT)? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR );
		glSamplerParameteri(m_iSampler, GL_TEXTURE_MAG_FILTER, unsigned(_MagFilter));

		m_iHash = int(_EdgeMode) + int(_MinFilter) * 10 + int(_MagFilter) * 100 + int(_MipFilter) * 1000;
	}

	SamplerState::~SamplerState()
	{
		glDeleteSamplers(1, &m_iSampler);
	}


	void SamplerState::Apply( unsigned _iLocation ) const
	{
		glBindSampler(_iLocation, m_iSampler);
	}
};