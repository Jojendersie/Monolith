#pragma once

#include <string>
#include <cassert>
// TODO: own 32bit color class
#include <cstdint>
#include "vertexbuffer.hpp"
#include "effect.hpp"


namespace Graphic {

	/// \brief Information required by shader to find and format the
	///		character tile.
	struct CharacterVertex
	{
		Math::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
		Math::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
		Math::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
		uint32_t color;
		float thickness;		///< A number in [0,1] to modify character thickness.
		float scale;			///< Something like the font size.
	};

	/// \brief A class to load and render a bitmap font.
	class Font
	{
	public:
		/// \brief Load a bitmap font and initialize the render effect.
		Font();
	private:
		Effect m_effect;
	};

	/// \brief A buffer with a formated string to draw.
	class TextRender
	{
	public:
		/// \brief Create a text of a specific font.
		TextRender( Font* _font );

		/// \brief Draw all buffered texts.
		void Draw();
	private:
		Font* m_font;	///< Reference to the used font.
		VertexBuffer m_characters;
	};
};