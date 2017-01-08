#pragma once

#include "scriptengine.hpp"

namespace Script{
	enum DisplayMode{
		PlainText,
		Bar
	};

	struct DisplayValue
	{
		float value;
		DisplayMode mode;
	};

	typedef std::vector<struct Script::DisplayValue* const> VarRefContainer;
}

extern NaReTi::ScriptEngine g_scriptEngine;