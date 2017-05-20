#include "hudelements.hpp"
#include "math/ray.hpp"
#include "input/camera.hpp"
#include "voxel/model.hpp"
#include "graphic/interface/singlecomponentrenderer.hpp"
#include "GLFW/glfw3.h"
#include "pixelcoords.hpp"
#include "hud.hpp"

using namespace ei;

namespace Graphic
{
	ScreenTexture::ScreenTexture(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor,
		std::function<void()> _OnMouseUp) :
		ScreenOverlay(_position, _size, _def, _anchor,_OnMouseUp)
	{
		Jo::Files::MetaFileWrapper& posMap = Resources::GetTextureMap();
		m_vertex.position = m_position;
		m_vertex.texCoord = Vec2(posMap.RootNode[_name][std::string("positionX")],posMap.RootNode[_name][std::string("positionY")]);
		m_vertex.size = Vec2(posMap.RootNode[_name][std::string("sizeX")],posMap.RootNode[_name][std::string("sizeY")]);
		m_textureSize = m_vertex.size;
		if (all(Vec2(0.f) == _size))
		{
			Vec2 screenSize(posMap.RootNode[std::string("width")].Get(132), posMap.RootNode[std::string("height")].Get(132));
			screenSize *= m_vertex.size; //pixel size of this texture
			m_vertex.screenSize = PixelOffset(screenSize);

			ScreenOverlay::SetSize(m_vertex.screenSize);
		}
		else
			m_vertex.screenSize = _size;
	}

	void ScreenTexture::Register(Hud& _hud)
	{
		_hud.RegisterElement(*this);
	}

	void ScreenTexture::SetPosition(Vec2 _pos)
	{
		ScreenOverlay::SetPosition(_pos);
		m_vertex.position = m_position;// + Math::Vec2(0.f, m_vertex.screenSize[1]/2.f);
	}

	// ************************************************************************ //

	void ScreenTexture::SetSize(Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_vertex.screenSize = _size;
	}

	// ************************************************************************ //

	void ScreenTexture::Scale(Vec2 _scale)
	{
		ScreenOverlay::Scale(_scale);
		m_vertex.screenSize = m_size;
		m_vertex.position = m_position;
	}

	// ************************************************************************ //
	void ScreenTexture::SetTextureRect(ei::Vec2 _scale)
	{
		m_vertex.size = m_textureSize * _scale;
	}

	// ************************************************************************ //

	Button::Button(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor, const std::string& _caption,
		std::function<void()> _OnMouseUp, Font* _font) :
		ScreenTexture(_name+"Default", _position, _size, _def, _anchor, _OnMouseUp),
		m_btnDefault(_name+"Default", _position, _size, _def, _anchor),
		m_btnOver(_name + "Over", _position, _size, _def, _anchor),
		m_btnDown(_name + "Down", _position, _size, _def, _anchor),
		m_caption(Vec2(0.f), Anchor(), _font),
		m_autoCenter(true)
	{
		SetVisible(false); 
		SetActive(true);
		
		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);

