#include "samplerstate.hpp"
#include "opengl.hpp"
#include "utilities/assert.hpp"

namespace Graphic {

	SamplerState::SamplerState( EDGE_TREATMENT _edgeMode, SAMPLE _minFilter, SAMPLE _magFilter, SAMPLE _mipFilter )
	{
		GL_CALL(glGenSamplers, 1, &m_sampler);

		//glBindSampler(_dwLocation, _dwSampler);?
		GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_WRAP_S, (int)_edgeMode);
		GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_WRAP_T, (int)_edgeMode);
		GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_WRAP_R, (int)_edgeMode);

		if( _mipFilter == SAMPLE::POINT )
			GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_MIN_FILTER,
			(_minFilter == SAMPLE::POINT) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
		else if( _mipFilter == SAMPLE::LINEAR )
			GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_MIN_FILTER,
			(_minFilter == SAMPLE::POINT)? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR );
		GL_CALL(glSamplerParameteri, m_sampler, GL_TEXTURE_MAG_FILTER, unsigned(_magFilter));

		m_hash = int(_edgeMode) + int(_minFilter) * 10 + int(_magFilter) * 100 + int(_mipFilter) * 1000;
	}

	SamplerState::~SamplerState()
	{
		GL_CALL(glDeleteSamplers, 1, &m_sampler);
	}


	void SamplerState::Apply( unsigned _location ) const
	{
		GL_CALL(glBindSampler, _location, m_sampler);
	}
};