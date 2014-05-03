#pragma once

#include <vector>

namespace Graphic {

	class Texture;

	/// Abstraction for rendertarget objects
	/// \remarks Not yet supported:
	/// - Bind cubemap faces
	/// - Some MSAA functionality?
	/// - Blit multiple Targets at once
	/// - Clear multiple Targets at once
	/// - ...
	class Framebuffer
	{
	public:
		struct Attachment
		{
			/// \param[in] _mipLevel
			///   Used miplevel of this attachment.
			/// \param[in] _layer
			///   Used array or cubemap depth level of this texture.
			Attachment(Texture* _pTexture, unsigned int _mipLevel = 0, unsigned int _layer = 0);

			Texture* pTexture;
			unsigned int mipLevel;
			unsigned int layer;
		};



		/// \brief Creates a new framebuffer with the given attachments.
		/// Will perform several sanity checks if _DEBUG is defined.
		///
		/// \param [in] depthWithStencil
		///   If true the depthStencilAttachment is expected to contain a stencil buffer.
		Framebuffer(const Attachment& _colorAttachment, const Attachment& _depthStencilAttachment = Attachment(NULL), bool _depthWithStencil = false);

		/// \brief Creates a new framebuffer with the given attachments.
		/// Will perform several sanity checks if _DEBUG is defined.
		///
		/// \param [in] depthWithStencil
		///   If true the depthStencilAttachment is expected to contain a stencil buffer.
		Framebuffer(const std::vector<Attachment>& _colorAttachments, const Attachment& _depthStencilAttachment = Attachment(NULL), bool _depthWithStencil = false);

		~Framebuffer();

		const std::vector<Attachment>& GetColorAttachments() const		{ return m_colorAttachments; }
		const Attachment& GetDepthStencilAttachment()					{ return m_depthStencil; }

	private:
		friend class Device;

		/// Internal initialization function, since we lack C++11 delegating constructors -.-
		/// Note that m_depthStencil is already filled by the constructors initializer list.
		void Initialize(const std::vector<Attachment>& _colorAttachments, bool _depthWithStencil);

		unsigned int m_framebuffer;

		Attachment m_depthStencil;
		std::vector<Attachment> m_colorAttachments;
	};
}
