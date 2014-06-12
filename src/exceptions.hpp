#pragma once

#include <exception>
#include <string>

/// \brief Exception which is generated if invalid files are load
class InvalidSaveGame: std::exception
{
public:
	InvalidSaveGame( const Jo::Files::IFile& _file, const std::string& _where );
	const char* what() const	{ return m_what.c_str(); }
private:
	std::string m_what;
};