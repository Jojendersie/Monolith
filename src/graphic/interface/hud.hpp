#pragma once

//#include "hudelements.hpp"
#include "messagebox.hpp"

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud : public ScreenOverlay
	{
	public:
		/// \brief Creates an Hud object which handles a 2d interface on the specified screen rectangle
		/// \param [in] _componentRenderer Reference to a renderer to show single component previews.
		///		The HUD class does not take the ownership.
		Hud( Monolith* _game, Graphic::SingleComponentRenderer* _componentRenderer, ei::Vec2 _pos=ei::Vec2(-1.f,-1.f) , ei::Vec2 _size=ei::Vec2(2.f,2.f), int _cursor = 1, bool _showDbg = true);

		template< typename _T, typename... _Args, typename = std::enable_if< std::is_base_of<ScreenOverlay, _T>::value >::type >//only ScreenOverlays should be made with this
		_T& CreateScreenElement(_Args&&... _args)
		{
			auto ptr = new _T(std::forward<_Args>(_args)...);

			m_screenElements.emplace_back(ptr);
			ptr->Register(*this);

			return *ptr;
		}
		/// \brief Creates an container in the current Hud and returns it as Hud* to fill it with elements 
		Hud* CreateContainer(ei::Vec2 _pos=ei::Vec2(-1.f,-1.f) , ei::Vec2 _size=ei::Vec2(2.f,2.f));

		/// \brief Creates an screenModel
		/// ScreenModels require a camera to be set first per SetCamera()
		void CreateComponent(Voxel::ComponentType _type, ei::Vec2 _pos, float _scale);

		TextRender& CreateLabel(const ei::Vec2& _pos, const std::string& _text, float _scale = 1.f, Font& _font = Resources::GetFont(Graphic::Fonts::DEFAULT));

		MessageBox& CreateMessageBox(const ei::Vec2& _pos, const ei::Vec2& _size);

		/// \brief Last call in every frame draw call
		void Draw(double _deltaTime);

		/// \brief Adds an ScreenOverlay to the management
		void AddScreenOverlay(ScreenOverlay* _screenOverlay);

		/// \brief Adds an existing label(TextRender) to the auto draw management and takes ownership
		/// \details Size gets relativated to the hud it is member of
		void AddTextRender(TextRender* _label);

		/// \brief Adds an existing screenTex to the auto draw and collision management
		void AddTexture(ScreenTexture* _tex);

		/// \brief Adds an existing button to the auto draw management and takes ownership
		void AddButton(Button* _btn);

		/// \brief Sets the camera; Currently only used by ScreenModel
		void SetCamera(Input::Camera* _cam) {m_camera = _cam;};

		/// \brief When scrollable all elements of the hud will move when a scrollevent is recieved
		void SetScrollable(bool _scrollable) {m_scrollable = _scrollable;};

		void ShowCursor(int _cursor);
		int CursorVisible() { return m_showCursor; };

		/// \brief Mouse events
		///	At least one is important so that dynamic_cast can work
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown( int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)) override;

		/// \brief called by the current game state to update mouse input
		virtual void MouseMove( double _dx, double _dy )override;

		virtual bool Scroll(double _dx, double _dy) override;

		Jo::Files::MetaFileWrapper* GetTexContainerMap() { return m_texContainerMap; }

		~Hud();

		TextRender* m_dbgLabel;

	private:

		ScreenOverlay* m_focus;///< object which currently takes the input

		/// \brief rebuilds the vertex buffer for screen textures
		void RenewBuffer(); 

		// pointers to global information
		Monolith* m_game;
		Input::Camera* m_camera;
		Graphic::SingleComponentRenderer* m_componentRenderer;

		VertexArrayBuffer m_characters;/// < vertex buffer that holds the screen textures

		Texture m_texContainer; ///< The basic texture container for screen elements; loads "combined.png" 
		// is created dynamically in constructor since it temporarily needs a hdd file handle
		Jo::Files::MetaFileWrapper* m_texContainerMap; ///< the size and position informations of the elements in the container

		// dynamic lists to hold and manage HUD elements
		// all elements of m_containers and m_screenModels are as well in m_screenOverlays

		//ownership
		std::vector<std::unique_ptr< ScreenOverlay >> m_screenElements;
		std::vector<std::unique_ptr < ScreenComponent > > m_screenComponents;
		std::vector<std::unique_ptr < TextRender > > m_labels; ///< The container of TextRenders that are not part of another element
		//no ownership
		std::vector< MessageBox* > m_messageBoxes;
		std::vector<TextRender*> m_textRenders;
		std::vector<ScreenOverlay*> m_screenOverlays;

		ScreenOverlay* m_preElem;///< Handle to the screenTexture wich the cursor points to 

		struct CursorData
		{
			CursorData(std::string _name,
				ei::Vec2 _size = ei::Vec2(0.07f, 0.07f), ei::Vec2 _off = ei::Vec2(0.f));
			ScreenTexture texture;
			ei::Vec2 offset;
		};

		std::vector < CursorData > m_cursors; ///< all available cursors
		CursorData* m_cursor;///< the ingame cursor
		int m_showCursor;

		bool m_scrollable; ///< wether mouse scrolling moves the elements
		bool m_showDbg; ///< Should the debug label be displayed
	};
};
