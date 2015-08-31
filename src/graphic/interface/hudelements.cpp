#include "hudelements.hpp"
#include "..\..\math\ray.hpp"
#include "..\..\input\camera.hpp"
#include "../../voxel/model.hpp"
#include "GLFW/glfw3.h"

using namespace Math;

namespace Graphic
{
	ScreenTexture::ScreenTexture(Jo::Files::MetaFileWrapper* _posMap, std::string _name, Math::Vec2 _position, Math::Vec2 _size,
		RealDimension _rDim, std::function<void()> _OnMouseUp):
		ScreenOverlay(_position, _size, _OnMouseUp),
		m_realDimension(_rDim), //sizes are calculated when added to a hud
		m_posDef(_position),
		m_sizeDef(_size)
	{
		m_vertex.position = _position;
		m_vertex.texCoord = Math::Vec2(_posMap->RootNode[_name][std::string("positionX")],_posMap->RootNode[_name][std::string("positionY")]);
		m_vertex.size = Math::Vec2(_posMap->RootNode[_name][std::string("sizeX")],_posMap->RootNode[_name][std::string("sizeY")]);
		m_vertex.screenSize = Math::Vec2(0.f,0.f) == _size ? m_vertex.size : _size ;
	}

	void ScreenTexture::SetPos(Math::Vec2 _pos)
	{
		ScreenOverlay::SetPos(_pos);
		m_vertex.position = _pos;// + Math::Vec2(0.f, m_vertex.screenSize[1]/2.f);
	}

	// ************************************************************************ //

	void ScreenTexture::SetSize(Math::Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_vertex.screenSize = _size;
	}

	// ************************************************************************ //

	Button::Button(Jo::Files::MetaFileWrapper* _posMap, std::string _name, Math::Vec2 _position, Math::Vec2 _size,
		 RealDimension _rDim, Font* _font, std::function<void()> _OnMouseUp ):
		ScreenTexture(_posMap, _name+"Default", _position, _size, _rDim, _OnMouseUp),
		m_btnDefault(_posMap, _name+"Default", _position, _size, _rDim),
		m_btnOver(_posMap, _name+"Over", _position, _size, _rDim),
		m_btnDown(_posMap, _name+"Down", _position, _size, _rDim),
		m_caption(_font),
		m_autoCenter(true)
	{
		SetVisibility(false); 
		SetState(true);
		
		m_caption.SetText(_name);
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
	}

