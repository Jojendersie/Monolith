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
DataBuffer::DataBuffer(VertexAttribute _attribute, bool _instanceData) :
	m_divisor(_instanceData ? 1 : 0),
	m_cursor(0),
	m_numElements(128),
	m_numElementsGPU(0),
	m_isStatic(false),
	m_isDiry(false)
{
	m_binding.push_back( _attribute.bindingLocation );
	m_types.push_back( _attribute.type );
	m_elemSize = TYPE_SIZES[(int)_attribute.type];
	m_data = (uint8*)malloc(m_numElements * m_elemSize);

	GL_CALL(glGenBuffers, 1, &m_VBO);
}

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

	//StartWorking();

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
VertexArrayBuffer::VertexArrayBuffer(PrimitiveType _primitiveType, std::initializer_list<std::shared_ptr<DataBuffer>> _buffers) :
	m_primitiveType(_primitiveType),
	m_numVertices(0),
	m_numInstances(1),
	m_isInstanced(false)
{
	// Take ownership of data
	for(auto& b : _buffers)
		m_buffers.push_back(std::move(b));

	// Create the vertex declaration
	GL_CALL(glGenVertexArrays, 1, &m_VAO);
	GL_CALL(glBindVertexArray, m_VAO);
	for(auto b : m_buffers)
	{
		AttachBuffer(*b);
		if(b->IsInstanceData()) {
			m_isInstanced = true;
			m_numInstances = 0;
		}
	}
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
		m_buffers.push_back(std::make_shared<DataBuffer>(_vertexDeclaration, false));
		AttachBuffer(*m_buffers.back());
	} else {
		// Create a new buffer per attribute
		for(auto decl : _vertexDeclaration)
		{
			m_buffers.push_back(std::make_shared<DataBuffer>(decl, false));
			AttachBuffer(*m_buffers.back());
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

void VertexArrayBuffer::AttachBuffer(DataBuffer& _b)
{
	int offset = 0;
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, _b.m_VBO);
	for(int i = 0; i < _b.m_types.size(); ++i)
	{
		GL_CALL(glEnableVertexAttribArray, _b.m_binding[i]);
		switch(_b.m_types[i])
		{
		case VertexAttribute::Type::FLOAT:
			GL_CALL(glVertexAttribPointer, _b.m_binding[i], 1, GL_FLOAT, GL_FALSE, _b.m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC2:
			GL_CALL(glVertexAttribPointer, _b.m_binding[i], 2, GL_FLOAT, GL_FALSE, _b.m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC3:
			GL_CALL(glVertexAttribPointer, _b.m_binding[i], 3, GL_FLOAT, GL_FALSE, _b.m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::VEC4:
			GL_CALL(glVertexAttribPointer, _b.m_binding[i], 4, GL_FLOAT, GL_FALSE, _b.m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::UINT:
			GL_CALL(glVertexAttribIPointer, _b.m_binding[i], 1, GL_UNSIGNED_INT, _b.m_elemSize, (GLvoid*)(offset));
			break;
		case VertexAttribute::Type::COLOR:
			GL_CALL(glVertexAttribPointer, _b.m_binding[i], 4, GL_UNSIGNED_BYTE, GL_TRUE, _b.m_elemSize, (GLvoid*)(offset));
			break;
		}
		offset += TYPE_SIZES[ (int)_b.m_types[i] ];
		GL_CALL(glVertexAttribDivisor, _b.m_binding[i], _b.m_divisor);
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
/*void VertexArrayBuffer::BeginWorking()
{
	if(m_state == State::UPLOAD_READY)
	{
		// Wait for a current running upload.
		std::unique_lock<std::mutex> lock(m_uploading);
		m_state = State::WORKING;
	}
	// Until the first SetDirty or Commit(2) call INITIALIZATION is not left
	Assert(m_state == State::WORKING || m_state == State::INITIALIZING,
		"Thread state not clear!");
}*/

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

// ************************************************************************* //
// VERTEX BUFFER
// ************************************************************************* //
// Helper function to work with the format specification
/*inline int VectorSize(char a)
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

// Creating a vertex buffer with arbitrary interleaved data
void VertexBuffer::CreateVAO(const char* _vertexDeclaration)
{
	// First step analyzing the string
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
	GL_CALL(glGenVertexArrays, 1, &m_VAO);
	GL_CALL(glBindVertexArray, m_VAO);
		// Create Vertex buffer object
		GL_CALL(glGenBuffers, 1, &m_VBO);
		GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_VBO);	// Bind VBO to VAO
		int i=0;
		int stride = 0, bindOff = 0, colorBindOff = 0, uintBindOff = 0;
		while(_vertexDeclaration[i])
		{
			int location;
			switch(_vertexDeclaration[i])
			{
			case 'p': location = int(BindingLocation::POSITION);
				GL_CALL(glVertexAttribPointer, location, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 3*sizeof(float);
				break;
			case 'n': location = int(BindingLocation::NORMAL);
				GL_CALL(glVertexAttribPointer, location, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 3*sizeof(float);
				break;
			case 't': location = int(BindingLocation::TANGENT);
				GL_CALL(glVertexAttribPointer, location, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 3*sizeof(float);
				break;
			case '1': location = int(BindingLocation::VEC) + bindOff;
				GL_CALL(glVertexAttribPointer, location, 1, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += sizeof(float);
				++bindOff;
				break;
			case '2': location = int(BindingLocation::VEC) + bindOff;
				GL_CALL(glVertexAttribPointer, location, 2, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 2*sizeof(float);
				++bindOff;
				break;
			case '3': location = int(BindingLocation::VEC) + bindOff;
				GL_CALL(glVertexAttribPointer, location, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 3*sizeof(float);
				++bindOff;
				break;
			case '4': location = int(BindingLocation::VEC) + bindOff;
				GL_CALL(glVertexAttribPointer, location, 4, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 4*sizeof(float);
				++bindOff;
				break;
			case 'c':
				if(colorBindOff>=3) LOG_ERROR("Too many 'c' in vertex declaration. The maximum number of color binding points is 4!");
				location = int(BindingLocation::COLOR0) + colorBindOff;
				GL_CALL(glVertexAttribPointer, location, 4, GL_UNSIGNED_BYTE, GL_TRUE, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 4;
				++colorBindOff;
				break;
			case 'u':
				if(uintBindOff>=3) LOG_ERROR("Too many 'u' in vertex declaration. The maximum number of uint binding points is 4!");
				location = int(BindingLocation::UINT0) + uintBindOff;
				GL_CALL(glVertexAttribIPointer, location, 1, GL_UNSIGNED_INT, m_vertexSize, (GLvoid*)(stride));
				GL_CALL(glEnableVertexAttribArray, location);
				stride += 4;
				++uintBindOff;
				break;
			}
			++i;
		}

	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

// ************************************************************************* //
// Creating a static vertex buffer with arbitrary interleaved data.
VertexBuffer::VertexBuffer( const char* _vertexDeclaration, void* _data, int _size, PrimitiveType _type ) :
	m_data(nullptr),
	m_firstDirtyIndex(std::numeric_limits<int>::max()),
	m_lastDirtyIndex(-1),
	m_positionOffset(0xffffffff),
	m_normalOffset(0xffffffff),
	m_tangentOffset(0xffffffff),
	m_primitiveType(_type),
	m_isStatic(true),
	m_state(State::INITIALIZING)
{
	CreateVAO(_vertexDeclaration);

	// Upload the data
	if(_data) {
		GL_CALL(glBindBuffer,  GL_ARRAY_BUFFER, m_VBO );
		GL_CALL(glBufferData, GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
		GL_CALL(glBindBuffer,  GL_ARRAY_BUFFER, 0 );

		m_cursor = m_maxNumVertices = _size / m_vertexSize;

		// Resolve thread-waiting
		m_state = State::WORKING;
	} else m_maxNumVertices = m_cursor = 0;
}

// ************************************************************************* //
// Creating a dynamic vertex buffer with arbitrary interleaved data.
VertexBuffer::VertexBuffer( const char* _vertexDeclaration, PrimitiveType _type ) :
	m_data(nullptr),
	m_firstDirtyIndex(std::numeric_limits<int>::max()),
	m_lastDirtyIndex(-1),
	m_cursor(0),
	m_maxNumVertices(64),
	m_positionOffset(0xffffffff),
	m_normalOffset(0xffffffff),
	m_tangentOffset(0xffffffff),
	m_primitiveType(_type),
	m_isStatic(false),
	m_state(State::INITIALIZING)
{
	CreateVAO(_vertexDeclaration);

	// Create the data on GPU side
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_VBO);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, m_vertexSize * m_maxNumVertices, nullptr, GL_DYNAMIC_DRAW);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	// Data on CPU side
	m_data = (uint8*)malloc( m_vertexSize * m_maxNumVertices );
}

// ************************************************************************* //
VertexBuffer::VertexBuffer( VertexBuffer&& _buffer ) :
	m_cursor( _buffer.m_cursor ),
	m_data( _buffer.m_data ),
	m_firstDirtyIndex( _buffer.m_firstDirtyIndex ),
	m_lastDirtyIndex( _buffer.m_lastDirtyIndex ),
	m_maxNumVertices( _buffer.m_maxNumVertices ),
	m_normalOffset( _buffer.m_normalOffset ),
	m_positionOffset( _buffer.m_positionOffset ),
	m_primitiveType( _buffer.m_primitiveType ),
	m_tangentOffset( _buffer.m_tangentOffset ),
	m_VAO( _buffer.m_VAO ),
	m_VBO( _buffer.m_VBO ),
	m_vertexSize( _buffer.m_vertexSize ),
	m_isStatic( _buffer.m_isStatic )
{
	_buffer.m_VAO = 0;
	_buffer.m_VBO = 0;
	_buffer.m_data = nullptr;
	m_state.store( _buffer.m_state );
}

// ************************************************************************* //
VertexBuffer::~VertexBuffer()
{
	if( m_VAO )
	{
		// Detach and delete Vertex buffer
		GL_CALL(glBindVertexArray, m_VAO);
		//glDisableVertexAttribArray(0);
		GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
		GL_CALL(glDeleteBuffers, 1, &m_VBO);
		// Detach and delete array
		GL_CALL(glBindVertexArray, 0);
		GL_CALL(glDeleteVertexArrays, 1, &m_VAO);
	}

	free(m_data);
	m_data = nullptr;
}


// ******************************************************************************** //
void VertexBuffer::Resize(unsigned _numVertices)
{
	// Private method should never be called for static buffers!
	Assert(!IsStatic(), "Static vertex buffers cannot be resized!");

	m_maxNumVertices = _numVertices;

	// Discard on GPU side
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_VBO);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, m_vertexSize * _numVertices, nullptr, GL_DYNAMIC_DRAW);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	m_data = (uint8*)realloc(m_data, _numVertices * m_vertexSize);
}


// ******************************************************************************** //
// Set the number of vertices back to 0.
void VertexBuffer::Clear()
{
	Assert(!IsStatic(), "Static vertex buffers can not be cleared!");
	StartWorking();
	m_cursor = 0;
	m_firstDirtyIndex = std::numeric_limits<int>::max();
	m_lastDirtyIndex = -1;
}

// ******************************************************************************** //
void VertexBuffer::StartWorking()
{
	// Waiting
	if(m_state == State::UPLOAD_READY)
	{
		std::unique_lock<std::mutex> lock(m_uploading);
		m_state = State::WORKING;
	}
	// Until the first SetDirty od Commit(2) call INITIALIZATION is not left
	Assert(m_state == State::WORKING || m_state == State::INITIALIZING,
		"Thread state not clear!");
}


// ******************************************************************************** //
void VertexBuffer::Commit()
{
	if(m_data && IsDirty())
	{
		std::unique_lock<std::mutex> lock(m_uploading);
		// Discard everything in a range where vertices are dirty
		GL_CALL(glBindBuffer,  GL_ARRAY_BUFFER, m_VBO );
		if( IsStatic() )
		{
			GL_CALL(glBufferData, GL_ARRAY_BUFFER, m_vertexSize * m_maxNumVertices, m_data, GL_STATIC_DRAW);
			free(m_data);
			m_data = nullptr;
		} else {
			GL_CALL(glBufferSubData, GL_ARRAY_BUFFER,
				m_firstDirtyIndex * m_vertexSize,
				(m_lastDirtyIndex - m_firstDirtyIndex + 1) * m_vertexSize,
				m_data );
		}
		GL_CALL(glBindBuffer,  GL_ARRAY_BUFFER, 0 );

		m_firstDirtyIndex = std::numeric_limits<int>::max();
		m_lastDirtyIndex = -1;

		// Resolve thread-waiting
		m_state = State::WORKING;
	}
}

void VertexBuffer::Commit(void*& _data, int _size)
{
	Assert(_data, "No data to commit!");
	Assert(_size >= m_vertexSize, "Empty data should not be committed!");
	Assert(_size % m_vertexSize == 0, "Size is not a multiple of the vertex size!");

	StartWorking();

	// Take data for later commit
	free(m_data);
	m_data = (uint8*)_data;
	_data = nullptr;

	// Derive the statistic data
	m_cursor = m_maxNumVertices = _size / m_vertexSize;
	m_firstDirtyIndex = 0;
	m_lastDirtyIndex = m_cursor-1;

	SetDirty();
}

// ******************************************************************************** //
void VertexBuffer::DeleteVertex(unsigned _index)
{
	Assert( m_state != State::UPLOAD_READY, "Call StartWork() to make sure the buffer is in a clean state before changing things!");
	Assert( m_cursor > 0, "Cursor is already at the buffer's beginning!");	// Number of vertices
	if(m_cursor <= _index) LOG_LVL2("Vertex cannot be deleted: Index too large.");
	--m_cursor;
	if(_index != m_cursor) {
		memcpy(m_data + m_vertexSize * _index, m_data + m_vertexSize * m_cursor, m_vertexSize );
	}
}

// ******************************************************************************** //
void VertexBuffer::FlipNormals()
{
	Assert( m_state != State::UPLOAD_READY, "Call StartWork() to make sure the buffer is in a clean state before changing things!");
	if(m_normalOffset == 0xffffffff) return;
	for(unsigned i=0; i<GetNumVertices(); ++i)
	{
		ei::Vec3* pN = (ei::Vec3*)(((uint8*)m_data)+i*m_vertexSize+m_normalOffset);
		(*pN) = -(*pN);
	}
}

// ******************************************************************************** //
void VertexBuffer::Bind() const
{
	// In case there is nothing to upload commit will do nothing
	const_cast<VertexBuffer*>(this)->Commit();

	GL_CALL(glBindVertexArray, m_VAO);
}*/



};