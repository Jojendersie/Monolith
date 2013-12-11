#pragma once

#include <string>
#include <cassert>
// TODO: own 32bit color class
#include <cstdint>
#include "vertexbuffer.hpp"
#include "effect.hpp"
#include "texture.hpp"
#include "device.hpp"
#include "content.hpp"
#include <jofilelib.hpp>


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
		Math::Vec2 m_sizeTable[256];
		Math::Vec2 m_coordTable[256];
		Effect m_effect;
		Texture m_texture;
		/// \brief Load a bitmap font and initialize the render effect.
		Font(std::string _fontName, Content* _globalPipelineData);
	};

	/// \brief A buffer with a formated string to draw.
	class TextRender
	{
	public:
		/// \brief Create a text of a specific font.
		TextRender( Font* _font );

		/// \brief Draw all buffered texts.
		void Draw();

		/// \brief Sets the text and updates the VertexBuffer.
		void SetText(std::string _text);

		// \brief Sets the Position on the screen.
		void SetPos(Math::Vec2 _screenPos);
	private:
		void RenewBuffer();
		Font* m_font;	///< Reference to the used font.
		std::string m_text;
		Math::Vec2 m_screenPos;
		VertexBuffer m_characters;
	};
};