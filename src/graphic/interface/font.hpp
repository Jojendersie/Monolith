#pragma once

#include <string>
#include "utilities/assert.hpp"
#include <cstdint>
#include "graphic/core/vertexbuffer.hpp"
#include "graphic/core/effect.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/core/device.hpp"
#include <jofilelib.hpp>
#include "utilities/color.hpp"


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
		void SetText(const std::string& _text);

		const std::string& GetText() {return m_text;};

		/// \brief Sets the Position on the screen.
		void SetPos(Math::Vec2 _screenPos);
		const Math::Vec2& GetPos() { return m_screenPos; };

		/// \brief Sets the default size of the Textrender which gets used while no control char overrides it
		void SetDefaultSize(float _size);
		float GetDefaultSize() {return m_sizeD;};

		/// \brief returns the real dimensions the chars are drawn in
		/// \details chars are stretched to preserve the right view on the screen
		Math::Vec2 GetDim();

		/// \brief returns the size of the smallest rectangle in screen coords in wich the displayed text would fit in
		Math::Vec2 GetExpanse();

		/// \brief Scales the font size so that the text fills the area on the screen
		void SetExpanse(const Math::Vec2& _expanse, bool _onlyScaleDown = false);

		/// \brief returns the largest size the string contains(set per <s>)
		float GetMaxSize(){return m_sizeMax;};

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

		float m_sizeMax;

		int m_lineCount; ///< Amount of linebreaks m_text contains
		Math::Vec2 m_expanse; ///< size of the area the string posesses on the screen
		Math::Vec2 m_charSize; //< real x/y size a char is drawn in; x is not reliable since most fonts have differing widths
	};
};