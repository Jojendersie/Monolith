#include "hud.hpp"
#include "../../input/input.hpp"
#include "../core/scissor.hpp"
#include "graphic/content.hpp"
#include "GLFW/glfw3.h"


using namespace ei;

namespace Graphic
{
	Hud::Hud(Monolith* _game, Graphic::SingleComponentRenderer* _componentRenderer, Vec2 _pos, Vec2 _size, int _cursor, bool _showDbg) :
		ScreenOverlay(ei::Vec2(_pos[0],_pos[1] + _size[1]), _size),
		m_game(_game),
		m_componentRenderer(_componentRenderer),
		m_characters( VertexArrayBuffer::PrimitiveType::POINT, {{VertexAttribute::VEC2, 11}, {VertexAttribute::VEC2, 12}, {VertexAttribute::VEC2, 13}, {VertexAttribute::VEC2, 14}} ),
		m_texContainer("texture/combined.png"),
		m_preElem(NULL),
		m_showCursor(_cursor),
		m_scrollable(false),
		m_focus(nullptr),
		m_showDbg(_showDbg)
	{
	//	for( int i = 0; i < MAX_SCREENTEX; i++ )
	//		m_screenTextures[i] = nullptr;
		if (m_showDbg)
		{
			m_dbgLabel = new TextRender(&Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT));//DEFAULT
			m_dbgLabel->SetPosition(Vec2(0.5f, 0.8f));
			AddTextRender(m_dbgLabel);
		}

		m_texContainerMap = &Graphic::Resources::GetTextureMap();//new Jo::Files::MetaFileWrapper( file, Jo::Files::Format::SRAW );
		
		m_cursors.emplace_back("cursor");
		m_cursors.emplace_back("cursorAlt", Vec2(0.07f, 0.07f), Vec2(-0.035f, 0.035f));

		//cursor occupies adress [0]
		m_screenOverlays.push_back(&m_cursors[0].texture);

