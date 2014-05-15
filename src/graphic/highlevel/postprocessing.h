#pragma once

namespace Graphic {

	class Texture;
	class Effect;

	/// \brief PostProcessing effects for Monolith.
	class PostProcessing
	{
	public:
		PostProcessing();
		~PostProcessing();

		/// \brief Performs postprocessing using the given textures and outputs the result to the (hardware-)backbuffer.
		void PerformPostProcessing(const Texture& sceneTexture, const Texture& depthTexture);
		
	private:
		Effect* m_screenOutput;
	};

} // namespace Graphic