#pragma once

#include "uniformbuffer.hpp"
#include "effect.hpp"
#include "samplerstate.hpp"
#include "../predeclarations.hpp"
#include "font.hpp"

namespace Graphic {

	/// \brief Global content like fonts and render effects
	struct Content {
		Content();
		~Content();

		Graphic::Effect voxelRenderEffect;
		Graphic::Effect texture2DEffect;
		Graphic::UniformBuffer objectUBO;
		Graphic::UniformBuffer cameraUBO;
		Graphic::UniformBuffer globalUBO;
		Graphic::SamplerState pointSampler;
		Graphic::SamplerState linearSampler;

		Graphic::Font* defaultFont;		///< Font used for all menus...
	};

} // namespace Graphic