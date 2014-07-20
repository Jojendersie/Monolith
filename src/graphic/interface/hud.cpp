#include "hud.hpp"
#include "../../input/input.hpp"
#include "../core/scissor.hpp"
#include "graphic/content.hpp"
#include "GLFW/glfw3.h"


namespace Graphic
{
	Hud::Hud(Monolith* _game, Math::Vec2 _pos, Math::Vec2 _size, bool _showCursor):
		ScreenOverlay(Math::Vec2(_pos[0],-_pos[1]), _size),
		m_game(_game),
		m_characters( "2222", VertexBuffer::PrimitiveType::POINT ),
		m_texContainer("texture/combined.png"),
		m_preElem(NULL),
		m_showCursor(_showCursor),
		m_scrollable(false)
	{
	//	for( int i = 0; i < MAX_SCREENTEX; i++ )
	//		m_screenTextures[i] = nullptr;

		m_dbgLabel = new TextRender(&Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT));//DEFAULT
		m_dbgLabel->SetPos(Math::Vec2(0.7f,0.8f));
		AddTextRender(m_dbgLabel);

		//m_globalPipelineData->texture2DEffect.BindTexture( "u_screenTex", 7, _globalPipelineData->linearSampler );

		//test screen tex
		Jo::Files::HDDFile file("texture/combined.sraw");
		m_texContainerMap = new Jo::Files::MetaFileWrapper( file, Jo::Files::Format::SRAW );
		
		m_cursor = new ScreenTexture(m_texContainerMap, "cursor", Math::Vec2(0.f,0.f), Math::Vec2(0.07f,0.07f), height);
	//	m_cursor->SetState(_showCursor);
		AddTexture(m_cursor);

