#pragma once

#include "font.hpp"
#include "game.hpp"
#include "screenoverlay.hpp"
#include "graphic/content.hpp"

#include <memory>

namespace Graphic {

	struct TextureVertex
	{
		ei::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
		ei::Vec2 screenSize;
		ei::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
		ei::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
	};

	/// \brief A 2d screen overlay texture 
	/// \details To preserve the ratio defined in size one can define _rDim so that the other dimension is scaled to fit the needs
	class ScreenTexture : public ScreenOverlay
	{
	public:
		ScreenTexture( const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			std::function<void()> _OnMouseUp = [] () {return;});
		
		TextureVertex m_vertex;

		void Register(Hud& _hud) override;
		//override to apply vertex changes 
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;

		ei::Vec2 m_posDef;///< size as defined
		ei::Vec2 m_sizeDef;///< size as defined
	protected:
	private:
	};

	/// \brief A pressable button with mouseover indication
	class Button : public ScreenTexture
	{
	public:
		/// \brief creates a button
		Button(const std::string& _name, ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), 
			DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(), const std::string& _caption = "",
			std::function<void()> _OnMouseUp = [] () {return;}, 
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));

		ScreenTexture m_btnDefault;
		ScreenTexture m_btnOver;
		ScreenTexture m_btnDown;
		TextRender m_caption;

		void SetCaption(const std::string& _caption);

		void Register(Hud& _hud) override;
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;

	//	void SetMouseOverColor(Utils::Color8U _color)

		void SetAutoCenterX(bool _autoCenter) {m_autoCenter[0] = _autoCenter;};
		void SetAutoCenterY(bool _autoCenter) {m_autoCenter[1] = _autoCenter;};

		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

	private:
		ei::Vec<bool,2> m_autoCenter;
		int m_btnState; // 0 - default; 1 - mouseover; 2 - down
	};

	/// \brief A component rendered as overlay on the camera level
	class ScreenComponent : public ScreenOverlay
	{
		/// \brief Takes an model which gets rendered in the center specified screen rect
		/// \details param [in] _position the center of the model is in the center of the
		///		rectangle _component.
	public:
		ScreenComponent( Voxel::ComponentType _component, ei::Vec2 _position, float _scale, int _sideFlags, Anchor _anchor = Anchor() );
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		
		/// \brief Positions and draws the model 
		void Draw(Graphic::SingleComponentRenderer* _renderer, const Input::Camera& _cam);
	private:
		/// \brief Centers the model in the current rectangle
		void Center();

		ei::Vec2 m_center;
		float m_scale;
		Voxel::ComponentType m_componentType;
		int m_sideFlags;
	};

	/// \brief A panel which allows text input
	class EditField : public ScreenTexture
	{
	public:
		/// \brief Create a useable EditField on the screen
		/// \details param [in] informations needed for texture and Textrender
		/// _lines The amount of lines the field has; 0 means automatic
		/// _fontSize used fontsize for the text; 0 means automatic
		EditField(ei::Vec2 _position, ei::Vec2 _size, Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT),
			int _lines = 1, float _fontSize = 1);

		void Register(Hud& _hud) override;

		/// \brief Returns the current text the field contains.
		const std::string& GetText() { return m_content; };

		/// \brief Returns the textRender to diretly manipulate the text.
		TextRender* getTextRender() { return &m_textRender; };
	private:

		/// \brief Adds an line after the specified one when it does not violate m_linesMax 
		void AddLine(int _preLine);

		int m_linesMax;
		Font* m_font;
		float m_fontSize; ///< all lines have the same size
		int m_cursor; /// < index of the cursor char in m_content

		std::string m_content;
		TextRender m_textRender; /// < text is stored in a single textRender
		//std::vector< std::unique_ptr< TextRender > > m_lines; 

		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

	};

};