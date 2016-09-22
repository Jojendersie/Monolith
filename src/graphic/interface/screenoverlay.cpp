#include "screenoverlay.hpp"
#include "hud.hpp"

namespace Graphic{
	using namespace ei;

	ScreenOverlay::ScreenOverlay(ei::Vec2 _pos, ei::Vec2 _size, 
		DefinitionPoint _def, Anchor _anchor,
		std::function<void()> _OnMouseUp)
		: m_sizeBase(_size), 
		m_positionDef(_pos),
		m_active(true), 
		m_visible(true), 
		OnMouseUp(_OnMouseUp),
		m_scale(1.f),
		m_anchor(_anchor),
		m_definitionPoint(_def)
	{
		UpdateParams();
		if (_anchor.parent) _anchor.parent->m_childs.push_back(this);
	}

	void ScreenOverlay::SetPosition(Vec2 _pos)
	{
		m_positionDef = _pos;

		UpdateParams();
	}

	void ScreenOverlay::Register(Hud& _hud)
	{
		_hud.AddScreenOverlay(this);
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
		m_size = m_scale * m_sizeBase;

		int x = (int)m_definitionPoint % 3;
		int y = (int)m_definitionPoint / 3;

		m_position = m_positionDef + Vec2(-x * 0.5f * m_size.x,
			-y * 0.5f * m_size.y);

		if (m_anchor.parent)
		{
			int px = (int)m_anchor.definitionPoint % 3;
			int py = (int)m_anchor.definitionPoint / 3;

			m_position += m_anchor.parent->m_position + Vec2(px * 0.5f * m_anchor.parent->m_size.x,
				-py * 0.5f * m_anchor.parent->m_size.y);
		}

		//elements relative to this have to be updated
	//	for (auto child : m_childs) child->UpdateParams();
	}
}