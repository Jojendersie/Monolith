#pragma once

#include "core/uniformbuffer.hpp"
#include "core/effect.hpp"
#include "core/samplerstate.hpp"
#include "../predeclarations.hpp"
#include "interface/font.hpp"

namespace Graphic {

	/// \brief Preloaded effects.
	enum struct Effects
	{
		VOXEL_RENDER = 0,	///< Draw lots of cubes. Bound UBOs: {OBJECT_VOXEL, CAMERA, GLOBAL}
		TEXTURE_2DQUAD = 1,	///< Draw a screen space quad. Bound UBOs: {}
		WIRE = 2,			///< Draw a set of lines. Bound UBOs: {OBJECT_WIRE}
		BEAM = 3,			///< Draw beams for a set of lines Bound: {OBJECT_WIRE}
		COUNT				///< Number of effects - this must be the last enumeration member
	};

	/// \brief Preloaded effects.
	enum struct UniformBuffers
	{
		GLOBAL = 0,
		CAMERA = 1,
		OBJECT_VOXEL = 2,
		OBJECT_WIRE = 3,
		COUNT			///< Number of effects - this must be the last enumeration member
	};

	/// \brief Predefined effects.
	enum struct Fonts
	{
		DEFAULT = 0,	///< Font used if nothing else is specified
		GAME_FONT = 1,	///< Font used for all menus...
		COUNT			///< Number of effects - this must be the last enumeration member
	};

	/// \brief Predefined sampler states
	enum struct SamplerStates
	{
		POINT = 0,
		LINEAR = 1,
		LINEAR_NOMIPMAP = 2, ///< Performs no filter on mipmaps. Useful for lineraly sampling textures without mipmaps.
		COUNT			///< Number of effects - this must be the last enumeration member
	};

	/// \brief Global content like fonts and render effects
	/// \details This is like a content manager for specific data. It does not
	///		load arbitrary files but offers state objects, effects and fonts
	///		for rendering.
	class Resources
	{
	public:
		/// \brief Get one of the predefined effects.
		/// \details The first time this is called for an effect it is loaded.
		static Graphic::Effect* GetEffect(Effects _effect);

		/// \brief Get one of the predefined uniform buffers.
		/// \details The first time this is called for a buffer it is created.
		static Graphic::UniformBuffer* GetUBO(UniformBuffers _ubo);

		/// \brief Get one of the predefined fonts.
		/// \details The first time this is called the font is loaded.
		static Graphic::Font* GetFont(Fonts _font);

		/// \brief Get one of the predefined sampler states.
		/// \details The first time this is called the state is created.
		static Graphic::SamplerState* GetSamplerState(SamplerStates _state);

		/// \brief Delete all the loaded resources
		static void Unload();
	private:

		static Graphic::Effect* s_effects[Effects::COUNT];
		static Graphic::UniformBuffer* s_ubos[UniformBuffers::COUNT];
		static Graphic::Font* s_fonts[Fonts::COUNT];
		static Graphic::SamplerState* s_samplers[SamplerStates::COUNT];
	};

} // namespace Graphic