#pragma once

#include "predeclarations.hpp"
#include "ei/elementarytypes.hpp"
#include "utilities/assert.hpp"

#include <atomic>
#include <condition_variable>
#include <vector>
#include <initializer_list>
#include <memory>

namespace Graphic {

	struct VertexAttribute 
	{
		/// \brief Data type per attribute. The DataBuffer is an array of
		///		a single type.
		enum Type
		{
			FLOAT,	// 1 x float
			VEC2,	// 2 x float
			VEC3,	// 3 x float
			VEC4,	// 4 x float
			UINT,	// 1 x uint32
			COLOR,	// 8bit RGBA
		};

		/// \brief Globally defined shader binding locations of vertex attributes.
		/*enum struct BindingLocation
		{
			POSITION		= 0,
			NORMAL			= 1,
			TANGENT			= 2,
			FREE_CHOICE		= 3, // (FREE_CHOICE + x) locations have no predetermined semantic
		};*/

		Type type;
		int bindingLocation;
	};

	class DataBuffer
	{
	public:
		/// \brief Create an vertex buffer with interleaved data (multiple attributes with different types).
		/// \details Using only one attribute creates a pure buffer without interleaving.
		DataBuffer(std::initializer_list<VertexAttribute> _interleavedData, bool _instanceData);
		
		~DataBuffer();

		bool IsStatic() const					{ return m_isStatic; }
		bool IsDirty() const					{ return m_isDiry; }
		bool IsInstanceData() const				{ return m_divisor == 1; }
		int GetNumElements() const				{ return m_cursor; }

		/// \brief Set the number of vertices back to 0.
		/// \details Only possible for dynamic buffers. The storage will not
		///		be reduced!
		void Clear();

		/// \brief Prepare upload of static vertex buffer.
		/// \details This call makes the buffer static in any case.
		///		The ownership of the data is taken until upload.
		///		The buffer is set to dirty automatically.
		///
		///		This call does not upload the data directly to allow a multi
		///		threaded filling of vertex buffers.
		/// \param [in] _data Data to upload. Ownership goes to the vertex buffer
		///		and the pointer is set to nullptr. Data must be created with
		///		malloc.
		void SetData(void*& _data, int _size);

		/// \brief Return access to the internal memory. This yields nullptr
		///		if the buffer is static.
		void* GetDirectAccess() { return m_data; }

		/// \brief Declare the buffer as dirty.
		/// \details Add(), Remove() or the direct access do not do this automatically.
		///		This has the advantage that a change can be completed by a thread
		///		before the rendering tries to recommit the buffer.
		///
		///		However, the buffer guard IS CALLING Touch(). I.e. if you get
		///		access by the VertexArrayBuffer you don't need to do anything.
		void Touch() { m_isDiry = true; }

		/// \brief Add one vertex at the end of the buffer.
		/// \details This copies as much bytes as specified through _vertexDeclaration in
		///		constructor.
		template<typename T>
		void Add(const T& _value);

		/// \brief Replace an element with the last one
		template<typename T>
		void Remove(int _index);
	private:
		uint8*		m_data;				///< A CPU copy of the data or nullptr for static buffers
		std::mutex	m_dataLock;			///< Data is under editing or gets uploaded
		std::vector<VertexAttribute::Type>	m_types;	///< Data type.
		std::vector<int>	m_binding;	///< Binding location index.
		unsigned	m_VBO;				///< Only one vertex buffer is used for the whole array
		int			m_elemSize;			///< Size of all attributes.
		int			m_divisor;			///< Vertex attrib divisor: instance (1) or vertex (0) data?
		//int			m_firstDirtyIndex;	///< If there is a non committed change which is the first updated vertex?
		int			m_cursor;			///< If there is a non committed change which is the last updated vertex?
		int			m_numElements;		///< Maximum number of vertices/instances
		int			m_numElementsGPU;	///< Buffer size on GPU side (if smaller than m_cursor a realloc on GPU side occures).
		bool		m_isStatic;			///< Set on buffer creation time. Static buffers do not keep a copy of the memory.
		bool		m_isDiry;			///< Something changed since last commit

