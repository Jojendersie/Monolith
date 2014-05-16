#pragma once

namespace Graphic {

	class Texture;
	class Framebuffer;
	class Effect;

	/// \brief PostProcessing effects for Monolith.
	class PostProcessing
	{
	public:
		enum class AmbientOcclusionConfig
		{
			OFF,
			HIGH_QUALITY // Todo: Lower qualities
		};

		PostProcessing(AmbientOcclusionConfig _aoConfig);
		~PostProcessing();

		/// \brief Resets the current configuration and recreates buffers and shaders if necessary.
		///
		/// For sake of simplicity and safety some of the performed operations may be optional.
		/// However even without this function can not perform very fast.
		void Reconfigure(AmbientOcclusionConfig _aoConfig);

		/// \brief Performs postprocessing using the given textures and outputs the result to the (hardware-)backbuffer.
		void PerformPostProcessing(const Texture& _sceneTexture, const Texture& _depthTexture);

		/// \brief Recreates all screen size dependent textures.
		void OnScreenResized();

	private:
		/// (Re-)Loads all shader with the current configuration.
		void ReLoadShader();

		Effect* m_screenOutputEffect;

		AmbientOcclusionConfig m_ambientOcclusionConfig;

		Effect* m_ambientOcclusionEffect;
		Effect* m_ambientOcclusionBlurXEffect;
		Effect* m_ambientOcclusionBlurYEffect;

		// Textures for ambient occlution.
		Framebuffer* m_ambientOcclusionFB[2];
		Texture* m_ambientOcclusion[2];
	};

} // namespace Graphic