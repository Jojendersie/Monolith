#pragma once

#include "core/uniformbuffer.hpp"
#include "core/effect.hpp"
#include "core/samplerstate.hpp"
#include "../predeclarations.hpp"
#include "interface/font.hpp"

namespace Graphic {

	/// \brief Global content like fonts and render effects
	struct Content {
		Content();
		~Content();

		Graphic::Effect voxelRenderEffect;
		Graphic::Effect texture2DEffect;
		Graphic::Effect wireEffect;
		Graphic::Effect beamEffect;
		Graphic::UniformBuffer objectUBO;
		Graphic::UniformBuffer cameraUBO;
		Graphic::UniformBuffer globalUBO;
		Graphic::SamplerState pointSampler;
		Graphic::SamplerState linearSampler;

		Graphic::Font* defaultFont; ///< Font used if nothing else is specified		
		Graphic::Font* gameFont;	///< Font used for all menus...
	};

} // namespace Graphic