	// ************************************************************************ //
	void Button::SetPos(Math::Vec2 _pos)
	{
		ScreenOverlay::SetPos(_pos);
/*		m_btnDefault.SetPos(_pos);
		m_btnOver.SetPos(_pos);
		m_btnDown.SetPos(_pos);
		m_caption.SetPos(_pos);*/
		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetSize(Math::Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_btnDefault.SetSize(_size);
		m_btnOver.SetSize(_size);
		m_btnDown.SetSize(_size);
	}

	// ************************************************************************ //
	void Button::SetCaption(const std::string& _caption)
	{
		int len = (int)_caption.length();
		int lineCount = 1;
		int charCount = 0;
		int charCountMax = 0;
		for(int i = 0; i < len; i++)
		{
			charCount++;
			if(_caption[i] == '\n')
			{
				if(charCountMax < charCount)
					charCountMax = charCount;
				charCount = 0;
				lineCount++;
			}
		}

		//if the text contains no linebreaks
		if(!charCountMax) charCountMax = charCount;

		Math::Vec2 captionDim = m_caption.GetDim();

		// in case that the text is to large in any direction scale it down
		if(captionDim[0] * charCountMax * m_caption.GetDefaultSize() >= m_size[0] || captionDim[1] * lineCount >= m_size[1])
		{
			m_caption.SetDefaultSize(Math::min( (float)(m_size[0] / (captionDim[0] * charCountMax)),
												(float)(m_size[1] / (captionDim[1] * lineCount))) );
		}

		//set the (new) caption first to rebuild the buffer
		m_caption.SetText(_caption);

		// center in both directions
		m_caption.SetPos(m_pos+Math::Vec2(!m_autoCenter[0] ? 0.f : ((m_size[0] - captionDim[0] * charCountMax * m_caption.GetDefaultSize()) / 2.f),
							!m_autoCenter[1] ? 0.f : (- m_size[1] / (float)lineCount * 0.5f	- captionDim[1] * m_caption.GetMaxSize() * 0.45f )));//0.5f

	}

	void Button::MouseEnter()
	{
		ScreenOverlay::MouseEnter();
		m_btnDefault.SetVisibility(false);
		m_btnOver.SetVisibility(true);
	}
	void Button::MouseLeave()
	{
		ScreenOverlay::MouseLeave();
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
	}
	bool Button::KeyDown(int _key, int _modifiers, Math::Vec2 _pos)
	{
		ScreenOverlay::KeyDown(_key, _modifiers, _pos);
		m_btnDefault.SetVisibility(false);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(true);
		//since an object(this) is hit, the return value is always true
		return true;
	}

	bool Button::KeyUp(int _key, int _modifiers, Math::Vec2 _pos)
	{
		ScreenOverlay::KeyUp(_key, _modifiers, _pos);
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
		return true;
	}
	
	
	// ************************************************************** //

	ScreenModel::ScreenModel(Math::Vec2 _position, Math::Vec2 _size, Voxel::Model* _model, float _scale ):
		ScreenOverlay(_position, _size),
		m_model(_model),
		m_scale(_scale)
	{
		Center();
	}

	ScreenModel::~ScreenModel()
	{
		delete m_model;
	}

	void ScreenModel::SetPos(Math::Vec2 _pos)
	{
		ScreenOverlay::SetPos(_pos);
		Center();
	}

	// ************************************************************************ //

	void ScreenModel::SetSize(Math::Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		Center();
	}

	void ScreenModel::Center()
	{
		m_center[0] = m_pos[0] + m_size[0] * 0.5f;
		m_center[1] = m_pos[1] + m_size[1] * 0.5f;
	}
	
	void ScreenModel::Draw(const Input::Camera& _cam)
	{
		Math::WorldRay ray = _cam.GetRay(m_center);
	//	Math::FixVec3 pos = ray.origin+35.f*ray.direction;
		m_model->SetPosition(ray.origin+(Math::FixVec3)( 1.f / m_scale * 10.f*ray.direction));
		m_model->Draw( _cam );
	}

	// ************************************************************** //


	EditField::EditField(Jo::Files::MetaFileWrapper* _posMap, Font* _font, Math::Vec2 _position, Math::Vec2 _size, int _lines, float _fontSize) :
		ScreenTexture(_posMap, "componentBtnDefault", _position, _size, RealDimension::none),
		m_linesMax(_lines),
		m_font(_font),
		m_fontSize(_fontSize),
		m_textRender(m_font),
		m_content(""),//init with cursor indicator
		m_cursor(0)
	{
	//	m_lines.emplace_back(new TextRender(_font));
	//	SetVisibility(false);
	//	SetState(true);
		m_textRender.SetText("");
		Vec2 dim = m_textRender.GetDim();

		//automatic rezising of the text to fit the field
		if (!_fontSize)
		{
			m_fontSize = m_size[1] / dim[1];
			m_textRender.SetDefaultSize( m_fontSize );
		}

		//offset of an half char in x direction ;center in y direction
		m_textRender.SetPos(m_pos + Math::Vec2(m_fontSize * m_textRender.GetDim()[0] * 0.5f, -m_size[1] * 0.5f - m_textRender.GetExpanse()[1] * 0.5f));
	}

	void EditField::AddLine(int _preLine)
	{
	/*	int size = (int)m_lines.size();
		if(size >= m_linesMax) return;
		//create new line on right pos
		m_lines.insert(m_lines.begin()+_preLine, std::unique_ptr<TextRender>(new TextRender(m_font)));
		//erange texture and TextRenders
		for(int i = 0; i < size; i++)
			m_lines[i]->SetPos(m_vertex.position+Math::Vec2(0.02f,-0.75f*i));
		*/
	}

	bool EditField::KeyDown(int _key, int _modifiers, Math::Vec2 _pos)
	{
		//mouse click -> set cursor
		if (_key == GLFW_MOUSE_BUTTON_LEFT)
		{
		//	m_cursor = 0;
			return true; //nothing more happens
		}
		//right arrow key -> shift cursor 
		else if (_key == GLFW_KEY_RIGHT && m_cursor < m_content.size())
		{
		//	std::swap(m_content[m_cursor], m_content[m_cursor + 1]);
			m_cursor++;
		}
		//left arrow key 
		else if (_key == GLFW_KEY_LEFT && m_cursor > 0)
		{
		//	std::swap(m_content[m_cursor], m_content[m_cursor - 1]);
			m_cursor--;
		}
		//backspace
		else if (_key == GLFW_KEY_BACKSPACE && m_cursor > 0)
		{
			m_cursor--;
			m_content.erase(m_cursor, 1);
		}
		//del
		else if (_key == GLFW_KEY_DELETE && m_cursor < m_content.size())
		{
			m_content.erase(m_cursor, 1);
		}
		//printable chars
		//check whether an added char would overflow the rectangle; 1.5 to take into account the start offset
		else if (_key >= 32 && _key <= 162 && m_textRender.GetExpanse()[0] + m_textRender.GetDim()[0] * m_fontSize * 1.5f < m_size[0])
		{
			//letter A - Z without shift -> lower case
			if (_key >= 65 && _key <= 90 && !(_modifiers & GLFW_MOD_SHIFT))
				_key += 32;
			//insert happens in front of m_cursor
			m_content.insert(m_cursor, 1, _key);
			m_cursor++;
		}
		else return false;

		//update textRender
		std::string str = m_content;
		str.insert(m_cursor, 1, 'I');
		m_textRender.SetText(str);
		//calc pursor pos
		//take dimensions of the first char, as every 
//		m_cursor[0] = _pos[0] / m_font->m_sizeTable[0][0];
	//	m_cursor[1] = _pos[1] / m_fontSize;
		return true;
	}

	bool EditField::KeyUp(int _key, int _modifiers, Math::Vec2 _pos)
	{
		return true;
	}
};