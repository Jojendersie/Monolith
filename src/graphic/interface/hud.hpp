#include "hudelements.hpp"


namespace Graphic {

	const int MAX_SCREENTEX = 64;
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud : public ScreenOverlay
	{
	public:
		/// \brief Creates an Hud object wich handles a 2d interface on the specfied screen rectangle
		Hud( Monolith* _game, Math::Vec2 _pos=Math::Vec2(-1.f,-1.f) , Math::Vec2 _size=Math::Vec2(2.f,2.f), bool _showCursor = true);

		// functions intended do be used in gamestates to create a button with the specefied params
		void CreateBtn(std::string _texName, std::string _desc, Math::Vec2 _position, Math::Vec2 _size,
			RealDimension _rDim = no, std::function<void()> _OnMouseUp = [] () {return;},
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));

		/// \brief Creates an container in the current Hud and returns it as Hud* to fill it with elements 
		Hud* CreateContainer(Math::Vec2 _pos=Math::Vec2(-1.f,-1.f) , Math::Vec2 _size=Math::Vec2(2.f,2.f));

		/// \brief Creates an screenModel
		/// ScreenModels requier a camera to be set first per SetCamera()
		void CreateModel(Math::Vec2 _pos , Math::Vec2 _size, Voxel::Model* _model);

		/// \brief Last call in every frame drawcall
		void Draw(double _deltaTime);

		/// \brief Adds an ScreenOverlay to the managegement and takes ownership
		void AddScreenOverlay(ScreenOverlay* _screenOverlay);

		/// \brief Adds an existing label(TextRender) to the auto draw managment and takes ownership
		/// \details Size gets relativated to the hud it is member of
		void AddTextRender(TextRender* _label);

		/// \brief Adds an existing screenTex to the auto draw and collision managment and takes ownership
		void AddTexture(ScreenTexture* _tex);

		/// \brief Adds an existing button to the auto draw managment and takes ownership
		void AddButton(Button* _btn);

		/// \brief Sets the camera; Currently only used by ScreenModel
		void SetCamera(Input::Camera* _cam) {m_camera = _cam;};

		/// \brief When scrollable all elements of the hud will move when a scrollevent is recieved
		void SetScrollable(bool _scrollable) {m_scrollable = _scrollable;};

		/// \brief Mouse events
		//atleast one is important so that dynamic_cast can work
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown( int _key, int _modifiers, Math::Vec2 _pos = Math::Vec2(0.f,0.f)) override;
		virtual bool KeyUp(int _key, int _modifiers, Math::Vec2 _pos = Math::Vec2(0.f,0.f)) override;

		/// \ called by the current gamestate to update mouseinput
		virtual void MouseMove( double _dx, double _dy )override;

		virtual bool Scroll(double _dx, double _dy) override;

		~Hud();

		TextRender* m_dbgLabel;

	private:

		ScreenTexture* m_focus;///< object wich currently takes the input

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
		//todo use std::vector + RAII - Mememorymanagment for better performance and readability
		//all elements of m_containers and m_screenModels are aswell in m_screenOverlays

		std::vector<TextRender*> m_textRenders;

		std::vector<ScreenOverlay*> m_screenOverlays;

		std::vector<Button*> m_buttons;

		std::vector<std::unique_ptr <Hud> > m_containers;

		std::vector<std::unique_ptr <ScreenModel> > m_screenModels;

		ScreenOverlay* m_preElem;///< Handle to the screenTexture wich the cursor points to 
		ScreenTexture* m_cursor;///< the ingame cursor
		bool m_showCursor;

		bool m_scrollable; ///< wether mouse scrolling moves the elements
	};
};