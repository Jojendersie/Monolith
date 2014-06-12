#include <jofilelib.hpp>
#include "exceptions.hpp"

// ************************************************************************* //
InvalidSaveGame::InvalidSaveGame( const Jo::Files::IFile& _file, const std::string& _where )
{
	m_what = std::string("Cannot read file: ") + _file.Name() + " with: " + _where;
}