#pragma once

#include <functional>
#include <ei/vector.hpp>
#include <vector>

namespace Graphic{


	enum DefinitionPoint
	{
		TopLeft,
		TopMid,
		TopRight,
		MidLeft,
		MidMid,
		MidRight,
		BotLeft,
		BotMid,
		BotRight
	};

	class ScreenOverlay;

	// a position on the screen that can be described relative to a ScreenOverlay
	class ScreenPosition
	{
	public:
		struct Anchor
		{
			Anchor(DefinitionPoint _defPoint = DefinitionPoint::TopLeft, ScreenOverlay* _parent = nullptr) :
				definitionPoint(_defPoint), parent(_parent){}

			DefinitionPoint definitionPoint;
			ScreenOverlay* parent;
		};

		ScreenPosition(ei::Vec2 _position = ei::Vec2(), Anchor _anchor = Anchor());
		virtual ~ScreenPosition(){};

		/// \ Functions to alter the rectangle in lifetime
		virtual void SetPosition(ei::Vec2 _pos);

		/// \brief Called once by the hud to add it and its child.
		/// \details The hud should take full ownership of all elements.
		virtual void Register(class Hud& _hud) {};

		const Anchor& GetAnchor() const { return m_anchor; }
	protected:
		void UpdateParams();

		ei::Vec2 m_position;///< position in screen coordsystem
	private:
		ei::Vec2 m_positionDef; ///< position as given without the offset from the def point
		Anchor m_anchor;

		friend class Hud; //todo remove this
	};

	/// \brief a basic class for 2d screen elements
	/// \details a storage for size information and capable of interaction with the mouse and keys when managed by a hud
	/// _position is the upper left corner of the Overlay in screenspace [-1,1]x[-1,1] starting from the lower left corner
	/// _size is the size in screen space
	/// both are relative to the owning parent(Hud), thus a size of 2.f will make that the Overlay has the size of the Hud 

	//todo: rework rendering of hud elements
	// add virtual void Draw()
	// check current shader/ texture and set only if necessary
	class ScreenOverlay : public ScreenPosition
	{
	public:
		ScreenOverlay(ei::Vec2 _pos, 
			ei::Vec2 _size, 
			DefinitionPoint _def = DefinitionPoint::TopLeft,
			ScreenPosition::Anchor _anchor = ScreenPosition::Anchor(),
			std::function<void()> _OnMouseUp = []() {return; });

		virtual ~ScreenOverlay() {}

		virtual void Register(class Hud& _hud) override;

		void SetPosition(ei::Vec2 _pos) override;
		/// \brief Sets the base size. Scale will still be applied to this value.
		virtual void SetSize(ei::Vec2 _size);
		/// \brief Returns the actual size after scale has been applied to it.
		ei::Vec2 GetSize() const { return m_size; }

		/// \brief Overwrites the scale with _scale.
		void SetScale(ei::Vec2 _scale);
		ei::Vec2 GetScale() const { return m_scale; }

		/// \brief Scales the current size with _scale.
		virtual void Scale(ei::Vec2 _scale);

		/// \brief An inactive screen overlay does not receive player input.
		void SetState(bool _state) { m_active = _state; };
		void SetVisibility(bool _visibility){ m_visible = _visibility; };

		bool GetState() const { return m_active; };
		bool GetVisibility() const { return m_visible; };

		//Only the overlay in front receives this events

		virtual void MouseMove(double _dx, double _dy){ return; };
		/// \brief Triggered when the mouse entered the tex this frame
		virtual void MouseEnter(){ if (OnMouseEnter != NULL) OnMouseEnter(); };
		/// \brief Triggered when the mouse leaved the tex this frame
		virtual	void MouseLeave(){ if (OnMouseLeave != NULL) OnMouseLeave(); };
		/// \brief Called when left mouse buttons goes down inside the rectangle; @param _pos Pos of Mouse relative to the button
		///returns true when input gets captured
		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f, 0.f)){ if (OnMouseDown != NULL) OnMouseDown(); return true; };
		/// \brief Called when left mouse buttons goes up inside the rectangle; @param _pos Pos of Mouse relative to the button
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos = ei::Vec2(0.f, 0.f)){ if (OnMouseUp != NULL) OnMouseUp(); return true; };

		/// \brief Only triggered for the overlay the mouse is in; Does nothing by default 
		virtual bool Scroll(double _dx, double _dy){ return false; };

	protected:
		void UpdateParams(); // calculates real size and position from the defined params

		bool m_active; ///< when false: gets ignored by everything
		bool m_visible; ///< visibility

		//events
		std::function<void()> OnMouseEnter;
		std::function<void()> OnMouseLeave;
		std::function<void()> OnMouseDown;
		std::function<void()> OnMouseUp;

		ei::Vec2 m_size;///< actual size in screen coordsystem after scaling
		ei::Vec2 m_scale; ///< size is multiplied with this

		std::vector<ScreenOverlay*> m_childs;
	private:
		ei::Vec2 m_sizeBase; ///< the actual size before scaling
		DefinitionPoint m_definitionPoint;

		friend class BasicHud;
		friend class Hud;
	};
}