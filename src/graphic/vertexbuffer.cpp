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
	case '3':case 'p':case 'n':case 't': return 3;
	case '2': return 2;
	case '1':case 'c': return 1;
	}
	return 0;
}

// Creating a vertex buffer with arbitary interleaved data
VertexBuffer::VertexBuffer(unsigned _iMaxNumVertices, const char* _pcVD) :
	m_pData(nullptr),
	m_iMaxNumVertices(_iMaxNumVertices),
	m_bDirty(true),
	m_iCursor(0),
	m_iPositionOffset(0xffffffff),
	m_iNormalOffset(0xffffffff),
	m_iTangentOffset(0xffffffff)
{
	// First step analysing the string
		// Counting VBO's size and string length
		int iLen = 0;
		m_iVertexSize = 0;
		while(_pcVD[iLen])
		{
			if(_pcVD[iLen] == 'n') m_iNormalOffset = m_iVertexSize;
			else if(_pcVD[iLen] == 'p') m_iPositionOffset = m_iVertexSize;
			else if(_pcVD[iLen] == 't') m_iTangentOffset = m_iVertexSize;
			m_iVertexSize += VectorSize(_pcVD[iLen]) * 4;
			++iLen;
		}

	// Second step create VAO and VBOs
	glGenVertexArrays(1, &m_iVAO);
	glBindVertexArray(m_iVAO);
		// Create Vertex buffer object
		glGenBuffers(1, &m_iVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);	// Bind VBO to VAO
		int i=0;
		int iStride = 0, iBindOff = 0, iColorBindOff = 0;
		while(_pcVD[i])
		{
			int iLocation;
			switch(_pcVD[i])
			{
			case 'p': iLocation = int(BindingLocation::POSITION);
				glVertexAttribPointer(iLocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 3*sizeof(float);
				break;
			case 'n': iLocation = int(BindingLocation::NORMAL);
				glVertexAttribPointer(iLocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 3*sizeof(float);
				break;
			case 't': iLocation = int(BindingLocation::TANGENT);
				glVertexAttribPointer(iLocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 3*sizeof(float);
				break;
			case '1': iLocation = int(BindingLocation::VEC) + iBindOff;
				glVertexAttribPointer(iLocation, 1, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += sizeof(float);
				++iBindOff;
				break;
			case '2': iLocation = int(BindingLocation::VEC) + iBindOff;
				glVertexAttribPointer(iLocation, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 2*sizeof(float);
				++iBindOff;
				break;
			case '3': iLocation = int(BindingLocation::VEC) + iBindOff;
				glVertexAttribPointer(iLocation, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 3*sizeof(float);
				++iBindOff;
				break;
			case '4': iLocation = int(BindingLocation::VEC) + iBindOff;
				glVertexAttribPointer(iLocation, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 4*sizeof(float);
				++iBindOff;
				break;
			case 'c':
				if(iColorBindOff>=7) std::cout << "[VertexBuffer::VertexBuffer] Too many 'c' in vertex declaration. The maximum number of color binding points is 8!\n";
				iLocation = int(BindingLocation::COLOR0) + iColorBindOff;
				glVertexAttribPointer(iLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid*)(iStride));
				glEnableVertexAttribArray(iLocation);
				iStride += 4;
				++iColorBindOff;
				break;
			}	   
			++i;
		}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if( m_iMaxNumVertices * m_iVertexSize )
	{
		m_pData = (uint8_t*)malloc(m_iMaxNumVertices * m_iVertexSize);
	}
}

// ************************************************************************* //
VertexBuffer::~VertexBuffer()
{
	// Detach and delete Vertex buffer
	glBindVertexArray(m_iVAO);
	//glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_iVBO);
	// Detach and delete array
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_iVAO);
}

// ************************************************************************* //
void VertexBuffer::Add(void* _pVal)
{
	if(m_iCursor == m_iMaxNumVertices)
	{std::cout << "[VertexBuffer::Add] Vertex buffer full (capacity: "<<m_iMaxNumVertices<<"). Add has no effect.\n"; return;}

	memcpy(m_pData + m_iCursor * m_iVertexSize, _pVal, m_iVertexSize);
	++m_iCursor;
	m_bDirty = true;
}

// ************************************************************************* //
void VertexBuffer::Set(unsigned _iIndex, const void* _pValue)
{
	if( IsStatic() ) {std::cout << "[VertexBuffer::Set] Cannot set vertices in a static buffer.\n"; return;}
	assert( 0<=_iIndex && _iIndex < GetNumVertices() );
	memcpy(m_pData + _iIndex * m_iVertexSize, _pValue, m_iVertexSize);
	m_bDirty = true;
}

// ************************************************************************* //
void* VertexBuffer::Get(unsigned _iIndex)
{
	if( IsStatic() ) {std::cout << "[VertexBuffer::Get] Cannot get vertices in a static buffer.\n"; return nullptr;}
	assert( 0<=_iIndex && _iIndex < GetNumVertices() );
	// Assume write access from outside
	m_bDirty = true;
	return m_pData + _iIndex * m_iVertexSize;
}

// ******************************************************************************** //
void VertexBuffer::MakeStatic()
{
	if(m_bDirty) Commit();
	free(m_pData);
	m_pData = nullptr;
}

// ******************************************************************************** //
void VertexBuffer::Commit()
{
	if(m_pData && m_bDirty)
	{
		//glBindVertexArray(m_dwVAO);
		// Discard everything on gpu and upload new
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
		//glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, m_iCursor * m_iVertexSize, m_pData, GL_STATIC_DRAW);
		m_bDirty = false;
	}
}

// ******************************************************************************** //
void VertexBuffer::DeleteVertex(unsigned _iIndex)
{
	assert( m_iCursor > 0 );	// Number of vertices
	if(m_iCursor <= _iIndex) std::cout << "[VertexBuffer::DeleteVertex] Vertex cannot be deleted: Index too large.\n";
	--m_iCursor;
	if(_iIndex != m_iCursor) Set(_iIndex, Get(m_iCursor));
}

// ******************************************************************************** //
void VertexBuffer::FlipNormals()
{
	if(m_iNormalOffset == 0xffffffff) return;
	for(unsigned i=0; i<GetNumVertices(); ++i)
	{
		// TODO: if vector existing
	//	OrE::Math::Vec3* pN = (OrE::Math::Vec3*)(((uint8_t*)m_pData)+i*m_iVertexSize+m_iNormalOffset);
	//	(*pN) = -(*pN);
	}
}

// ******************************************************************************** //
void VertexBuffer::Bind()
{
	// Don't use a empty buffer. Call MakeStatic or Commit before.
	assert( !m_bDirty );

	glBindVertexArray(m_iVAO);
}



};