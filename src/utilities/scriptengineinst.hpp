#pragma once

#include "scriptengine.hpp"
#include "ei/vector.hpp"

namespace Script{
	enum DisplayMode{
		PlainText,
		Bar
	};

	struct DisplayValue
	{
		DisplayMode mode;
	//	ei::Vec2 position;
		float value;
		char* string;
	};

	typedef std::vector<Script::DisplayValue* const> VarRefContainer;
}

extern NaReTi::ScriptEngine g_scriptEngine;