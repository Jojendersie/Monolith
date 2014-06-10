#include "hudelements.hpp"
#include "..\..\math\ray.hpp"
#include "..\..\input\camera.hpp"
#include "../../voxel/model.hpp"

namespace Graphic
{
	ScreenTexture::ScreenTexture(Jo::Files::MetaFileWrapper* _posMap, std::string _name, Math::Vec2 _position, Math::Vec2 _size,
		RealDimension _rDim, std::function<void()> _OnMouseUp):
		ScreenOverlay(_position, _size, _OnMouseUp),
		m_realDimension(_rDim),
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
		m_caption(_font)
	{
		SetVisibility(false); 
		SetState(true);
		//scan for linebreaks to adjust the line height
		m_caption.SetPos(_position+Math::Vec2(0.02f,-_size[1] * 0.5f - m_caption.GetDim()[1] * m_caption.GetMaxSize() * 0.5f));//-0.75f*_size[1]
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
		int len = _caption.length();
		int lineCount = 1;
		for(int i = 0; i < len; i++)
			if(_caption[i] == '\n') lineCount++;
		m_caption.SetPos(m_pos+Math::Vec2(0.02f,-m_size[1]/lineCount * 0.5f - m_caption.GetDim()[1] * 0.7f));//*0.5f
		m_caption.SetText(_caption);
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

	ScreenModel::ScreenModel(Math::Vec2 _position, Math::Vec2 _size, Voxel::Model* _model ):
		ScreenOverlay(_position, _size),
		m_model(_model)
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
		m_model->SetPosition(ray.origin+(Math::FixVec3)(42.f*ray.direction));
		m_model->Draw( _cam );
	}

	// ************************************************************** //


	Editfield::Editfield(Jo::Files::MetaFileWrapper* _posMap, Font* _font, Math::Vec2 _position, Math::Vec2 _size, int _lines, float _fontSize):
		ScreenTexture(_posMap, "EditField", _position, _size),
		m_linesMax(_lines),
		m_font(_font),
		m_fontSize(_fontSize)
	{
		m_lines.push_back(std::unique_ptr<TextRender>(new TextRender(_font)));
	//	SetVisibility(false); 
	//	SetState(true);
		m_lines[0]->SetPos(_position+Math::Vec2(0.02f,-0.75f*_size[1]));
	//	m_lines[0].SetText("insert text here");
	}

	void Editfield::AddLine(int _preLine)
	{
		int size = m_lines.size();
		if(size >= m_linesMax) return;
		//create new line on right pos
		m_lines.insert(m_lines.begin()+_preLine, std::unique_ptr<TextRender>(new TextRender(m_font)));
		//erange texture and TextRenders
		for(int i = 0; i < size; i++)
			m_lines[i]->SetPos(m_vertex.position+Math::Vec2(0.02f,-0.75f*i));
	}

	bool Editfield::KeyDown(int _key, int _modifiers, Math::Vec2 _pos)
	{
		//calc pursor pos
		//take dimensions of the first char, as every 
//		m_cursor[0] = _pos[0] / m_font->m_sizeTable[0][0];
	//	m_cursor[1] = _pos[1] / m_fontSize;
		return true;
	}

	bool Editfield::KeyUp(int _key, int _modifiers, Math::Vec2 _pos)
	{
		return true;
	}
};