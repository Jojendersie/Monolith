#pragma once

#include "policy.hpp"
#include <cstdio>
#include <iostream>

namespace Jo {
namespace Logger {

	FilePolicy::FilePolicy( const std::string& _fileName )
	{
		m_file = fopen(_fileName.c_str(), "wb");
		if( !m_file ) throw "Cannot open file '" + _fileName + "' for logging.";
	}
	
	FilePolicy::~FilePolicy()
	{
		fclose( m_file );
	}
	
	void FilePolicy::Write( const std::string& _message )
	{
		fwrite( _message.c_str(), _message.length(), 1, m_file );
		// Modification to original logging lib: copy into window
		std::cout << _message;
	}

} // namespace Logger
} // namespace Jo