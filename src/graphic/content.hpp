#pragma once

#include "uniformbuffer.hpp"
#include "effect.hpp"
#include "samplerstate.hpp"
#include "../predeclarations.hpp"

namespace Graphic {

	/// \brief Global content like fonts and render effects
	struct Content {
		Content();
		~Content();

		Graphic::Effect voxelRenderEffect;
		Graphic::Effect texture2DEffect;
		Graphic::UniformBuffer objectUBO;
		Graphic::UniformBuffer cameraUBO;
		Graphic::SamplerState pointSampler;
		Graphic::SamplerState linearSampler;

		Graphic::Texture* voxelTextures;
	};

} // namespace Graphic