		m_caption.SetDefaultSize(2.f);
		SetCaption(_caption);
	}

	void Button::Register(Hud& _hud)
	{
		_hud.RegisterElement(m_caption);
		m_btnDefault.Register(_hud);
		m_btnOver.Register(_hud);
		m_btnDown.Register(_hud);

		//add the collision overlay last so it is in front of the elements
		ScreenTexture::Register(_hud);

		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetPosition(Vec2 _pos)
	{
		ScreenTexture::SetPosition(_pos);
/*		m_btnDefault.SetPosition(_pos);
		m_btnOver.SetPosition(_pos);
		m_btnDown.SetPosition(_pos);
		m_caption.SetPosition(_pos);*/
		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetSize(Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_btnDefault.SetSize(_size);
		m_btnOver.SetSize(_size);
		m_btnDown.SetSize(_size);
	}

	// ************************************************************************ //

	void Button::Scale(Vec2 _scale)
	{
		ScreenTexture::Scale(_scale);
		m_btnDefault.Scale(_scale);
		m_btnOver.Scale(_scale);
		m_btnDown.Scale(_scale);

		SetCaption(m_caption.GetText());
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

		Vec2 captionDim = m_caption.GetCharSize();

		// in case that the text is to large in any direction scale it down
		float ySize = captionDim[1] * lineCount + captionDim[1] * 0.2f;
		if (captionDim[0] * charCountMax * m_caption.GetDefaultSize() >= m_size[0] || ySize >= m_size[1])
		{
			m_caption.SetDefaultSize(min( (float)(m_size[0] / (captionDim[0] * charCountMax)),
				(float)(m_size[1] / ySize)));
		}

		//set the (new) caption first to rebuild the buffer
		m_caption.SetText(_caption);

		// center in both directions
		m_caption.SetPosition(m_position+Vec2(!m_autoCenter[0] ? 0.f : ((m_size[0] - captionDim[0] * charCountMax * m_caption.GetDefaultSize()) / 2.f),
			!m_autoCenter[1] ? 0.f : (-m_size[1] / (float)lineCount * 0.5f)));//- captionDim[1] * m_caption.GetMaxSize() * 0.45f

	}

	void Button::MouseEnter()
	{
		ScreenOverlay::MouseEnter();
		m_btnDefault.SetVisible(false);
		m_btnOver.SetVisible(true);

		m_btnState = State::MouseOver;
		m_caption.SetDefaultColor(Utils::Color8U((uint8_t)210, 210, 210));
	}
	void Button::MouseLeave()
	{
		ScreenOverlay::MouseLeave();
		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);

		m_btnState = State::Base;
		m_caption.SetDefaultColor(Utils::Color8U((uint8_t)255, 255, 255));
	}
	bool Button::KeyDown(int _key, int _modifiers, Vec2 _pos)
	{
		ScreenOverlay::KeyDown(_key, _modifiers, _pos);
		m_btnDefault.SetVisible(false);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(true);

		m_btnState = State::Pressed;
		//since an object(this) is hit, the return value is always true
		return true;
	}

	bool Button::KeyUp(int _key, int _modifiers, Vec2 _pos)
	{
		ScreenOverlay::KeyUp(_key, _modifiers, _pos);
		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);
		m_btnState = State::MouseOver;

		return true;
	}
	
	
	// ************************************************************** //

	ScreenComponent::ScreenComponent( Voxel::ComponentType _component, ei::Vec2 _position, float _scale, int _sideFlags, Anchor _anchor ):
		ScreenOverlay(_position, Vec2(0.0f), TopLeft, _anchor),
		m_componentType(_component),
		m_scale(_scale),
		m_sideFlags(_sideFlags)
	{
		Center();
	}

	void ScreenComponent::SetPosition(Vec2 _pos)
	{
		ScreenOverlay::SetPosition(_pos);
		Center();
	}

	// ************************************************************************ //

	void ScreenComponent::SetSize(Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		Center();
	}

	void ScreenComponent::Center()
	{
		m_center[0] = m_position[0] + m_size[0] * 0.5f;
		m_center[1] = m_position[1] + m_size[1] * 0.5f;
	}
	
	void ScreenComponent::Draw(Graphic::SingleComponentRenderer* _renderer, const Input::Camera& _cam)
	{
		Math::WorldRay ray = _cam.GetRay(m_center);
		Mat4x4 transformation = translation( 10.0f * ray.direction - m_scale * Vec3(0.5f, 0.5f, 0.0f) );
		transformation *= rotationH( Quaternion(-asin(ray.direction.y) - 0.25f, 0.0f, 0.0f) * Quaternion(0.0f, 1.0f, 0.0f) );
		transformation *= scalingH( m_scale );
		_renderer->Draw(m_componentType, m_sideFlags, transformation, _cam.GetProjection());
	}

	// ************************************************************** //


	EditField::EditField(Vec2 _position, Vec2 _size, Font* _font, int _lines, float _fontSize) :
		ScreenTexture("componentBtnDefault", _position, _size),
		m_linesMax(_lines),
		m_font(_font),
		m_fontSize(_fontSize),
		m_textRender(Vec2(0.f), Anchor(), m_font),
		m_content(""),//init with cursor indicator
		m_cursor(0)
	{
	//	m_lines.emplace_back(new TextRender(_font));
	//	SetVisible(false);
	//	SetActive(true);
		m_textRender.SetText("");
		Vec2 dim = m_textRender.GetCharSize();

		const float textToBoxSize = 0.75f;
		//automatic rezising of the text to fit the field
		if (!_fontSize)
		{
			m_fontSize = m_size[1] * textToBoxSize / dim[1];
			m_textRender.SetDefaultSize( m_fontSize );
		}
		m_textRender.SetRectangle(_size);
		//offset of an half char in x direction ;center in y direction
		m_textRender.SetPosition(m_position + Vec2(m_fontSize * m_textRender.GetCharSize()[0] * 0.5f, -m_size[1] * (textToBoxSize) * 0.5f/* * 0.5f /*- m_textRender.GetRectangle()[1] * 0.5f*/));
	}

	void EditField::Register(Hud& _hud)
	{
		ScreenTexture::Register(_hud);
		_hud.RegisterElement(m_textRender);
	}

	// ************************************************************** //

	void EditField::AddLine(int _preLine)
	{
	/*	int size = (int)m_lines.size();
		if(size >= m_linesMax) return;
		//create new line on right pos
		m_lines.insert(m_lines.begin()+_preLine, std::unique_ptr<TextRender>(new TextRender(m_font)));
		//erange texture and TextRenders
		for(int i = 0; i < size; i++)
			m_lines[i]->SetPosition(m_vertex.position+Math::Vec2(0.02f,-0.75f*i));
		*/
	}

	bool EditField::KeyDown(int _key, int _modifiers, Vec2 _pos)
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
		else if (_key >= 32 && _key <= 162 && m_textRender.GetRectangle()[0] + m_textRender.GetCharSize()[0] * m_fontSize * 1.5f < m_size[0])
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

	bool EditField::KeyUp(int _key, int _modifiers, Vec2 _pos)
	{
		return true;
	}

	// ************************************************************************ //
	FillBar::FillBar(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def,
		Anchor _anchor) :
		ScreenTexture("simpleWindow", _position, _size, _def, _anchor)
	{
		
	}

	// ************************************************************************ //

	void FillBar::SetFillLevel(float _level)
	{
		Vec2 vec(_level, 1.f);
		SetScale(vec);
		SetTextureRect(vec);
		//to update texture vertex
		//todo: remove this
		Scale(Vec2(1.f));
	}
};