#include "font.hpp"
#include "../game.hpp"
#include <functional>

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

		virtual void MouseEnter();
		virtual	void MouseLeave();
		virtual void MouseDown();
		virtual void MouseUp();

		void SetState(bool _state) {m_active = _state;};
		void SetVisibility(bool _visibility){m_visible = _visibility;};

		bool GetState(){return m_active;};
		bool GetVisibility(){return m_visible;};
	private:
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
		void MouseDown();
		void MouseUp();

	private:
		int m_btnState; // 0 - default; 1 - mouseover; 2 - down
	};

};