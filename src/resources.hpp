#pragma once

#include "predeclarations.hpp"
#include <unordered_map>

/// \brief Global class for shared resources access.
class Resources
{
public:
	/// \brief Load text in the current chosen language (defined by config).
	/// \param [in] _key The lookup key to find the correct text. A key must
	///		be unique.
	static const std::string& GetText( const std::string& _key );

	/// \brief Load the translation data from a given file.
	/// \details The load will override all texts contained in the file. If
	///		a file does not define a value the old value will be kept.
	static void LoadLanguageData( const std::string& _fileName );
private:
	std::unordered_map<std::string, std::string> m_textData;

	/// \brief Singleton access
	static Resources& Instance()
	{
		static Resources s_resources;
		return s_resources;
	}
	Resources();
	~Resources();
};