#include "hud.hpp"
#include "..\input\input.hpp"

namespace Graphic
{
	Hud::Hud(Content* _globalPipelineData, Monolith* _game ):
		m_globalPipelineData(_globalPipelineData),
		m_game(_game),
		m_characters( "2222", VertexBuffer::PrimitiveType::POINT ),
		m_container("texture/combined.png"),
		m_screenTexCount(0),
		m_TextRenderCount(0),
		m_preTex(NULL)
//		m_mapFile("texture/combined.sraw", true),
//		m_containerMap( m_mapFile, Jo::Files::Format::SRAW )
//		m_labelMS(&TextRender(m_defaultFont))
	{
		m_dbgLabel = new TextRender(m_globalPipelineData->defaultFont);
		m_dbgLabel->SetPos(Math::Vec2(0.7f,0.8f));
		AddTextRender(m_dbgLabel);

		m_globalPipelineData->texture2DEffect.BindTexture( "u_screenTex", 7, _globalPipelineData->linearSampler );

		//test screen tex
		Jo::Files::HDDFile file("texture/combined.sraw");
		m_containerMap = new Jo::Files::MetaFileWrapper( file, Jo::Files::Format::SRAW );
		
		m_cursor = new ScreenTexture(m_containerMap, "cursor", Math::Vec2(0.f,0.f), Math::Vec2(0.07f,0.07f));

		AddTexture(m_cursor);

		m_btnMenu = new Button(m_containerMap, m_globalPipelineData->defaultFont, "button", Math::Vec2(-1.f,0.92f), Math::Vec2(0.14f,0.07f));
		m_btnMenu->m_caption.SetText("<c 000 024 242 255> <s 032>Menue</s> </c>");
		AddButton(m_btnMenu);
	}

	Hud::~Hud()
	{
		delete m_dbgLabel;

		delete m_containerMap;
		delete m_cursor;
		delete m_btnMenu;
	}

	void Hud::Draw(double _time, double _deltaTime)
	{

		RenewBuffer();

		Device::SetEffect( m_globalPipelineData->texture2DEffect );
		Device::SetTexture( m_container, 7 );
		Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices() );

		for(int i = 0; i < m_TextRenderCount; i++ )
			if(m_TextRenders[i]->m_active) m_TextRenders[i]->Draw();
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

	void Hud::AddTextRender(TextRender* _label)
	{
		m_TextRenders[m_TextRenderCount] = _label;
		m_TextRenderCount++;
	}

	void Hud::AddTexture(ScreenTexture* _tex)
	{
		m_screenTextures[m_screenTexCount] = _tex;
		m_screenTexCount++;
	}

	void Hud::AddButton(Button* _btn)
	{
		AddTexture(&(_btn->m_btnDefault));
		AddTexture(&(_btn->m_btnOver));
		AddTexture(&(_btn->m_btnDown));
		AddTexture(_btn);//add the collision btn last so it is in front of the elements
		AddTextRender(&(_btn->m_caption));
	}

};