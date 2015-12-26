#include "graphic\interface\hud.hpp"
#include <array>

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class HudGsEditor : public Hud
	{
	public:
		/// \brief Creates an Hud object which handles a 2d interface on the specified screen rectangle
		/// \param [in] _componentRenderer Reference to a renderer to show single component previews.
		///		The HUD class does not take the ownership.
		HudGsEditor(Monolith* _game, Graphic::SingleComponentRenderer* _componentRenderer);

		EditField* m_nameEdit;

		Hud* m_voxelContainer;
		Hud* m_modelInfoContainer;

		TextRender* m_mass;
		std::array < TextRender*, 6 > m_resourceCosts;
	private:
	};
};