		ShowCursor(_cursor);
		//make sure that the cursor occupies address [0]
	}

	// ************************************************************************* //
	Hud::~Hud()
	{
		delete m_dbgLabel;
	}

	// ************************************************************************* //
	 void Hud::CreateBtn(std::string _texName, std::string _desc, Vec2 _position, Vec2 _size, 
		std::function<void()> _OnMouseUp, bool _autoX, bool _autoY, Font* _font)
	{
/*		 Button* btn = new Button(_texName, _position, _size, 
			 DefinitionPoint::TopLeft, Anchor(),
			 &Resources::GetFont(Fonts::GAME_FONT), _OnMouseUp);
		btn->SetAutoCenterX(_autoX);
		btn->SetAutoCenterY(_autoY);
		btn->SetCaption(_desc);
		AddButton(btn);*/
	}

	// ************************************************************************* //
	Hud* Hud::CreateContainer(Vec2 _pos, Vec2 _size) 
	{
		Hud* hud = new Hud(m_game, m_componentRenderer, _pos, _size, false, false);
		m_screenElements.emplace_back(hud);
		AddScreenOverlay(hud);
		return hud;
	};

	// ************************************************************************* //
	void Hud::CreateComponent(Voxel::ComponentType _type, ei::Vec2 _pos, float _scale)
	{
		m_screenComponents.push_back( std::make_unique< ScreenComponent >(_type, _pos, _scale, 0x3b, Anchor(TopLeft, this)) );
		AddScreenOverlay( m_screenComponents.back().get() );
	};

	// ************************************************************************* //
	EditField& Hud::CreateEditField(Vec2 _pos, Vec2 _size, int _lines, float _fontSize)
	{
		EditField* editField = new EditField(&Resources::GetFont(Fonts::GAME_FONT), _pos, _size, _lines, _fontSize);

		m_screenElements.emplace_back(editField);
		AddTexture(editField);
		AddTextRender(editField->getTextRender());

		return *editField;
	}

	// ************************************************************************* //
	ScreenTexture& Hud::CreateScreenTexture(const Vec2& _pos, const Vec2& _size, const std::string& _name)
	{
		auto screenTex = new ScreenTexture(_name, _pos, _size);
		m_screenElements.emplace_back(screenTex);
		screenTex->Register(*this);

		return *screenTex;
	}

	// ************************************************************************* //
	TextRender& Hud::CreateLabel(const Vec2& _pos, const std::string& _text, float _scale, Font& _font)
	{
		TextRender* label = new TextRender(&_font);

		m_labels.emplace_back(label);
		
		label->SetDefaultSize(_scale);
		label->SetText(_text);
		label->SetPosition(_pos);

		//after the position is set
		AddTextRender(label);

		return *label;
	}
	// ************************************************************************* //
	MessageBox& Hud::CreateMessageBox(const Vec2& _pos, const Vec2& _size)
	{
		MessageBox* msgBox = new MessageBox(_pos, _size);

		m_messageBoxes.push_back(msgBox);
		m_screenElements.emplace_back(msgBox);
		AddTexture((ScreenTexture*)msgBox);
		AddTextRender(&msgBox->m_textRender);

		return *msgBox;
	}

	// ************************************************************************* //
	void Hud::Draw(double _deltaTime)
	{
		for (auto& msgBox : m_messageBoxes)
			msgBox->Process((float)_deltaTime);

		ScissorRect scissor(m_position[0], m_position[1]-m_size[1], m_size[0], m_size[1]);
		RenewBuffer();

		Device::SetEffect( Resources::GetEffect(Effects::TEXTURE_2DQUAD) );
		Device::SetTexture( m_texContainer, 7 );
		//ignore cursor
		Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices()-1 );

		for(int i = (int)m_textRenders.size() - 1; i >= 0; --i)
			if(m_textRenders[i]->m_active) m_textRenders[i]->Draw();

		//draw every subhud(container)
		for (int i = (int)m_screenOverlays.size(); i-- > 0;)
		{
			Hud* hud = dynamic_cast<Hud*> (m_screenOverlays[i]);
			if(hud != NULL && hud->GetState() && hud->GetVisibility())
				hud->Draw(_deltaTime);
		}

		if(m_componentRenderer && m_screenComponents.size() > 0)
		{
			// activate voxel rendering
			Graphic::Device::SetEffect(	Graphic::Resources::GetEffect(Graphic::Effects::VOXEL_RENDER) );
			// draw every screenComponent
			for (int i = (int)m_screenComponents.size(); i-- > 0;)
			{
				m_screenComponents[i]->Draw(m_componentRenderer, *m_camera);
			}
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
		auto vbGuard = m_characters.GetBuffer(0);
	
	//	for( size_t i = m_screenOverlays.size(); i-- > 0; )
		for (size_t i = 1; i < m_screenOverlays.size(); i++)
		{
			ScreenTexture* screenTex = dynamic_cast<ScreenTexture*> (m_screenOverlays[i]);
			if(screenTex != NULL && screenTex->GetState() && screenTex->GetVisibility()) 
				vbGuard->Add(screenTex->m_vertex);
		}
		//cursor
		ScreenTexture* screenTex = dynamic_cast<ScreenTexture*> (m_screenOverlays[0]);
		if (screenTex != NULL && screenTex->GetState() && screenTex->GetVisibility())
			vbGuard->Add(screenTex->m_vertex);
	/*	for(auto& screenTex : m_screenTextures)
		{
			if(screenTex->GetState() && screenTex->GetVisibility()) m_characters.Add(screenTex->m_vertex);
		}*/
	}

	// ************************************************************************* //
	void Hud::MouseMove( double _dx, double _dy )
	{
		// Get cursor converted to screen coordinates
		Vec2 cursorPos = Input::Manager::GetCursorPosScreenSpace();
		if (m_cursor)	m_cursor->texture.m_vertex.position = cursorPos + m_cursor->offset;

		//todo: include mousespeed in config  

		//collision with hud elements
		for(size_t i = m_screenOverlays.size(); i-- > 0; )
		{
			ScreenOverlay* screenOverlay = m_screenOverlays[i]; 
			Vec2 loc2;
			loc2[0] = screenOverlay->m_position[0] + screenOverlay->m_size[0];
			loc2[1] = screenOverlay->m_position[1] - screenOverlay->m_size[1];

			if((screenOverlay->GetState())
			&&(screenOverlay->m_position[0] < cursorPos[0]) && (screenOverlay->m_position[1] > cursorPos[1])
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
	bool Hud::KeyDown( int _key, int _modifiers, Vec2 _pos )
	{
		//clicking on a screenOverlay
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preElem != nullptr)
		{
			return m_preElem->KeyDown(_key, _modifiers);
		}

		//focused object recieves input
		if (m_focus)
		{
			return m_focus->KeyDown(_key, _modifiers);
		}

		return false;
	}

	bool Hud::KeyUp( int _key, int _modifiers, Vec2 _pos )
	{
		if(_key == GLFW_MOUSE_BUTTON_LEFT && m_preElem != nullptr)
		{
			m_focus = dynamic_cast<EditField*>(m_preElem);
			return	m_preElem->KeyUp(_key, _modifiers);
		}

		//focused object recieves input
		if (m_focus)
		{
			return m_focus->KeyUp(_key, _modifiers);
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
				//only top level elements are moved directly
				if (m_screenOverlays[i]->GetAnchor().parent != this) continue;
				m_screenOverlays[i]->SetPosition(Vec2(m_screenOverlays[i]->m_positionDef[0] - (float)_dx * 0.1f, m_screenOverlays[i]->m_positionDef[1] - (float)_dy * 0.1f));
			}
			//temporary implementation to call updateParams() and refresh the vertex information
			for (auto el : m_screenOverlays) el->SetPosition(el->m_positionDef);
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
//		_screenOverlay->SetSize(_screenOverlay->m_size * (m_size * 0.5f));
		//calculate the offset, add one so that -1 means no offset; mul with size because thats the relative space the overlay is in
//		_screenOverlay->SetPosition((_screenOverlay->m_position + Vec2(1.f,-1.f)) * m_size * 0.5f + m_position);// + Math::Vec2(1.f, -1.f)
		m_screenOverlays.push_back(_screenOverlay);
	}

	void Hud::AddTextRender(TextRender* _label)
	{
		_label->SetPosition((_label->GetPosition() + Vec2(1.f, -1.f)) * m_size * 0.5f + m_position);
		m_textRenders.push_back(_label);
	}

	void Hud::AddTexture(ScreenTexture* _tex)
	{
		//adjustments to display the texture right in the window
	//	_tex->m_vertex.size[0] += 0.5f/m_texContainer.Width();
		_tex->m_vertex.size[1] -= 1.5f/m_texContainer.Height();
	//	_tex->m_vertex.texCoord[0] += 0.5f/m_texContainer.Width();
	//	_tex->m_vertex.texCoord[1] -= 0.5f/m_texContainer.Height();
//		_tex->m_vertex.screenSize[0] /= Device::GetAspectRatio();
		AddScreenOverlay(_tex);
	}

	void Hud::AddButton(Button* _btn)
	{
		
		//keep pointer to delete it later
		m_screenElements.emplace_back(_btn);

		AddTextRender(&(_btn->m_caption));

		AddTexture(&(_btn->m_btnDefault));
		AddTexture(&(_btn->m_btnOver));
		AddTexture(&(_btn->m_btnDown));
		AddTexture(_btn);//add the collision btn last so it is in front of the elements
	}

	void Hud::ShowCursor(int _cursor)
	{ 
		m_showCursor = _cursor;
		if (_cursor)
		{
			m_cursor = &m_cursors[_cursor - 1];

			//cursor position is only updated on mousemove
			m_cursor->texture.m_vertex.position = Input::Manager::GetCursorPosScreenSpace() + m_cursor->offset;

			m_screenOverlays[0] = &m_cursor->texture;
		}
	}

	Hud::CursorData::CursorData(std::string _name,
		Vec2 _size, Vec2 _off)
		: texture(_name, Vec2(0.f)),
		offset(_off)
	{
		texture.Scale(Vec2(0.5f));
		//rescaling to fit the screen
	//	texture.SetSize(Vec2(texture.m_sizeDef[0] / Device::GetAspectRatio(), texture.m_sizeDef[1]));
	//	offset[0] /= Device::GetAspectRatio();
	};

};