		// Prevent copy constructor and operator = being generated.
		DataBuffer(const DataBuffer&);
		const DataBuffer& operator = (const DataBuffer&);

		friend class VertexArrayBuffer;
		friend class DataBufferGuard;
	};

	// ************************************************************************* //
	template<typename T>
	void DataBuffer::Add(const T& _value)
	{
		if( IsStatic() ) { LOG_ERROR("Cannot add vertices to a static buffer."); return; }
		if( m_elemSize != sizeof(T) ) { LOG_ERROR("Data size differs from attribute declaration. Cannot add vertex!"); return; }
		if( m_cursor == m_numElements )
		{
			// Grow on CPU side
			m_numElements *= 2;
			m_data = (uint8*)realloc(m_data, m_numElements * m_elemSize);
		}

		memcpy(m_data + m_cursor * m_elemSize, &_value, m_elemSize);
		++m_cursor;
		//m_isDiry = true;
	}

	template<typename T>
	void DataBuffer::Remove(int _index)
	{
		if( IsStatic() ) { LOG_ERROR("Cannot delete vertices from a static buffer."); return; }
		if( m_elemSize != sizeof(T) ) { LOG_ERROR("Data size differs from attribute declaration. Cannot add vertex!"); return; }
		Assert( m_cursor > 0, "No vertex to delete! The buffer is empty." );
		Assert( _index < m_cursor, "Invalid index to delete a vertex." );

		T* data = (T*)m_data;
		data[_index] = data[--m_cursor];
	}


	// ************************************************************************* //
	/// \brief A class which behaves like a DataBuffer pointer, but also handles
	///		locking and vertex counting.
	class DataBufferGuard
	{
	public:
		DataBufferGuard() : m_buffer(nullptr), m_bufferArray(nullptr) {}
		DataBufferGuard(class VertexArrayBuffer* _bufferArray, DataBuffer* _buffer);
		DataBufferGuard(DataBufferGuard&& _other);
		~DataBufferGuard();

		void operator = (DataBufferGuard&& _other);
		DataBuffer* operator -> () { return m_buffer; }
		DataBuffer& operator * () { return *m_buffer; }
	private:
		class VertexArrayBuffer* m_bufferArray;
		DataBuffer* m_buffer;

		DataBufferGuard(DataBufferGuard& _other);
		void operator = (DataBufferGuard& _other);
	};

	// ************************************************************************* //
	/// \brief An vertex array aggregates complex mesh and instance data in a single object.
	class VertexArrayBuffer
	{
	public:
		enum struct PrimitiveType
		{
			POINT	= 0x0000,				// GL_POINTS
			LINE	= 0x0001,				// GL_LINES
			TRIANGLE_FAN = 0x0006,			// GL_TRIANGLE_FAN
			TRIANGLE_STRIPE = 0x0005,		// GL_TRIANGLE_STRIP
			TRIANGLE_LIST = 0x0004,			// GL_TRIANGLES
			INDEXED
		};

		/// \brief Create a pure empty object. Buffers must be attached with AttachBuffer.
		/// \details See constructor with buffer list: it is essentially the same, only
		///		that buffers are attached successive.
		VertexArrayBuffer(PrimitiveType _primitiveType);

		/// \brief Create an vertex declaration from a list of buffer objects.
		/// \details This call allows to create instanced data as well as other
		///		more complex setups. It also allows to share the buffers between
		///		different Array objects. For example to use a reduced model version
		///		for shadowmap rendering.
		/// \param [in] _primitiveType What form of geometry will be stored? The standard
		///		case is indexed where the according index buffer will determine
		///		the primitive type. This is required for direct vertex-draw calls
		VertexArrayBuffer(PrimitiveType _primitiveType, std::initializer_list<std::shared_ptr<DataBuffer>> _buffers);

