#include "framebuffer.hpp"
#include "texture.hpp"
#include "opengl.hpp"
#include "math/vector.hpp"
#include "device.hpp"

#include <cassert>
#include <memory>

namespace Graphic {

Framebuffer::Attachment::Attachment(Texture* _pTexture, unsigned int _mipLevel, unsigned int _layer) :
	pTexture(_pTexture), mipLevel(_mipLevel), layer(_layer)
{
	assert(_layer < pTexture->Depth() && "Can't bind non-existing texture layer to framebuffer!");
	assert(_mipLevel < pTexture->MipLevels() && "Can't bind non-existing texture mipLevel to framebuffer!");
}

Framebuffer::Framebuffer(const Attachment& _colorAttachment, const Attachment& _depthStencilAttachment, bool _depthWithStencil) :
	m_depthStencil(_depthStencilAttachment)
{
	std::vector<Attachment> colorAttachments;
	colorAttachments.push_back(_colorAttachment);
	Initialize(colorAttachments, _depthWithStencil);
}

Framebuffer::Framebuffer(const std::vector<Attachment>& _colorAttachments, const Attachment& _depthStencilAttachment, bool _depthWithStencil) :
	m_depthStencil(_depthStencilAttachment)
{
	Initialize(_colorAttachments, _depthWithStencil);
}

void Framebuffer::Initialize(const std::vector<Attachment>& _colorAttachments, bool _depthWithStencil)
{
	/// \todo Plaster with glGetError function

	glGenFramebuffers(1, &m_framebuffer);

	// Need to bind framebuffer to be able to create it.
	Device::BindFramebuffer(this, true);

	if (m_depthStencil.pTexture)
	{
		GLint attachment = _depthWithStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

		// Sufficient for MSAA?
		if (m_depthStencil.layer > 0)
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, m_depthStencil.pTexture->m_textureID, m_depthStencil.mipLevel, m_depthStencil.layer);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_depthStencil.pTexture->m_textureID, m_depthStencil.mipLevel);
	}


	for (auto it = _colorAttachments.begin(); it != _colorAttachments.end(); ++it)
	{
		assert(it->pTexture && "FBO Color attachment texture is NULL!");
		GLint attachment = GL_COLOR_ATTACHMENT0 + m_colorAttachments.size();
		if (it->layer > 0)
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, it->pTexture->m_textureID, it->mipLevel, it->layer);
		else
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, it->pTexture->m_textureID, it->mipLevel);

		m_colorAttachments.push_back(*it);
	}

	// setup draw buffers
	std::unique_ptr<GLuint[]> drawBuffers(new GLuint[m_colorAttachments.size()]);
	for (size_t i = 0; i < m_colorAttachments.size(); ++i)
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(m_colorAttachments.size(), drawBuffers.get());

	// Set read buffers to first color target.
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// Error checking
#ifdef _DEBUG
	assert(m_depthStencil.pTexture != NULL || m_colorAttachments.size() > 0 && "You cannot create empty FBOs! Need at least a depth/stencil buffer or a color attachment.");
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (framebufferStatus)
	{
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		Jo::Logger::g_logger.Write("Error", "Not all framebuffer attachment points are framebuffer attachment complete.This means that at least one "
			"attachment point with a renderbuffer or texture  attached has its attached object no longer in existence "
			"or has an attached image with a width or height of zero, or the color attachment point has a "
			"non - color - renderable image attached, or the depth attachment point has a non - depth - renderable "
			"image attached, or the stencil attachment point has a non - stencil - renderable image attached. "
			"Color - renderable formats include GL_RGBA4, GL_RGB5_A1, and GL_RGB565."
			"GL_DEPTH_COMPONENT16 is the only depth - renderable format.GL_STENCIL_INDEX8 is the only stencil - renderable format.");
		break;
	//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		Jo::Logger::g_logger.Write("Error", "Not all attached images have the same width and height.");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		Jo::Logger::g_logger.Write("Error", "No images are attached to the framebuffer.");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		Jo::Logger::g_logger.Write("Error", "The combination of internal formats of the attached images violates an implementation - dependent set of restrictions.");
		break;
	}
	assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE && "Frame buffer creation failed!");

	LogGlError("Framebuffer creation");
#endif
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_framebuffer);
}

/*
void Framebuffer::BlitTo(Framebuffer* pDest, const ezRectU32& srcRect, const ezRectU32& dstRect, bool depthStencil, GLuint filter)
{
	if (pDest == NULL)
		BindBackBuffer();
	else
		pDest->Bind(false);
	if (s_BoundFrameBufferRead != this)
	{
		glBindFramebuffer(GL_READ_BUFFER, m_framebuffer);
		s_BoundFrameBufferRead = this;
	}

	glBlitFramebuffer(srcRect.x, srcRect.y, srcRect.x + srcRect.width, srcRect.y + srcRect.height,
		dstRect.x, dstRect.y, dstRect.x + dstRect.width, dstRect.y + dstRect.height, depthStencil ? (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) : GL_COLOR_BUFFER_BIT, filter);

#ifdef _DEBUG
	LogGlError("Framebuffer blitting");
#endif
}
*/
}