#include "uniformbuffer.hpp"
#include "opengl.hpp"
#include <cstdint>
#include <cassert>

namespace Graphic {

	static int g_iNumUBOs = 0;

	UniformBuffer::UniformBuffer( const std::string& _name ) :
		m_name(_name), m_size(0), m_isDirty(false)
	{
		glGenBuffers(1, &m_bufferID);
		m_index = g_iNumUBOs++;

		// Allocate memory on CPU side. The maximum size is 1KB.
		m_memory = malloc(1024);

		// Create GPU side memory
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferData(GL_UNIFORM_BUFFER, 1024, 0, GL_STREAM_DRAW);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
		free(m_memory);
	}


	void UniformBuffer::AddAttribute( const std::string& _name, ATTRIBUTE_TYPE _type )
	{
		// Attribute already exists!
		assert(m_attributes.find(_name) == m_attributes.end());

		// Determine alignment
		int offset = m_size & 0xf;	// modulo 16
		if( offset )
		{
			if( (m_size/16) == ((m_size+int(_type)-1)/16) )
				// Variable does not skip a 16 byte alignment border
				offset = 0;
			else offset = 16 - offset;
		}
		offset += m_size;

		// Is there still memory?
		if( int(_type)+offset > 1024 ) { LOG_ERROR("Size of uniform buffer not large enough."); return; }

		// All right add to map
		m_attributes.insert( std::pair<std::string,int>( _name, offset ) );
		m_size = int(_type)+offset;
	}

	UniformBuffer::UniformVar UniformBuffer::operator [] (const std::string& _name)
	{
		// Cannot access unknown attribute!
		assert(m_attributes.find(_name) != m_attributes.end());

		return UniformVar((uint8_t*)m_memory + m_attributes[_name], this);
	}

	const UniformBuffer::UniformVar UniformBuffer::operator [] ( const std::string& _name ) const
	{
		// Cannot access unknown attribute!
		assert(m_attributes.find(_name) != m_attributes.end());

		// Give a nullptr instead of a buffer - it is only accessed during write
		return UniformVar((uint8_t*)m_memory + m_attributes.at(_name), nullptr);
	}


	void UniformBuffer::Commit()
	{
		// Bind to binding point according to its index
		glBindBufferBase(GL_UNIFORM_BUFFER, m_index, m_bufferID);

		if( m_isDirty )
		{
			// Upload the whole used part of the buffer. It could be more efficient
			// to upload only the changed part.
			glBindBuffer( GL_UNIFORM_BUFFER, m_bufferID );
			glBufferSubData( GL_UNIFORM_BUFFER, 0, m_size, m_memory );

			// The following line forces a sync on Intel HD chips. Otherwise reseting the buffer
			// twice has no effect. (In case both times the same program is used.)
			glFlush();

#ifdef _DEBUG
			LogGlError("An error during binding and uploading data occured.");
#endif

			m_isDirty = false;
		}
	}
};