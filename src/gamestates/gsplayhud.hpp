#pragma once

#include "graphic\interface\hud.hpp"
#include "utilities/scriptengineinst.hpp"

namespace Graphic {
	
	/// \brief A class that handles (2d)graphic overlays.
	class HudGsPlay : public Hud
	{
	public:
		/// \brief Creates an Hud object wich handles a 2d interface on the specfied screen rectangle
		HudGsPlay(Monolith* _game);
		
		TextRender* m_velocityLabel;
		TextRender* m_targetVelocityLabel;

		MessageBox* m_mainMessageBox;

		FillBar* m_batteryDisplay; // todo: have these generated for existing systems

		Script::VarRefContainer& GetScriptVars() { return m_scriptVars; };
		void BuildScriptVars();
		void UpdateScriptVars();
	private:
		Script::VarRefContainer m_scriptVars;
		std::vector<FillBar*> m_scriptDisplays;
	};
};
