#include "hudelements.hpp"


namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class Hud
	{
	public:
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

		Hud( Content* _globalPipelineData, Monolith* _game );
		~Hud();

		TextRender* m_dbgLabel;

	private:
		void RenewBuffer();

		Font* m_defaultFont;
		Monolith* m_game;
		Content* m_globalPipelineData;
		VertexBuffer m_characters;

		int m_TextRenderCount;
		TextRender* m_TextRenders[64];

		//screen texture stuff

		Texture m_container;
		Jo::Files::MetaFileWrapper* m_containerMap;
		int m_screenTexCount; 
		ScreenTexture* m_screenTextures[64];

		ScreenTexture* m_preTex;
		ScreenTexture* m_cursor;

		//todo easy use of textures, textrenderer, buttons
		Button* m_btnMenu;
	};
};