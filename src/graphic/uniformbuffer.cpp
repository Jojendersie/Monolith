#include "uniformbuffer.hpp"
#include "../opengl.hpp"
#include <iostream>
#include <cstdint>
#include <cassert>

namespace Graphic {

	static int g_iNumUBOs = 0;

	UniformBuffer::UniformBuffer( const std::string& _sName ) :
		m_sName(_sName), m_iSize(0)
	{
		glGenBuffers(1, &m_iBufferID);
		m_iIndex = g_iNumUBOs++;

		// Allocate memory on CPU side. The maximum size is 1KB.
		m_pMemory = malloc(1024);

		// Create GPU side memory
		glBindBuffer(GL_UNIFORM_BUFFER, m_iBufferID);
		glBufferData(GL_UNIFORM_BUFFER, 1024, 0, GL_STREAM_DRAW);

		// Bind to binding point according to its index
		glBindBufferBase(GL_UNIFORM_BUFFER, m_iIndex, m_iBufferID);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_iBufferID);
		free(m_pMemory);
	}


	void UniformBuffer::AddAttribute( const std::string& _sName, ATTRIBUTE_TYPE _Type )
	{
		// Attribute already exists!
		assert(m_Attributes.find(_sName) == m_Attributes.end());

		// Determine alignment
		int iOffset = m_iSize & 0xf;	// modulu 16
		if( iOffset && (m_iSize/16) == ((m_iSize+int(_Type))/16) )
			// Variable does not skip a 16 byte alignment border
			iOffset = 0;
		else iOffset = 16 - iOffset;
		iOffset += m_iSize;

		// Is there still memory?
		if( int(_Type)+iOffset > 1024 ) { std::cout << "[UniformBuffer::AddAttribute] Size of uniform buffer not large enough."; return; }

		// All right add to map
		m_Attributes.insert( std::pair<std::string,int>( _sName, iOffset ) );
		m_iSize = int(_Type)+iOffset;
	}

	void* UniformBuffer::operator [] (const std::string& _sName)
	{
		// Cannot access unkonw attribute!
		assert(m_Attributes.find(_sName) != m_Attributes.end());

		return (uint8_t*)m_pMemory + m_Attributes[_sName];
	}


	void UniformBuffer::BindToShader( unsigned _iProgramID )
	{
		unsigned iIndex = glGetUniformBlockIndex(_iProgramID, m_sName.c_str());
		// Ignore the errors. There are shaders without the blocks
		if( !glGetError() && iIndex!=GL_INVALID_INDEX )
			glUniformBlockBinding(_iProgramID, iIndex, m_iIndex);
	}


	void UniformBuffer::Commit()
	{
		// Upload the whole used part of the buffer. It could be more efficient
		// to upload only the changed part.
		glBindBuffer(GL_UNIFORM_BUFFER, m_iBufferID);
		glBufferSubData( GL_UNIFORM_BUFFER, 0, m_iSize, m_pMemory );

		// The following line forces a sync on Intel HD chips. Otherwise reseting the buffer
		// twice has no effect. (In case both times the same program is used.)
		//glFlush();

		const GLenum ErrorValue = glGetError();
		if (ErrorValue != GL_NO_ERROR)
			std::cout << "[UniformBuffer::Commit] : An error in binding and uoloading data occured.\n";
	}
};