		/// \brief Create an vertex array from short descriptor list.
		/// \details It is not possible to create instanced rendering with this kind
		///		of constructor. Instead create the buffers manually and add them to this
		///		buffer.
		/// \param [in] _primitiveType What form of geometry will be stored? The standard
		///		case is indexed where the according index buffer will determine
		///		the primitive type. This is required for direct vertex-draw calls
		/// \param [in] _vertexDeclaration Vertex definition/declaration specifies the
		///		format of one vertex. The input is a list of types and binding locations.
		///		Example: {{VertexAttribute::VEC3, 0}, {VertexAttribute::VEC3, 1}} for
		///			position + normal.
		/// \param [in] _interleaved Only create a single data buffer of a combined type.
		///		Otherwise a dedicated buffer per attribute is created.
		VertexArrayBuffer(PrimitiveType _primitiveType, std::initializer_list<VertexAttribute> _vertexDeclaration, bool _interleaved = true);

		/// \brief Move construction
		VertexArrayBuffer(VertexArrayBuffer&& _other);

		~VertexArrayBuffer();

		/// \brief Clear all internal buffers
		void Clear();

		/// \brief Bind the buffer for the draw call
		void Bind() const;

		/// \brief Add a new vertex data array.
		/// \details If the binding point is already occupied it is overwritten.
		void AttachBuffer(std::shared_ptr<DataBuffer> _buffer);

		int GetNumVertices() const	{ return m_numVertices; }
		int GetNumInstances() const { return m_numInstances; }
		void SetNumVertices(int _numVertices) { m_numVertices = _numVertices; }
		void SetNumInstances(int _numInstances) { if(m_isInstanced) m_numInstances = _numInstances; }
		bool IsInstanced() const	{ return m_isInstanced; }
		PrimitiveType GetPrimitiveType() const	{ return m_primitiveType; }

		DataBufferGuard&& GetBuffer(int _index, DataBufferGuard&& _destGuard = DataBufferGuard());
	private:
		std::vector<std::shared_ptr<DataBuffer>> m_buffers;
		PrimitiveType m_primitiveType;
		unsigned	m_VAO;					///< The OpenGL ID
		int			m_numVertices;
		int			m_numInstances;
		bool		m_isInstanced;


		/// \brief Upload changed part of a buffer to GPU.
		/// \details If the buffer was created static this is not necessary.
		void Commit();

		friend class DataBufferGuard;
	};




