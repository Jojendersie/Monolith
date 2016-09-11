#pragma once

#include <string>
#include "enginetypes.hpp"

namespace NaReTi{
	class ModuleLoader
	{
	public:
		ModuleLoader(LoaderConfig& _config) : m_config(_config){}

		std::string load(const std::string& _name);
	private:
		LoaderConfig& m_config;
	};
}