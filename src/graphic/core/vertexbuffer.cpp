#include "vertexbuffer.hpp"
#include <ei/vector.hpp>
#include "opengl.hpp"
#include <cstdlib>
#include <cstring>
#include "utilities/assert.hpp"
#include <cstdint>

namespace Graphic {
// ******************************************************************************** //
// DATA BUFFER
// ******************************************************************************** //
static const int TYPE_SIZES[] = {sizeof(float), sizeof(float) * 2, sizeof(float) * 3, sizeof(float) * 4, sizeof(uint32), sizeof(uint8) * 4};

DataBuffer::DataBuffer(std::initializer_list<VertexAttribute> _interleavedData, bool _instanceData) :
	m_divisor(_instanceData ? 1 : 0),
	m_cursor(0),
	m_numElements(128),
	m_numElementsGPU(0),
	m_isStatic(false),
	m_isDiry(false)
{
	m_elemSize = 0;
	for(auto a : _interleavedData)
	{
		m_binding.push_back( a.bindingLocation );
		m_types.push_back( a.type );
		m_elemSize += TYPE_SIZES[(int)a.type];
	}
	m_data = (uint8*)malloc(m_numElements * m_elemSize);

	GL_CALL(glGenBuffers, 1, &m_VBO);
}

DataBuffer::~DataBuffer()
{
	if(m_VBO)
	{
		GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
		GL_CALL(glDeleteBuffers, 1, &m_VBO);
	}
	free(m_data);
}

void DataBuffer::Clear()
{
	Assert(!IsStatic(), "Static vertex buffers can not be cleared!");
	m_cursor = 0;
}

void DataBuffer::SetData(void*& _data, int _size)
{
	Assert(_data, "No data to commit!");
	Assert(_size >= m_elemSize, "Empty data should not be committed!");
	Assert(_size % m_elemSize == 0, "Size is not a multiple of the vertex size!");

	// Already locked by the guard???
	//std::lock_guard<std::mutex> lock(m_dataLock);

	// Remove maybe old content.
	m_isStatic = true;
	free(m_data);
	m_data = nullptr;

	// Take data for later commit
	m_data = (uint8*)_data;
	_data = nullptr;

	// Derive the statistic data
	m_numElements = _size / m_elemSize;
	m_cursor = m_numElements;
	m_isDiry = true;
}


// ******************************************************************************** //
// VERTEX ARRAY BUFFER
// ******************************************************************************** //
VertexArrayBuffer::VertexArrayBuffer(PrimitiveType _primitiveType) :
	m_primitiveType(_primitiveType),
	m_numVertices(0),
	m_numInstances(1),
	m_isInstanced(false)
{
	// Create the vertex declaration
	GL_CALL(glGenVertexArrays, 1, &m_VAO);
	GL_CALL(glBindVertexArray, m_VAO);
}

VertexArrayBuffer::VertexArrayBuffer(PrimitiveType _primitiveType, std::initializer_list<std::shared_ptr<DataBuffer>> _buffers) :
	m_primitiveType(_primitiveType),
	m_numVertices(0),
	m_numInstances(1),
	m_isInstanced(false)
{
	// Create the vertex declaration
	GL_CALL(glGenVertexArrays, 1, &m_VAO);
	GL_CALL(glBindVertexArray, m_VAO);

	for(auto& b : _buffers)
		AttachBuffer(std::move(b));
}

VertexArrayBuffer::VertexArrayBuffer(PrimitiveType _primitiveType, std::initializer_list<VertexAttribute> _vertexDeclaration, bool _interleaved) :
	m_primitiveType(_primitiveType),
	m_numVertices(0),
	m_numInstances(1),
	m_isInstanced(false)
{
	// Create the vertex declaration and buffers together
	GL_CALL(glGenVertexArrays, 1, &m_VAO);
	GL_CALL(glBindVertexArray, m_VAO);
	if(_interleaved)
	{
		// Create a single new buffer
		AttachBuffer(std::make_shared<DataBuffer>(_vertexDeclaration, false));
	} else {
		// Create a new buffer per attribute
		for(auto decl : _vertexDeclaration)
		{
			AttachBuffer(std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({decl}), false));
		}
	}
}

VertexArrayBuffer::VertexArrayBuffer(VertexArrayBuffer&& _other) :
	m_buffers(std::move(_other.m_buffers)),
	m_primitiveType(_other.m_primitiveType),
	m_VAO(_other.m_VAO),
	m_numVertices(_other.m_numVertices),
	m_numInstances(_other.m_numInstances),
	m_isInstanced(_other.m_isInstanced)
{
	_other.m_VAO = 0;
}