	/*class VertexBuffer
	{
	public:

		/// \brief Creating a static vertex buffer with arbitrary interleaved data
		/// \param [in] _vertexDeclaration Vertex definition/declaration specifies the
		///		format of one vertex. Each character determines a binding
		///		location and its positions the position in the per vertex data.
		///
		///		The following characters can be used:
		///					p - 3 float position (BIND_POSITION)
		///					n - 3 float normal (BIND_NORMAL)
		///					1 - 1 float texture coordinate (BIND_VEC+x)
		///					2 - 2 float texture coordinate (BIND_VEC+x)
		///					3 - 3 float texture coordinate (BIND_VEC+x)
		///					4 - 4 float components (BIND_VEC+x)
		///					c - 4 byte color (BIND_COLOR+y)
		///					u - 1 uint32 (BIND_UINT+z)
		///	\param [in] _data The data to upload to GPU. It is possible to give
		///		a nullptr and upload data later by Commit(data, size).
		/// \param [in] _type What form of geometry will be stored? The standard
		///		case is indexed where the according index buffer will determine
		///		the primitive type. This is required for direct vertex-draw calls
		VertexBuffer( const char* _vertexDeclaration, void* _data, int _size, PrimitiveType _type = PrimitiveType::INDEXED );

		/// \brief Create a dynamic vertex buffer with arbitrary interleaved data.
		/// \details \see VertexBuffer
		VertexBuffer( const char* _vertexDeclaration, PrimitiveType _type = PrimitiveType::INDEXED );

		/// \brief RValue - Move constructor.
		VertexBuffer(VertexBuffer&&);

		~VertexBuffer();

		/// \brief Add one vertex at the end of the buffer.
		/// \details This copies as much bytes as specified through _vertexDeclaration in
		///		constructor.
		template<typename T>
		void Add(const T& _value);

		// \brief Overrides a index in the memory copy. Does nothing if static.
		/// \param [in] _index Index of the vertex which should be in
		///		[0,GetNumVertices()[.
		/// \param [in] _value A vertex structure.
		template<typename T>
		void Set(unsigned _index, const T& _value);

		/// \brief Reads from memory copy. Does nothing if static (nullptr).
		template<typename T>
		const T* Get(unsigned _index) const;

		/// \brief Set the number of vertices back to 0.
		/// \details Only possible for dynamic buffers. The storage will not
		///		be reduced!
		void Clear();

		/// \brief Lock until work can be done without conflict
		void StartWorking();
		
		/// \brief Upload static vertex buffer direct.
		/// \details In case of dynamic buffers the operation is invalid.
		/// \param [in] _data Data to upload. Ownership goes to the vertex buffer
		///		and the pointer is set to nullptr. Data must be created with
		///		malloc.
		void Commit(void*& _data, int _size);

		/// \brief Moving the last vertex to the specified index and overwrites the
		///		one there. Then removing the last vertex.
		void DeleteVertex(unsigned _index);

		/// \brief Invert the sign of all normal vectors.
		/// \details If there are no normals in vertex format it does nothing.
		void FlipNormals();

		/// \brief Bind the buffer for the draw call
		void Bind() const;

		unsigned GetNumVertices() const			{ return m_cursor; }
		int GetVertexSize() const				{ return m_vertexSize; }
		PrimitiveType GetPrimitiveType() const	{ return m_primitiveType; }
		void SetDirty()							{ if(m_lastDirtyIndex != -1) m_state = State::UPLOAD_READY;}
		bool IsStatic() const					{ return m_isStatic; }
		bool IsDirty() const					{ return m_state == State::UPLOAD_READY; }
		bool IsRenderable() const				{ return m_state != State::INITIALIZING; }

		unsigned GetPositionOffset() const		{ return m_positionOffset; }
		unsigned GetNormalOffset() const		{ return m_normalOffset; }
		unsigned GetTangentOffset() const		{ return m_tangentOffset; }
		
		const ei::Vec3& GetPosition(unsigned _index) const	{ return *(ei::Vec3*)(m_data + _index*m_vertexSize + m_positionOffset); }
		const ei::Vec3& GetNormal(unsigned _index) const	{ return *(ei::Vec3*)(m_data + _index*m_vertexSize + m_normalOffset); }
		const ei::Vec3& GetTangent(unsigned _index) const	{ return *(ei::Vec3*)(m_data + _index*m_vertexSize + m_tangentOffset); }

	private:
		uint8*	m_data;				///< A CPU copy of the data or nullptr for static buffers
		unsigned	m_VBO;				///< Only one vertex buffer is used for the whole array
		unsigned	m_VAO;				///< The OpenGL ID for the whole mesh-vertices
		unsigned	m_maxNumVertices;	///< Maximum size set at construction
		unsigned	m_cursor;			///< Cursor for stream write methods
		int			m_vertexSize;		///< Size of one vertex in bytes if single interleaved buffer, size of all components in an not interleaved buffer.
		int			m_firstDirtyIndex;	///< If there is a non committed change which is the first updated vertex?
		int			m_lastDirtyIndex;	///< If there is a non committed change which is the last updated vertex? A negative value (-1) determines a clean buffer.
		bool		m_isStatic;			///< Set on buffer creation time. Static buffers do not keep a copy of the memory
		PrimitiveType m_primitiveType;	///< What form of geometry is stored?

		/// \brief The following states control the thread safety of vertex buffers.
		/// \details The basic assumptions are:
		///		* Do not upload while changing.
		///		* The only reason for vertex buffer updates is to render their content.
		///		* A dedicated worker thread can wait, rendering not.
		///
		///		Therefore the concept is: Except for initializing state there is
		///		always a buffer on the GPU -> rendering possible. When flaged
		///		as upload-ready upload, then render. Somebody how wants to
		///		change something can do so except it is upload ready. Then the
		///		worker must wait.
		enum struct State
		{
			INITIALIZING,				///< Do not upload or render
			WORKING,					///< The memory is inconsistent - do not upload
			UPLOAD_READY,				///< The memory is consistent and can be uploaded to GPU (is set after a chain of changes), or is still uploading
		};
		std::atomic<State> m_state;
		std::mutex m_uploading;

		unsigned	m_positionOffset;	///< Byte offset inside a vertex to the position vector
		unsigned	m_normalOffset;		///< Byte offset inside a vertex to the normal vector
		unsigned	m_tangentOffset;	///< Byte offset inside a vertex to the tangent vector

		/// \brief Create the VBO and VAO (vertex declaration)
		/// \details Also sets the positionOffset, ..
		void CreateVAO(const char* _vertexDeclaration);

		/// \brief Reallocate CPU and GPU memory.
		/// \details Increasing or decreasing are both allowed. The dirtiness
		///		is not touched.
		/// \param [in] _numVertices
		void Resize(unsigned _numVertices);

		/// \brief Upload changed part of a buffer to GPU.
		/// \details If the buffer was created static this is not necessary.
		void Commit();

		// Prevent copy constructor and operator = being generated.
		VertexBuffer(const VertexBuffer&);
		const VertexBuffer& operator = (const VertexBuffer&);

		friend class Mesh;
	};
	typedef VertexBuffer* VertexBufferP;


	// ************************************************************************* //
	template<typename T>
	void VertexBuffer::Add(const T& _value)
	{
		if( IsStatic() ) {LOG_ERROR("Cannot add vertices to a static buffer."); return;}
		Assert( m_state != State::UPLOAD_READY, "Call StartWork() to make sure the buffer is in a clean state before changing things!");
		if( m_cursor == m_maxNumVertices )
		{
			// Grow on CPU side
			Resize( m_maxNumVertices * 2 );
		}

		// At least the vertex at the cursor is dirty now
		m_firstDirtyIndex = ei::min((int)m_cursor, m_firstDirtyIndex);
		m_lastDirtyIndex = ei::max((int)m_cursor, m_lastDirtyIndex);

		memcpy(m_data + m_cursor * m_vertexSize, &_value, m_vertexSize);
		++m_cursor;
	}

	// ************************************************************************* //
	template<typename T>
	void VertexBuffer::Set(unsigned _index, const T& _value)
	{
		if( IsStatic() ) {LOG_ERROR("Cannot set vertices in a static buffer."); return;}
		Assert( m_state != State::UPLOAD_READY, "Call StartWork() to make sure the buffer is in a clean state before changing things!");
		Assert( 0<=_index && _index < GetNumVertices(), "Index out of range.");
		memcpy(m_data + _index * m_vertexSize, _value, m_vertexSize);

		m_firstDirtyIndex = Math::min(_index, m_firstDirtyIndex);
		m_lastDirtyIndex = Math::max(_index, m_lastDirtyIndex);
	}

	// ************************************************************************* //
	template<typename T>
	const T* VertexBuffer::Get(unsigned _index) const
	{
		if( IsStatic() ) {LOG_ERROR("Cannot get vertices in a static buffer."); return nullptr;}
		Assert( m_state != State::UPLOAD_READY, "Call StartWork() to make sure the buffer is in a clean state before changing things!");
		Assert(0 <= _index && _index < GetNumVertices(), "Index out of range.");
		return (T*)(m_data + _index * m_vertexSize);
	}*/

};