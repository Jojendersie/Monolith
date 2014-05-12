#include "hud.hpp"
#include "input/input.hpp"
#include "graphic/content.hpp"
#include "../../../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"

namespace Graphic
{
	Hud::Hud(Monolith* _game ):
		m_game(_game),
		m_characters( "2222", VertexBuffer::PrimitiveType::POINT ),
		m_container("texture/combined.png"),
		m_screenTexCount(0),
		m_textRenderCount(0),
		m_btnCount(0),
		m_preTex(NULL)
//		m_mapFile("texture/combined.sraw", true),
//		m_containerMap( m_mapFile, Jo::Files::Format::SRAW )
//		m_labelMS(&TextRender(m_defaultFont))
	{
	//	for( int i = 0; i < MAX_SCREENTEX; i++ )
	//		m_screenTextures[i] = nullptr;

		m_dbgLabel = new TextRender(Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT));
		m_dbgLabel->SetPos(Math::Vec2(0.6f,0.8f));
		AddTextRender(m_dbgLabel);

		//test screen tex
		Jo::Files::HDDFile file("texture/combined.sraw");
		m_containerMap = new Jo::Files::MetaFileWrapper( file, Jo::Files::Format::SRAW );
		
		m_cursor = new ScreenTexture(m_containerMap, "cursor", Math::Vec2(0.f,0.f), Math::Vec2(0.07f,0.07f));

		AddTexture(m_cursor);

	//	m_btnMenu = new Button(m_containerMap, m_globalPipelineData->defaultFont, "menuBtn", Math::Vec2(-0.9f,0.92f), Math::Vec2(0.16f,0.07f));
	//	m_btnMenu->m_caption.SetText("<c 000 024 242 255> <s 032>Menue</s> </c>");
	//	AddButton(m_btnMenu);
	}

	Hud::~Hud()
	{
		delete m_dbgLabel;

		delete m_containerMap;
		delete m_cursor;
		for( int i = 0; i < m_btnCount; i++ )
			delete m_buttons[i];
	}

	void Hud::CreateBtn(std::string _texName, std::string _desc, Math::Vec2 _position, Math::Vec2 _size, std::function<void()> _OnMouseUp)
	{
		Button* btn = new Button(m_containerMap, Resources::GetFont(Fonts::GAME_FONT), _texName, _position, _size, _OnMouseUp);
		btn->m_caption.SetText(_desc);
		AddButton(btn);
	}

	void Hud::Draw(double _time, double _deltaTime)
	{

		RenewBuffer();

		Device::SetEffect( *Resources::GetEffect(Effects::TEXTURE_2DQUAD) );
		Device::SetTexture( m_container, 7 );
		//ignore cursor
		Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices()-1 );

		for(int i = 0; i < m_textRenderCount; i++ )
			if(m_textRenders[i]->m_active) m_textRenders[i]->Draw();

		//draw cursor last
		Device::SetEffect( *Resources::GetEffect(Effects::TEXTURE_2DQUAD) );
		Device::SetTexture( m_container, 7 );
		Device::DrawVertices( m_characters, m_characters.GetNumVertices()-1, 1 );
	}

	void Hud::RenewBuffer()
	{
		m_characters.Clear();
		for(int i = m_screenTexCount-1; i >= 0; i--)
		{
			if(m_screenTextures[i]->GetState() && m_screenTextures[i]->GetVisibility()) m_characters.Add(m_screenTextures[i]->m_vertex);
		}
		m_characters.Commit();
	}

	void Hud::MouseMove( double _dx, double _dy )
	{
		// Get cursor converted to screen coordinates
		Math::Vec2 cursorPos = Input::Manager::GetCursorPosScreenSpace();
		m_cursor->m_vertex.position = cursorPos;

		//todo: include mousespeed in config  

		//collision with hud objects todo: rewrite with better form/structure
		for(int i = m_screenTexCount-1; i >= 0; i--)
		{
			Math::Vec2 loc2;
			loc2[0] = m_screenTextures[i]->m_vertex.position[0] + m_screenTextures[i]->m_vertex.screenSize[0];
			loc2[1] = m_screenTextures[i]->m_vertex.position[1] - m_screenTextures[i]->m_vertex.screenSize[1];
			if((m_screenTextures[i]->GetState())
			&&(m_screenTextures[i]->m_vertex.position[0] < cursorPos[0]) && (m_screenTextures[i]->m_vertex.position[1] > cursorPos[1])
			&& (loc2[0] > cursorPos[0]) && (loc2[1] < cursorPos[1]))
			{
				//enter new tex; leave old
				if(m_preTex != m_screenTextures[i]) 
				{
					if(m_preTex != NULL){ m_preTex->MouseLeave(); m_preTex = nullptr;}
					m_screenTextures[i]->MouseEnter();	
					m_preTex = m_screenTextures[i];
				}
				return;
			}
		}
		//leave tex; enter free space 
		if(m_preTex != NULL)
		{
			m_preTex->MouseLeave();
			m_preTex = nullptr;
		}
	}

	bool Hud::KeyDown( int _key, int _modifiers )
	{
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preTex != nullptr)
		{
			m_preTex->MouseDown();
			return true;
		}
		return false;
	}

	bool Hud::KeyUp( int _key, int _modifiers )
	{
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preTex != nullptr)
		{
			m_preTex->MouseUp();
			return true;
		}
		return false;
	}

	bool Hud::Scroll(double _dx, double _dy)
	{
		if(m_focus != nullptr) ;
		return false;
	}

	void Hud::AddTextRender(TextRender* _label)
	{
		m_textRenders[m_textRenderCount] = _label;
		m_textRenderCount++;
	}

	void Hud::AddTexture(ScreenTexture* _tex)
	{
		//adjustments for to display the texture right in the window
		_tex->m_vertex.size[0] -= 1.5f/m_container.Width();//good
	//	_tex->m_vertex.size[1] -= 1.f/m_container.Height();
		_tex->m_vertex.texCoord[0] += 0.5f/m_container.Width();//good
	//	_tex->m_vertex.texCoord[1] += 0.5f/m_container.Height(); 
		_tex->m_vertex.screenSize[0] /= Device::GetAspectRatio();
		m_screenTextures[m_screenTexCount] = _tex;
		m_screenTexCount++;
	}

	void Hud::AddButton(Button* _btn)
	{
		//keep pointer to delete it later
		m_buttons[m_btnCount] = _btn;
		m_btnCount++;
		AddTexture(&(_btn->m_btnDefault));
		AddTexture(&(_btn->m_btnOver));
		AddTexture(&(_btn->m_btnDown));
		AddTexture(_btn);//add the collision btn last so it is in front of the elements
		AddTextRender(&(_btn->m_caption));
	}

};