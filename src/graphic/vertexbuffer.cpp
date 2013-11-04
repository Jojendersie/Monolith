#include "vertexbuffer.hpp"
#include "../opengl.hpp"
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdint>

namespace Graphic {
// ************************************************************************* //
// VERTEX BUFFER
// ************************************************************************* //
// Helper function to work with the format specification
inline int VectorSize(char a)
{
	switch(a)
	{
	case '4': return 4;
	case '3': case 'p': case 'n': case 't': return 3;
	case '2': return 2;
	case '1': case 'c': case 'u': return 1;
	}
	return 0;
}

// Creating a vertex buffer with arbitary interleaved data
VertexBuffer::VertexBuffer(unsigned _maxNumVertices, const char* _vertexDeclaration, PrimitiveType _type) :
	m_data(nullptr),
	m_maxNumVertices(_maxNumVertices),
	m_dirty(true),
	m_cursor(0),
	m_positionOffset(0xffffffff),
	m_normalOffset(0xffffffff),
	m_tangentOffset(0xffffffff),
	m_primitiveType(_type)
{
	// First step analysing the string
		// Counting VBO's size and string length
		int len = 0;
		m_vertexSize = 0;
		while(_vertexDeclaration[len])
		{
			if(_vertexDeclaration[len] == 'n') m_normalOffset = m_vertexSize;
			else if(_vertexDeclaration[len] == 'p') m_positionOffset = m_vertexSize;
			else if(_vertexDeclaration[len] == 't') m_tangentOffset = m_vertexSize;
			m_vertexSize += VectorSize(_vertexDeclaration[len]) * 4;
			++len;
		}

	// Second step create VAO and VBOs
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
		// Create Vertex buffer object
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);	// Bind VBO to VAO
		int i=0;
		int stride = 0, bindOff = 0, colorBindOff = 0, uintBindOff = 0;
		while(_vertexDeclaration[i])
		{
			int location;
			switch(_vertexDeclaration[i])
			{
			case 'p': location = int(BindingLocation::POSITION);
				glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 3*sizeof(float);
				break;
			case 'n': location = int(BindingLocation::NORMAL);
				glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 3*sizeof(float);
				break;
			case 't': location = int(BindingLocation::TANGENT);
				glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 3*sizeof(float);
				break;
			case '1': location = int(BindingLocation::VEC) + bindOff;
				glVertexAttribPointer(location, 1, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += sizeof(float);
				++bindOff;
				break;
			case '2': location = int(BindingLocation::VEC) + bindOff;
				glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 2*sizeof(float);
				++bindOff;
				break;
			case '3': location = int(BindingLocation::VEC) + bindOff;
				glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 3*sizeof(float);
				++bindOff;
				break;
			case '4': location = int(BindingLocation::VEC) + bindOff;
				glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 4*sizeof(float);
				++bindOff;
				break;
			case 'c':
				if(colorBindOff>=3) std::cout << "[VertexBuffer::VertexBuffer] Too many 'c' in vertex declaration. The maximum number of color binding points is 4!\n";
				location = int(BindingLocation::COLOR0) + colorBindOff;
				glVertexAttribPointer(location, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 4;
				++colorBindOff;
				break;
			case 'u':
				if(uintBindOff>=3) std::cout << "[VertexBuffer::VertexBuffer] Too many 'u' in vertex declaration. The maximum number of uint binding points is 4!\n";
				location = int(BindingLocation::UINT0) + uintBindOff;
				glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, 0, (GLvoid*)(stride));
				glEnableVertexAttribArray(location);
				stride += 4;
				++uintBindOff;
				break;
			}
			++i;
		}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if( m_maxNumVertices * m_vertexSize )
	{
		m_data = (uint8_t*)malloc(m_maxNumVertices * m_vertexSize);
	}
}

// ************************************************************************* //
VertexBuffer::~VertexBuffer()
{
	// Detach and delete Vertex buffer
	glBindVertexArray(m_VAO);
	//glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_VBO);
	// Detach and delete array
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_VAO);

	free(m_data);
	m_data = nullptr;
}

// ************************************************************************* //
void VertexBuffer::Add(void* _value)
{
	if(m_cursor == m_maxNumVertices)
	{std::cout << "[VertexBuffer::Add] Vertex buffer full (capacity: "<<m_maxNumVertices<<"). Add has no effect.\n"; return;}

	memcpy(m_data + m_cursor * m_vertexSize, _value, m_vertexSize);
	++m_cursor;
	m_dirty = true;
}

// ************************************************************************* //
void VertexBuffer::Set(unsigned _index, const void* _value)
{
	if( IsStatic() ) {std::cout << "[VertexBuffer::Set] Cannot set vertices in a static buffer.\n"; return;}
	assert( 0<=_index && _index < GetNumVertices() );
	memcpy(m_data + _index * m_vertexSize, _value, m_vertexSize);
	m_dirty = true;
}

// ************************************************************************* //
void* VertexBuffer::Get(unsigned _index)
{
	if( IsStatic() ) {std::cout << "[VertexBuffer::Get] Cannot get vertices in a static buffer.\n"; return nullptr;}
	assert( 0<=_index && _index < GetNumVertices() );
	// Assume write access from outside
	m_dirty = true;
	return m_data + _index * m_vertexSize;
}

// ******************************************************************************** //
void VertexBuffer::MakeStatic()
{
	if(m_dirty) Commit();
	free(m_data);
	m_data = nullptr;
}

// ******************************************************************************** //
void VertexBuffer::Commit()
{
	if(m_data && m_dirty)
	{
		//glBindVertexArray(m_dwVAO);
		// Discard everything on gpu and upload new
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		//glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, m_cursor * m_vertexSize, m_data, GL_STATIC_DRAW);
		m_dirty = false;
	}
}

// ******************************************************************************** //
void VertexBuffer::DeleteVertex(unsigned _index)
{
	assert( m_cursor > 0 );	// Number of vertices
	if(m_cursor <= _index) std::cout << "[VertexBuffer::DeleteVertex] Vertex cannot be deleted: Index too large.\n";
	--m_cursor;
	if(_index != m_cursor) Set(_index, Get(m_cursor));
}

// ******************************************************************************** //
void VertexBuffer::FlipNormals()
{
	if(m_normalOffset == 0xffffffff) return;
	for(unsigned i=0; i<GetNumVertices(); ++i)
	{
		// TODO: if vector existing
	//	OrE::Math::Vec3* pN = (OrE::Math::Vec3*)(((uint8_t*)m_data)+i*m_vertexSize+m_normalOffset);
	//	(*pN) = -(*pN);
	}
}

// ******************************************************************************** //
void VertexBuffer::Bind() const
{
	// Don't use a empty buffer. Call MakeStatic or Commit before.
	assert( !m_dirty );

	glBindVertexArray(m_VAO);
}



};