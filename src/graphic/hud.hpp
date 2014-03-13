#include "hudelements.hpp"


namespace Graphic {

	const int MAX_SCREENTEX = 64;
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud
	{
	public:
		// functions intendet do be used in gamestates
		void CreateBtn(std::string _texName, std::string _desc, Math::Vec2 _position, Math::Vec2 _size, std::function<void()> _OnMouseUp = [] () {return;});

		/// \brief Last call in every frame drawcall
		void Draw(double _time, double _deltaTime);

		/// \ Adds an existing label(TextRender) to the auto draw managment
		void AddTextRender(TextRender* _label);

		/// \ Adds an existing screenTex to the auto draw managment
		void AddTexture(ScreenTexture* _tex);

		/// \ Adds an existing button to the auto draw managment
		void AddButton(Button* _btn);

		/// \ called by the current gamestate to update mouseinput
		void MouseMove( double _dx, double _dy );

		/// \ Mouseclicks gets passed to the Hudelements
		bool KeyDown( int _key, int _modifiers );
		bool KeyUp( int _key, int _modifiers );

		Hud( Content* _globalPipelineData, Monolith* _game );
		~Hud();

		TextRender* m_dbgLabel;

	private:
		void RenewBuffer();

		Monolith* m_game;
		Content* m_globalPipelineData;
		VertexBuffer m_characters;

		int m_textRenderCount;
		TextRender* m_textRenders[64];

		//screen texture stuff

		Texture m_container;
		Jo::Files::MetaFileWrapper* m_containerMap;
		int m_screenTexCount; 
		ScreenTexture* m_screenTextures[MAX_SCREENTEX];

		int m_btnCount; 
		Button* m_buttons[MAX_SCREENTEX/4];

		ScreenTexture* m_preTex;
		ScreenTexture* m_cursor;

		Button* m_btnMenu;
	};
};