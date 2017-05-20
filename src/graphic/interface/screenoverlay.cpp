#include "screenoverlay.hpp"
#include "hud.hpp"

namespace Graphic{
	using namespace ei;


	ScreenPosition::ScreenPosition(ei::Vec2 _position, Anchor _anchor):
		m_anchor(_anchor),
		m_positionDef(_position)
	{
		UpdateParams();
	}

	void ScreenPosition::SetPosition(Vec2 _position)
	{
		m_positionDef = _position;

		UpdateParams();
	}

	void ScreenPosition::UpdateParams()
	{
		m_position = m_positionDef;

		if (m_anchor.parent)
		{
			int px = (int)m_anchor.definitionPoint % 3;
			int py = (int)m_anchor.definitionPoint / 3;

			m_position += m_anchor.parent->m_position + Vec2(px * 0.5f * m_anchor.parent->GetSize().x,
				-py * 0.5f * m_anchor.parent->GetSize().y);
		}
	}

	// ************************************************* //
	ScreenOverlay::ScreenOverlay(ei::Vec2 _pos, ei::Vec2 _size, 
		DefinitionPoint _def, Anchor _anchor,
		std::function<void()> _OnMouseUp)
		: ScreenPosition(_pos, _anchor),
		m_sizeBase(_size), 
		m_active(true), 
		m_visible(true), 
		OnMouseUp(_OnMouseUp),
		m_scale(1.f),
		m_definitionPoint(_def)
	{
		UpdateParams();
		if (_anchor.parent) _anchor.parent->m_childs.push_back(this);
	}

	void ScreenOverlay::SetPosition(Vec2 _pos)
	{
		ScreenPosition::SetPosition(_pos);

		UpdateParams();
	}

	void ScreenOverlay::Register(Hud& _hud)
	{
		_hud.RegisterElement(*this);
	}

	// ************************************************* //

	void ScreenOverlay::SetSize(Vec2 _size)
	{
		m_sizeBase = _size;

		UpdateParams();
	}

	void ScreenOverlay::SetScale(Vec2 _scale)
	{
		m_scale = _scale;

		UpdateParams();
	}

	void ScreenOverlay::Scale(Vec2 _scale)
	{
		m_scale *= _scale;

		UpdateParams();
	}

	// ************************************************* //

	void ScreenOverlay::UpdateParams()
	{
		ScreenPosition::UpdateParams();

		m_size = m_scale * m_sizeBase;

		int x = (int)m_definitionPoint % 3;
		int y = (int)m_definitionPoint / 3;

		m_position += Vec2(-x * 0.5f * m_size.x,
			-y * 0.5f * m_size.y);
	}
}