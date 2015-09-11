#include "graphic\interface\hud.hpp"

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class HudGsPlay : public Hud
	{
	public:
		/// \brief Creates an Hud object wich handles a 2d interface on the specfied screen rectangle
		HudGsPlay(Monolith* _game);
		
		TextRender* m_velocityLabel;

		MessageBox* m_mainMessageBox;
	private:
		
	};
};
