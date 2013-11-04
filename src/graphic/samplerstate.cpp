#include "samplerstate.hpp"
#include "../opengl.hpp"
#include <cassert>

namespace Graphic {

	SamplerState::SamplerState( EDGE_TREATMENT _edgeMode, SAMPLE _minFilter, SAMPLE _magFilter, SAMPLE _mipFilter )
	{
		glGenSamplers(1, &m_sampler);

		//glBindSampler(_dwLocation, _dwSampler);?
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, (int)_edgeMode);
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, (int)_edgeMode);
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_R, (int)_edgeMode);

		if( _mipFilter == SAMPLE::POINT )
			glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER,
			(_minFilter == SAMPLE::POINT)? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST );
		else if( _mipFilter == SAMPLE::LINEAR )
			glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER,
			(_minFilter == SAMPLE::POINT)? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR );
		glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, unsigned(_magFilter));

		m_hash = int(_edgeMode) + int(_minFilter) * 10 + int(_magFilter) * 100 + int(_mipFilter) * 1000;
	}

	SamplerState::~SamplerState()
	{
		glDeleteSamplers(1, &m_sampler);
	}


	void SamplerState::Apply( unsigned _location ) const
	{
		glBindSampler(_location, m_sampler);
	}
};