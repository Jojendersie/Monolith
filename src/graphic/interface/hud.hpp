#pragma once

//#include "hudelements.hpp"
#include "messagebox.hpp"

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud : public ScreenOverlay
	{
	public:
		enum struct CursorType
		{
			None,
			Pointer,
			Crosshair
		};

		/// \brief Creates an Hud object which handles a 2d interface on the specified screen rectangle
		/// \param [in] _componentRenderer Reference to a renderer to show single component previews.
		///		The HUD class does not take the ownership.
		Hud( Monolith* _game, Graphic::SingleComponentRenderer* _componentRenderer,
			ei::Vec2 _pos=ei::Vec2(-1.f,-1.f) , ei::Vec2 _size=ei::Vec2(2.f,2.f),
			const std::string& _texture = "defaultcontainer.png", CursorType _cursor = CursorType::Pointer);

		/// \brief Creates an 2d element that is registered and managed by this hud.
		/// \details For parameters required check the corresponding constructors.
		template< typename _T, typename... _Args, typename = std::enable_if< std::is_base_of<ScreenPosition, _T>::value >::type >//only ScreenPositions should be made with this
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

		MessageBox& CreateMessageBox(const ei::Vec2& _pos, const ei::Vec2& _size);

		/// \brief Last call in every frame draw call
		void Draw(double _deltaTime);


		// Always just call the most specialized RegisterElement!
		/// \brief Adds an ScreenOverlay to the input focus management
		void RegisterElement(ScreenOverlay& _screenOverlay);

		/// \brief Adds an existing TextRender to the auto draw management.
		/// \details Size gets adjusted relative to the hud it is member of.
		void RegisterElement(TextRender& _textRender);

		/// \brief Adds an existing screenTexture to the auto draw and collision management.
		void RegisterElement(ScreenTexture& _screenTexture);

		/// \brief Sets the camera; Currently only used by ScreenModel
		void SetCamera(Input::Camera* _cam) {m_camera = _cam;};

		/// \brief When scrollable all elements of the hud will move when a scroll-event is received
		void SetScrollable(bool _scrollable) {m_scrollable = _scrollable;};

		void ShowCursor(CursorType _cursor);
		CursorType GetCurrentCursor() const { return m_showCursor; }
		bool IsCursorVisible() const { return m_showCursor == CursorType::None; };

		TextRender& GetDebugLabel() { return *m_dbgLabel; }

		/// \brief Mouse events
		///	At least one is important so that dynamic_cast can work
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown( int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f,0.f)) override;

		/// \brief called by the current game state to update mouse input
		virtual void MouseMove( double _dx, double _dy )override;

		virtual bool Scroll(double _dx, double _dy) override;

		const Jo::Files::MetaFileWrapper* GetTexContainerMap() const { return m_texContainerMap; }

		~Hud();
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
		std::vector<std::unique_ptr< ScreenPosition >> m_screenElements;
		std::vector<std::unique_ptr < ScreenComponent > > m_screenComponents;
		//no ownership
		std::vector< MessageBox* > m_messageBoxes;
		std::vector<TextRender*> m_textRenders;
		std::vector<ScreenOverlay*> m_screenOverlays;
		std::vector<Hud*> m_subHuds;
		std::vector<ScreenTexture*> m_screenTextures;

		ScreenOverlay* m_preElem;///< Handle to the screenTexture which the cursor points to 

		struct CursorData
		{
			CursorData(const std::string& _name,
				ei::Vec2 _size = ei::Vec2(0.07f, 0.07f), ei::Vec2 _off = ei::Vec2(0.f));

			ScreenTexture texture;
			ei::Vec2 offset;
		};

		std::vector < CursorData > m_cursors; ///< all available cursors
		CursorData* m_cursor;///< the ingame cursor
		CursorType m_showCursor;

		bool m_scrollable; ///< wether mouse scrolling moves the elements
		bool m_showDbg; ///< Should the debug label be displayed
		std::unique_ptr<TextRender> m_dbgLabel;
	};
};
