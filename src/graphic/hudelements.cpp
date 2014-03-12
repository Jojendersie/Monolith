#include "hudelements.hpp"

namespace Graphic
{
	ScreenTexture::ScreenTexture(Jo::Files::MetaFileWrapper* _posMap, std::string _name, Math::Vec2 _position, Math::Vec2 _position2,
		std::function<void()> _OnMouseUp):
		OnMouseUp(_OnMouseUp),
		m_active(true),
		m_visible(true)
	{
		m_vertex.position = _position;
		m_vertex.texCoord = Math::Vec2(_posMap->RootNode[_name][std::string("positionX")],_posMap->RootNode[_name][std::string("positionY")]);
		m_vertex.size = Math::Vec2(_posMap->RootNode[_name][std::string("sizeX")],_posMap->RootNode[_name][std::string("sizeY")]);
		m_vertex.screenSize = Math::Vec2(0.f,0.f) == _position2 ? m_vertex.size : _position2 ;
	}

	void ScreenTexture::MouseEnter()
	{
		if(OnMouseEnter != NULL) OnMouseEnter();
	}
	void ScreenTexture::MouseLeave()
	{
		if(OnMouseLeave != NULL) OnMouseLeave();
	}
	void ScreenTexture::MouseDown()
	{
		if(OnMouseDown != NULL) OnMouseDown();
	}

	void ScreenTexture::MouseUp()
	{
		if(OnMouseUp != NULL) OnMouseUp();
	}

	Button::Button(Jo::Files::MetaFileWrapper* _posMap, Font* _font, std::string _name, Math::Vec2 _position, Math::Vec2 _size,
		 std::function<void()> _OnMouseUp ):
		ScreenTexture(_posMap, _name+"Default", _position, _size, _OnMouseUp),
		m_btnDefault(_posMap, _name+"Default", _position, _size),
		m_btnOver(_posMap, _name+"Over", _position, _size),
		m_btnDown(_posMap, _name+"Down", _position, _size),
		m_caption(_font)
	{
		SetVisibility(false); 
		SetState(true);
		m_caption.SetPos(_position+Math::Vec2(0.02f,-0.75f*_size[1]));
		m_caption.SetText(_name);
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
	}

	void Button::MouseEnter()
	{
		ScreenTexture::MouseEnter();
		m_btnDefault.SetVisibility(false);
		m_btnOver.SetVisibility(true);
	}
	void Button::MouseLeave()
	{
		ScreenTexture::MouseLeave();
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
	}
	void Button::MouseDown()
	{
		ScreenTexture::MouseDown();
		m_btnDefault.SetVisibility(false);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(true);
	}

	void Button::MouseUp()
	{
		ScreenTexture::MouseUp();
		m_btnDefault.SetVisibility(true);
		m_btnOver.SetVisibility(false);
		m_btnDown.SetVisibility(false);
	}

};