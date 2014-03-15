#include "font.hpp"
#include "../../game.hpp"
#include <functional>
#include <memory>

namespace Graphic {

	struct TextureVertex
	{
		Math::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
		Math::Vec2 screenSize;
		Math::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
		Math::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
	};

	/// \brief A 2d screen overlay texture 
	class ScreenTexture
	{
	public:
		ScreenTexture( Jo::Files::MetaFileWrapper* _posMap, std::string _name,
			Math::Vec2 _position, Math::Vec2 _position2 = Math::Vec2(0.f,0.f), std::function<void()> _OnMouseUp = [] () {return;});
		TextureVertex m_vertex;

		//Only the tex in front recieves this events

		/// \brief Triggered when the mouse entered the tex this frame
		virtual void MouseEnter();
		/// \brief Triggered when the mouse leaved the tex this frame
		virtual	void MouseLeave();
		/// \brief Called when left mouse buttons goes down inside the tex; @param _pos Pos of Mouse relative to the button
		virtual void MouseDown(Math::Vec2 _pos = Math::Vec2(0.f,0.f));
		/// \brief Called when left mouse buttons goes up inside the tex; @param _pos Pos of Mouse relative to the button
		virtual void MouseUp(Math::Vec2 _pos = Math::Vec2(0.f,0.f));

		void SetState(bool _state) {m_active = _state;};
		void SetVisibility(bool _visibility){m_visible = _visibility;};

		bool GetState(){return m_active;};
		bool GetVisibility(){return m_visible;};
	protected:
		bool m_active; ///< false: tex gets ignored by everything
		bool m_visible; 
		//events
		std::function<void()> OnMouseEnter;
		std::function<void()> OnMouseLeave;
		std::function<void()> OnMouseDown;
		std::function<void()> OnMouseUp;
	};

	/// \brief A 2d screen overlay texture 
	class Button : public ScreenTexture
	{
	public:
		/// \brief creates a button
		Button(Jo::Files::MetaFileWrapper* _posMap, Font* _font, std::string _name, Math::Vec2 _position, Math::Vec2 _size, 
			std::function<void()> _OnMouseUp = [] () {return;} );

		ScreenTexture m_btnDefault;
		ScreenTexture m_btnOver;
		ScreenTexture m_btnDown;
		TextRender m_caption;

		void MouseEnter();
		void MouseLeave();
		void MouseDown(Math::Vec2 _pos);
		void MouseUp(Math::Vec2 _pos);

	private:
		int m_btnState; // 0 - default; 1 - mouseover; 2 - down
	};
	/// \brief A field wich allows text input
	class Editfield : public ScreenTexture
	{
	public:
		/// \brief Create a useable EditField on the screen
		/// \details param [in] informations needed for tex and Textrender
		/// _lines The amount of lines the field has; 0 means automatic
		Editfield(Jo::Files::MetaFileWrapper* _posMap, Font* _font, Math::Vec2 _position, Math::Vec2 _size, int _lines = 1, float _fontSize = 1);

	private:

		/// \brief Adds an line after the specified one when it does not violate m_linesMax 
		void AddLine(int _preLine);

		int m_linesMax;
		Font* m_font;
		float m_fontSize; ///< all lines have the same size
		int m_cursor[2]; /// < [0] - char;  [1] - line
		std::vector< std::unique_ptr< TextRender > > m_lines; 

		void MouseDown(Math::Vec2 _pos);
		void MouseUp(Math::Vec2 _pos);

	};

};