#include "graphic\interface\hud.hpp"
#include <array>

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class HudGsEditor : public Hud
	{
	public:
		/// \brief Creates an Hud object wich handles a 2d interface on the specfied screen rectangle
		HudGsEditor(Monolith* _game);

		EditField* m_nameEdit;

		Hud* m_voxelContainer;
		Hud* m_modelInfoContainer;

		TextRender* m_mass;
		std::array < TextRender*, 6 > m_resourceCosts;
	private:
		
	};
};