	//	m_btnMenu = new Button(m_texContainerMap, m_globalPipelineData->defaultFont, "menuBtn", Math::Vec2(-0.9f,0.92f), Math::Vec2(0.16f,0.07f));
	//	m_btnMenu->m_caption.SetText("<c 000 024 242 255> <s 032>Menue</s> </c>");
	//	AddButton(m_btnMenu);
	}

	// ************************************************************************* //
	Hud::~Hud()
	{
		delete m_dbgLabel;

		delete m_texContainerMap;
		delete m_cursor;
		for( Button* btn : m_buttons )
			delete btn;
	}

	// ************************************************************************* //
	 void Hud::CreateBtn(std::string _texName, std::string _desc, Math::Vec2 _position, Math::Vec2 _size, RealDimension _rDim, 
		std::function<void()> _OnMouseUp, bool _autoX, bool _autoY, Font* _font)
	{
		Button* btn = new Button(m_texContainerMap, _texName, _position, _size, no, &Resources::GetFont(Fonts::GAME_FONT), _OnMouseUp);
		btn->SetAutoCenterX(_autoX);
		btn->SetAutoCenterY(_autoY);
		btn->SetCaption(_desc);
		AddButton(btn);
	}

	// ************************************************************************* //
	Hud* Hud::CreateContainer(Math::Vec2 _pos, Math::Vec2 _size) 
	{
		Hud* hud = new Hud(m_game, _pos, _size, false);
		m_containers.push_back(std::unique_ptr<Hud>(hud));
		AddScreenOverlay(hud);
		return hud;
	};

	// ************************************************************************* //
	void Hud::CreateModel(Math::Vec2 _pos , Math::Vec2 _size, Voxel::Model* _model)
	{
		ScreenModel* screenModel = new ScreenModel(_pos, _size, _model);
		AddScreenOverlay(screenModel);
		m_screenModels.push_back(std::unique_ptr<ScreenModel>(screenModel));
	};

	// ************************************************************************* //
	void Hud::Draw(double _deltaTime)
	{
		ScissorRect scissor(m_pos[0], -m_pos[1], m_size[0], m_size[1]);
		RenewBuffer();

		Device::SetEffect( Resources::GetEffect(Effects::TEXTURE_2DQUAD) );
		Device::SetTexture( m_texContainer, 7 );
		//ignore cursor
		Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices()-1 );

	/*	for(int i = 1; i < 256; i++)
			bla += (i%10 == 0)?(std::to_string(i)+":"+(char)i+"\n"):(std::to_string(i)+":"+(char)i+"  ");
		m_dbgLabel->SetText(bla);
		m_dbgLabel->SetPos(Math::Vec2(-0.9f,0.9f));*/
		for(int i = (int)m_textRenders.size(); i-- > 0; )
			if(m_textRenders[i]->m_active) m_textRenders[i]->Draw();

		//draw every subhud(container)
		for (int i = (int)m_screenOverlays.size(); i-- > 0;)
		{
			Hud* hud = dynamic_cast<Hud*> (m_screenOverlays[i]);
			if(hud != NULL && hud->GetState() && hud->GetVisibility())
				hud->Draw(_deltaTime);
		}

		//activate voxel rendering
		Graphic::Device::SetEffect(	Graphic::Resources::GetEffect(Graphic::Effects::VOXEL_RENDER) );
		//draw every screenModel
		for (int i = (int)m_screenModels.size(); i-- > 0;)
		{
			m_screenModels[i]->Draw(*m_camera);
		}

		//draw cursor last
		if(m_showCursor)
		{
			Device::SetEffect( Resources::GetEffect(Effects::TEXTURE_2DQUAD) );
			Device::SetTexture( m_texContainer, 7 );
			Device::DrawVertices( m_characters, m_characters.GetNumVertices()-1, 1 );
		}
	}

	// ************************************************************************* //
	void Hud::RenewBuffer()
	{
		m_characters.Clear();
	
		for( size_t i = m_screenOverlays.size(); i-- > 0; )
		{
			ScreenTexture* screenTex = dynamic_cast<ScreenTexture*> (m_screenOverlays[i]);
			if(screenTex != NULL && screenTex->GetState() && screenTex->GetVisibility()) 
				m_characters.Add(screenTex->m_vertex);
		}
	/*	for(ScreenTexture* screenTex : m_screenTextures)
		{
			if(screenTex->GetState() && screenTex->GetVisibility()) m_characters.Add(screenTex->m_vertex);
		}*/
		m_characters.SetDirty();
	}

	// ************************************************************************* //
	void Hud::MouseMove( double _dx, double _dy )
	{
		// Get cursor converted to screen coordinates
		Math::Vec2 cursorPos = Input::Manager::GetCursorPosScreenSpace();
		m_cursor->m_vertex.position = cursorPos;

		//todo: include mousespeed in config  

		//collision with hud elements 
		for(size_t i = m_screenOverlays.size(); i-- > 0; )
		{
			ScreenOverlay* screenOverlay = m_screenOverlays[i]; 
			Math::Vec2 loc2;
			loc2[0] = screenOverlay->m_pos[0] + screenOverlay->m_size[0];
			loc2[1] = screenOverlay->m_pos[1] - screenOverlay->m_size[1];
			if((screenOverlay->GetState())
			&&(screenOverlay->m_pos[0] < cursorPos[0]) && (screenOverlay->m_pos[1] > cursorPos[1])
			&& (loc2[0] > cursorPos[0]) && (loc2[1] < cursorPos[1]))
			{
				//enter new element; leave old
				if(m_preElem != screenOverlay) 
				{
					if(m_preElem != NULL){ m_preElem->MouseLeave(); m_preElem = nullptr;}
					screenOverlay->MouseEnter();	
					m_preElem = screenOverlay;
				}
				//pass mouseMove to the active object
				screenOverlay->MouseMove(_dx,_dy);
				return;
			}
		}
		//leave element; enter free space 
		MouseLeave();
	}

	// ************************************************************************* //
	bool Hud::KeyDown( int _key, int _modifiers, Math::Vec2 _pos )
	{
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preElem != nullptr)
		{
			m_preElem->KeyDown(_key, _modifiers);
			return true;
		}
		return false;
	}

	bool Hud::KeyUp( int _key, int _modifiers, Math::Vec2 _pos )
	{
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preElem != nullptr)
		{
			m_preElem->KeyUp(_key, _modifiers);
			return true;
		}
		return false;
	}
	
	bool Hud::Scroll(double _dx, double _dy)
	{
		//The focused overlay takes the input
		if(m_preElem && m_preElem->Scroll(_dx , _dy)) return true;
		//The Hud moves all elements up/down
		if(m_scrollable)
		{
			for(size_t i = m_screenOverlays.size(); i-- > 0; )
			{
				m_screenOverlays[i]->SetPos(Math::Vec2(m_screenOverlays[i]->m_pos[0]+(float)_dx * 0.1f, m_screenOverlays[i]->m_pos[1]+(float)_dy * 0.1f));
			}
			return true;
		}
		return false;
	}

	void Hud::MouseEnter()
	{
		ScreenOverlay::MouseEnter();
	}
	void Hud::MouseLeave()
	{
		if(m_preElem)
		{
			m_preElem->MouseLeave();
			m_preElem = nullptr;
		}
	}

	// ************************************************************************* //

	void Hud::AddScreenOverlay(ScreenOverlay* _screenOverlay)
	{
		_screenOverlay->SetSize(_screenOverlay->m_size * (m_size * 0.5f));
		//calculate the offset, add one so that -1 means no offset; mul with size because thats the relative space the overlay is in
		_screenOverlay->SetPos((_screenOverlay->m_pos + Math::Vec2(1.f,-1.f)) * m_size * 0.5f + m_pos);// + Math::Vec2(1.f, -1.f)
		m_screenOverlays.push_back(_screenOverlay);
	}

	void Hud::AddTextRender(TextRender* _label)
	{
		m_textRenders.push_back(_label);
	}

	void Hud::AddTexture(ScreenTexture* _tex)
	{
		//adjustments for to display the texture right in the window
		_tex->m_vertex.size[0] -= 1.5f/m_texContainer.Width();
	//	_tex->m_vertex.size[1] -= 1.f/m_texContainer.Height();
		_tex->m_vertex.texCoord[0] += 0.5f/m_texContainer.Width();
	//	_tex->m_vertex.texCoord[1] += 0.5f/m_texContainer.Height();
		switch (_tex->m_realDimension)
		{
		case width:
			_tex->SetSize(Math::Vec2(_tex->m_sizeDef[0], _tex->m_sizeDef[1] / Device::GetAspectRatio()));
			break;
		case height:
			_tex->SetSize(Math::Vec2(_tex->m_sizeDef[0] / Device::GetAspectRatio(), _tex->m_sizeDef[1] ));
			break;
		default:
			break;
		}  
//		_tex->m_vertex.screenSize[0] /= Device::GetAspectRatio();
		AddScreenOverlay(_tex);
	}

	void Hud::AddButton(Button* _btn)
	{
		
		//keep pointer to delete it later
		m_buttons.push_back(_btn);
		AddTexture(&(_btn->m_btnDefault));
		AddTexture(&(_btn->m_btnOver));
		AddTexture(&(_btn->m_btnDown));
		AddTexture(_btn);//add the collision btn last so it is in front of the elements
		AddTextRender(&(_btn->m_caption));
	}

};