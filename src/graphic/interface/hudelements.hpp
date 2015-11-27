#include "font.hpp"
#include "game.hpp"
#include "graphic/content.hpp"
#include <functional>
#include <memory>

namespace Graphic {

	struct TextureVertex
	{
		ei::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
		ei::Vec2 screenSize;
		ei::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
		ei::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
	};

	/// \brief Defines how an element should be scaled to fit the current screen ratio
	/// \details The choosen dimension is displayed as defined, while the other gets scaled to preserve the size ratio
	enum class RealDimension
	{
		width,
		height,
		none
	};

	/// \brief a basic class for 2d screen elements
	/// \details a storage for size information and capable of interaction with the mouse and keys when managed by a hud
	/// _position is the upper left corner of the Overlay in screenspace [-1,1]x[-1,1] starting from the lower left corner
	/// _size is the size in screen space
	/// both are relative to the owning parent(Hud), thus a size of 2.f will make that the Overlay has the size of the Hud 
	class ScreenOverlay
	{
	public:
		ScreenOverlay(ei::Vec2 _pos, ei::Vec2 _size, std::function<void()> _OnMouseUp = [] () {return;}):
			m_pos(_pos), m_size(_size), m_active(true), m_visible(true), OnMouseUp(_OnMouseUp){};

		/// \ Functions to alter the rectangle in lifetime
		///Derivates override to bring in custom behaviour.
		virtual void SetPos(ei::Vec2 _pos) {m_pos = _pos;};
		virtual void SetSize(ei::Vec2 _size){m_size = _size;};

		/// \ Functions to acsess the vision and activity states
		void SetState(bool _state) {m_active = _state;};
		void SetVisibility(bool _visibility){m_visible = _visibility;};

		bool GetState(){return m_active;};
		bool GetVisibility(){return m_visible;};

		//Only the overlay in front recieves this events

		virtual void MouseMove( double _dx, double _dy ){return;};
		/// \brief Triggered when the mouse entered the tex this frame
		virtual void MouseEnter(){if(OnMouseEnter != NULL) OnMouseEnter();};
		/// \brief Triggered when the mouse leaved the tex this frame
		virtual	void MouseLeave(){if(OnMouseLeave != NULL) OnMouseLeave();};
		/// \brief Called when left mouse buttons goes down inside the rectangle; @param _pos Pos of Mouse relative to the button
		///returns true when input gets captured
		virtual bool KeyDown( int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)){if(OnMouseDown != NULL) OnMouseDown(); return true;};
		/// \brief Called when left mouse buttons goes up inside the rectangle; @param _pos Pos of Mouse relative to the button
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)){if(OnMouseUp != NULL) OnMouseUp();return true;};
		
		/// \brief Only triggered for the overlay the mouse is in; Does nothing by default 
		virtual bool Scroll(double _dx, double _dy){return false;};

		//direct acsess should only be used for fast reading
		ei::Vec2 m_pos;///< position in screen coordsystem
		ei::Vec2 m_size;///< size in screen coordsystem

	protected:
		bool m_active; ///< when false: gets ignored by everything
		bool m_visible; ///< visibility

		//events
		std::function<void()> OnMouseEnter;
		std::function<void()> OnMouseLeave;
		std::function<void()> OnMouseDown;
		std::function<void()> OnMouseUp;

	private:
	};

	/// \brief A 2d screen overlay texture 
	/// \details To preserve the ratio defined in size one can define _rDim so that the other dimension is scaled to fit the needs
	class ScreenTexture : public ScreenOverlay
	{
	public:
		ScreenTexture( Jo::Files::MetaFileWrapper* _posMap, const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f,0.f), RealDimension _rDim = RealDimension::none,
			std::function<void()> _OnMouseUp = [] () {return;});
		
		TextureVertex m_vertex;

		//override to apply vertex changes 
		virtual void SetPos(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;


		RealDimension m_realDimension;

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
		Button(Jo::Files::MetaFileWrapper* _posMap, std::string _name, ei::Vec2 _position, ei::Vec2 _size, 
			RealDimension _rDim = RealDimension::none, Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT),
			std::function<void()> _OnMouseUp = [] () {return;} );

		ScreenTexture m_btnDefault;
		ScreenTexture m_btnOver;
		ScreenTexture m_btnDown;
		TextRender m_caption;

		void SetCaption(const std::string& _caption);

		virtual void SetPos(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;

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

	/// \brief An model rendered as overlay on the camera level
	class ScreenModel : public ScreenOverlay
	{
		/// \brief Takes an model which gets rendered in the center specified screen rect
		/// \details param [in] the center of the model is in the center of the rectangle
		/// _model The ScreenModel takes the ownership of the model
	public:
		ScreenModel( ei::Vec2 _position, ei::Vec2 _size, Voxel::Model* _model, float _scale );
		~ScreenModel();
		virtual void SetPos(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		
		/// \brief Positions and draws the model 
		void Draw(const Input::Camera& _cam);
	private:
		/// \brief Centers the modell in the current rectangle
		void Center();

		ei::Vec2 m_center;
		float m_scale;

		Voxel::Model* m_model;
	};

	/// \brief A panel wich allows text input
	class EditField : public ScreenTexture
	{
	public:
		/// \brief Create a useable EditField on the screen
		/// \details param [in] informations needed for texture and Textrender
		/// _lines The amount of lines the field has; 0 means automatic
		/// _fontSize used fontsize for the text; 0 means automatic
		EditField(Jo::Files::MetaFileWrapper* _posMap, Font* _font, ei::Vec2 _position, ei::Vec2 _size, int _lines = 1, float _fontSize = 1);

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