void VertexArrayBuffer::AttachBuffer(std::shared_ptr<DataBuffer> _b)
{
	// Take ownership of data
	m_buffers.push_back(_b);

	int offset = 0;
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, _b->m_VBO);
	for(int i = 0; i < _b->m_types.size(); ++i)
	{
		GL_CALL(glEnableVertexAttribArray, _b->m_binding[i]);
		switch(_b->m_types[i])
		{
		case VertexAttribute::Type::FLOAT:
			GL_CALL(glVertexAttribPointer, _b->m_binding[i], 1, GL_FLOAT, GL_FALSE, _b->m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC2:
			GL_CALL(glVertexAttribPointer, _b->m_binding[i], 2, GL_FLOAT, GL_FALSE, _b->m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC3:
			GL_CALL(glVertexAttribPointer, _b->m_binding[i], 3, GL_FLOAT, GL_FALSE, _b->m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC4:
			GL_CALL(glVertexAttribPointer, _b->m_binding[i], 4, GL_FLOAT, GL_FALSE, _b->m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::UINT:
			GL_CALL(glVertexAttribIPointer, _b->m_binding[i], 1, GL_UNSIGNED_INT, _b->m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::COLOR:
			GL_CALL(glVertexAttribPointer, _b->m_binding[i], 4, GL_UNSIGNED_BYTE, GL_TRUE, _b->m_elemSize, (GLvoid*)(offset));
			break;
		}
		offset += TYPE_SIZES[ (int)_b->m_types[i] ];
		GL_CALL(glVertexAttribDivisor, _b->m_binding[i], _b->m_divisor);
	}

	if(_b->IsInstanceData()) {
		m_isInstanced = true;
		m_numInstances = 0;
	}
}


// ******************************************************************************** //
VertexArrayBuffer::~VertexArrayBuffer()
{
	if( m_VAO )
	{
		GL_CALL(glBindVertexArray, 0);
		GL_CALL(glDeleteVertexArrays, 1, &m_VAO);
	}
}

// ******************************************************************************** //
void VertexArrayBuffer::Clear()
{
	for(auto& b : m_buffers)
		b->Clear();
}

// ******************************************************************************** //
void VertexArrayBuffer::Bind() const
{
	// In case there is nothing to upload commit will do nothing
	const_cast<VertexArrayBuffer*>(this)->Commit();

	GL_CALL(glBindVertexArray, m_VAO);
}

// ******************************************************************************** //
DataBufferGuard&& VertexArrayBuffer::GetBuffer(int _index, DataBufferGuard&& _destGuard)
{
	_destGuard = std::move( DataBufferGuard(this, m_buffers[_index].get()) );
	return std::move(_destGuard);
}

// ******************************************************************************** //
void VertexArrayBuffer::Commit()
{
	for(auto& b : m_buffers)
	{
		// Do not commit while data is edited
		if(b->IsDirty() && b->m_dataLock.try_lock())
		{
			try {
				// Discard everything in a range where vertices are dirty
				GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, b->m_VBO);
				// Required GPU allocation or only ranged update?
				if( b->m_numElementsGPU < b->m_cursor ) {
					GL_CALL(glBufferData, GL_ARRAY_BUFFER, b->m_elemSize * b->m_numElements, b->m_data, b->IsStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
					b->m_numElementsGPU = b->m_cursor;
				} else
					GL_CALL(glBufferSubData, GL_ARRAY_BUFFER, 0, b->m_cursor * b->m_elemSize, b->m_data );//TODO: offset?
				if( b->IsStatic() )
				{
					// Remove CPU memory copy
					free(b->m_data);
					b->m_data = nullptr;
				}
				GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);

				b->m_isDiry = false;
			} catch(...) {
				LOG_ERROR("Something during VertexArrayBuffer::Commit caused an exception. Going to ignore that.");
			}
			b->m_dataLock.unlock();
		}
	}
}


// ************************************************************************* //
// VERTEX BUFFER GUARD
// ************************************************************************* //
DataBufferGuard::DataBufferGuard(VertexArrayBuffer* _bufferArray, DataBuffer* _buffer) :
	m_bufferArray(_bufferArray),
	m_buffer(_buffer)
{
	m_buffer->m_dataLock.lock();
}

DataBufferGuard::DataBufferGuard(DataBufferGuard&& _other) :
	m_buffer(_other.m_buffer),
	m_bufferArray(_other.m_bufferArray)
{
	_other.m_buffer = nullptr;
	_other.m_bufferArray = nullptr;
}

DataBufferGuard::~DataBufferGuard()
{
	if(m_buffer)
	{
		// Update current number of vertices. We simply hope, that all buffers have
		// the same size, but since Guard destructions are sequentially temporal
		// inconsistencies are possible.
		if(m_buffer->IsInstanceData())
			m_bufferArray->m_numInstances = m_buffer->m_cursor;
		else m_bufferArray->m_numVertices = m_buffer->m_cursor;
		m_buffer->Touch();
		m_buffer->m_dataLock.unlock();
	}
}

void DataBufferGuard::operator = (DataBufferGuard&& _other)
{
	m_buffer = _other.m_buffer;
	m_bufferArray = _other.m_bufferArray;
	_other.m_buffer = nullptr;
	_other.m_bufferArray = nullptr;
}


};