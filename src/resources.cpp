#include "resources.hpp"
#include <jofilelib.hpp>


Resources::Resources()
{
	LOG_LVL0("Created Resources Singleton");
}

Resources::~Resources()
{
	LOG_LVL0("Destroyed Resources Singleton");
}

// ************************************************************************* //
const std::string& Resources::GetText( const std::string& _key )
{
	return Instance().m_textData[_key];
}

// ************************************************************************* //
void Resources::LoadLanguageData( const std::string& _fileName )
{
	try {
		Jo::Files::HDDFile file( "languages/" + _fileName );
		Jo::Files::MetaFileWrapper wrapper( file );

		// The LangName must be contained. To test this unload the old name value
		Instance().m_textData["LangName"] = "";

		// Loop over all properties in the file except the first.
		for( int i = 0; i < wrapper.RootNode.Size(); ++i )
		{
			std::string key = wrapper.RootNode[i].GetName();
			Instance().m_textData[key] = wrapper.RootNode[i];
		}

		// One of the keys must have been LangName
		if( Instance().m_textData["LangName"] != "" )
			LOG_LVL1("Loaded language " + Instance().m_textData["LangName"]);
		else LOG_ERROR("The LangName property must be in the language object file.");
		
	} catch(std::string _message) {
		// The JoFileLib is throwing strings
		LOG_ERROR(_message);
	} catch(...) {
		LOG_ERROR("Could not load a language file for an unknown reason");
	}
}