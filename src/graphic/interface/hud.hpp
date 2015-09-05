//#include "hudelements.hpp"
#include "messagebox.hpp"

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud : public ScreenOverlay
	{
	public:
		/// \brief Creates an Hud object wich handles a 2d interface on the specfied screen rectangle
		Hud( Monolith* _game, Math::Vec2 _pos=Math::Vec2(-1.f,-1.f) , Math::Vec2 _size=Math::Vec2(2.f,2.f), int _cursor = 1, bool _showDbg = true);

		// functions intended do be used in gamestates to create a button with the specefied params
		void CreateBtn(std::string _texName, std::string _desc, Math::Vec2 _position, Math::Vec2 _size,
			RealDimension _rDim = RealDimension::none, std::function<void()> _OnMouseUp = []() {return; },
			bool _autoX = true, bool _autoY = true,
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));

		/// \brief Creates an container in the current Hud and returns it as Hud* to fill it with elements 
		Hud* CreateContainer(Math::Vec2 _pos=Math::Vec2(-1.f,-1.f) , Math::Vec2 _size=Math::Vec2(2.f,2.f));

		/// \brief Creates an screenModel
		/// ScreenModels requiere a camera to be set first per SetCamera()
		void CreateModel(Math::Vec2 _pos , Math::Vec2 _size, Voxel::Model* _model, float _scale);

		/// \brief Creates an EditField
		/// \details returns a reference to the field
		/// which is valid until the hud is destroyed
		EditField& CreateEditField(Math::Vec2 _pos, Math::Vec2 _size, int _lines = 1, float _fontSize = 1);

		ScreenTexture& CreateScreenTexture(const Math::Vec2& _pos, const Math::Vec2& _size, const std::string& _name, RealDimension _rDim = RealDimension::none);

		TextRender& CreateLabel(const Math::Vec2& _pos, const std::string& _text, float _scale = 1.f, Font& _font = Resources::GetFont(Graphic::Fonts::DEFAULT));

		MessageBox& CreateMessageBox(const Math::Vec2& _pos, const Math::Vec2& _size);

		/// \brief Last call in every frame drawcall
		void Draw(double _deltaTime);

		/// \brief Adds an ScreenOverlay to the managegement
		void AddScreenOverlay(ScreenOverlay* _screenOverlay);

		/// \brief Adds an existing label(TextRender) to the auto draw managment and takes ownership
		/// \details Size gets relativated to the hud it is member of
		void AddTextRender(TextRender* _label);

		/// \brief Adds an existing screenTex to the auto draw and collision managment
		void AddTexture(ScreenTexture* _tex);

		/// \brief Adds an existing button to the auto draw managment and takes ownership
		void AddButton(Button* _btn);

		/// \brief Sets the camera; Currently only used by ScreenModel
		void SetCamera(Input::Camera* _cam) {m_camera = _cam;};

		/// \brief When scrollable all elements of the hud will move when a scrollevent is recieved
		void SetScrollable(bool _scrollable) {m_scrollable = _scrollable;};

		void ShowCursor(int _cursor);
		int CursorVisible() { return m_showCursor; };

		/// \brief Mouse events
		//atleast one is important so that dynamic_cast can work
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown( int _key, int _modifiers, Math::Vec2 _pos = Math::Vec2(0.f,0.f)) override;
		virtual bool KeyUp(int _key, int _modifiers, Math::Vec2 _pos = Math::Vec2(0.f,0.f)) override;

		/// \brief called by the current gamestate to update mouseinput
		virtual void MouseMove( double _dx, double _dy )override;

		virtual bool Scroll(double _dx, double _dy) override;

		~Hud();

		TextRender* m_dbgLabel;

	private:

		ScreenOverlay* m_focus;///< object wich currently takes the input

		/// \brief rebuilds the vertex buffer for screen textures
		void RenewBuffer(); 

		// pointers to global information
		Monolith* m_game;

		Input::Camera* m_camera;

		VertexBuffer m_characters;/// < vertex buffer that holds the screen textures

		Texture m_texContainer; ///< The basic texture container for screen elements; loads "combined.png" 
		//gets created dynamicly in constructor since it temporarly needs a hdd file handle
		Jo::Files::MetaFileWrapper* m_texContainerMap; ///< the size and position informations of the elements in the container

		//dynamic lists to hold and manage HUD elements
		//all elements of m_containers and m_screenModels are aswell in m_screenOverlays

		//ownership
		std::vector<Button*> m_buttons;
		std::vector<std::unique_ptr <EditField> > m_editFields;
		std::vector<std::unique_ptr <Hud> > m_containers;
		std::vector<std::unique_ptr <ScreenModel> > m_screenModels;
		std::vector<std::unique_ptr <ScreenTexture > > m_screenTextures;
		std::vector<std::unique_ptr < TextRender > > m_labels; ///< The container of TextRenders that are not part of another element
		std::vector<std::unique_ptr < MessageBox > > m_messageBoxes;
		//no ownership
		std::vector<TextRender*> m_textRenders;
		std::vector<ScreenOverlay*> m_screenOverlays;

		ScreenOverlay* m_preElem;///< Handle to the screenTexture wich the cursor points to 

		struct CursorData
		{
			CursorData(Jo::Files::MetaFileWrapper* _posMap, std::string _name,
				Math::Vec2 _size = Math::Vec2(0.07f, 0.07f), Math::Vec2 _off = Math::Vec2(0.f));
			ScreenTexture texture;
			Math::Vec2 offset;
		};

		std::vector < CursorData > m_cursors; ///< all available cursors
		CursorData* m_cursor;///< the ingame cursor
		int m_showCursor;

		bool m_scrollable; ///< wether mouse scrolling moves the elements
		bool m_showDbg; ///< Should the debug label be displayed
	};
};
