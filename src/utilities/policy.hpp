#pragma once

#include <string>

namespace Jo {
namespace Logger {

	class Policy
	{
	public:
		virtual ~Policy()	{}
		
		virtual void Write( const std::string& _message ) = 0;
	};
	
	class FilePolicy: public Policy
	{
	public:
		FilePolicy( const std::string& _fileName );
		
		~FilePolicy();
		
		void Write( const std::string& _message ) override;
		
	private:
		FILE* m_file;
	};

} // namespace Logger
} // namespace Jo