#pragma once

#include <string>
#include <cassert>
#include <cstdint>
#include "../core/vertexbuffer.hpp"
#include "../core/effect.hpp"
#include "../core/texture.hpp"
#include "../core/device.hpp"
#include <jofilelib.hpp>
#include "../../utilities/color.hpp"


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
		Font(std::string _fontName);
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
		/// \details chars are one byte, ansi code 
		/// control chars and formating : 
		/// 13 : line break
		/// "<s sss>" size "sized text" "</s>"
		/// "<c rrr ggg bbb aaa>" "colored text" "</c>"
		/// "<i>" "italic text" "</i" not yet implemented
		/// "<t  ttt>" "thickness text" "</b>"
		/*performance view of alternative syntax: 4x{ 2x cmp 3x {add; mul}} + all xx iterations need cmp, inc, jmp
										 current: direct acsess possible*/
		void SetText(const std::string& _text);

		// \brief Sets the Position on the screen.
		void SetPos(Math::Vec2 _screenPos);

		Math::Vec2 GetDim() {return Math::Vec2(m_font->m_sizeTable[0][0],m_font->m_sizeTable[0][1]*m_font->m_texture.Height() / m_font->m_texture.Width());};

		bool m_active;

	private:
		void RenewBuffer();
		/// \brief resolves control chars and aplies them
		/// \param [in] _start index of '<' in m_text
		int CntrlChr(int _i);
		Font* m_font;	///< Reference to the used font.
		std::string m_text;
		Math::Vec2 m_screenPos;
		VertexBuffer m_characters;
		Utils::Color8U m_color, m_colorD;
		float m_size, m_sizeD;
		float m_thickness, m_thicknessD;
	};
};