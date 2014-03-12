#pragma once

#include "../math/math.hpp"

namespace Graphic {

	class VertexBuffer
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
	private:
		unsigned	m_VBO;				///< Only one vertex buffer is used for the whole array
		unsigned	m_VAO;				///< The OpenGL ID for the whole mesh-vertices
		unsigned	m_maxNumVertices;	///< Maximum size set at construction
		unsigned	m_cursor;			///< Cursor for stream write methods
		int			m_vertexSize;		///< Size of one vertex in bytes if single interleaved buffer, size of all components in an not interleaved buffer.
		uint8_t*	m_data;				///< A CPU copy of the data or nullptr for static buffers
		int			m_firstDirtyIndex;	///< If there is a non committed change which is the first updated vertex?
		int			m_lastDirtyIndex;	///< If there is a non committed change which is the last updated vertex? A negative value (-1) determines a clean buffer.
		PrimitiveType m_primitiveType;	///< What form of geometry is stored?

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

		// Prevent copy constructor and operator = being generated.
		VertexBuffer(const VertexBuffer&);
		const VertexBuffer& operator = (const VertexBuffer&);

		friend class Mesh;
	public:
		/// \brief Shader binding locations of vertex attributes.
		enum struct BindingLocation
		{
			POSITION		= 0,
			NORMAL			= 1,
			TANGENT			= 2,
			COLOR0			= 3,
			COLOR1			= 4,
			COLOR2			= 5,
			COLOR3			= 6,
			UINT0			= 7,
			UINT1			= 8,
			UINT2			= 9,
			UINT3			= 10,
			VEC				= 11
		};

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
		
		/// \brief Upload changed part of a buffer to GPU.
		/// \details If the buffer was created static this is not necessary.
		void Commit();

		/// \brief Upload static vertex buffer direct.
		/// \details In case of dynamic buffers the operation is invalid.
		void Commit(void* _data, int _size);

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
		//void SetDirty()							{ m_dirty = true;}
		bool IsStatic() const					{ return m_data==nullptr; }
		bool IsDirty() const					{ return m_lastDirtyIndex != -1; }

		unsigned GetPositionOffset() const		{ return m_positionOffset; }
		unsigned GetNormalOffset() const		{ return m_normalOffset; }
		unsigned GetTangentOffset() const		{ return m_tangentOffset; }
		
		const Math::Vec3& GetPosition(unsigned _index) const	{ return *(Math::Vec3*)(m_data + _index*m_vertexSize + m_positionOffset); }
		const Math::Vec3& GetNormal(unsigned _index) const		{ return *(Math::Vec3*)(m_data + _index*m_vertexSize + m_normalOffset); }
		const Math::Vec3& GetTangent(unsigned _index) const		{ return *(Math::Vec3*)(m_data + _index*m_vertexSize + m_tangentOffset); }
	};
	typedef VertexBuffer* VertexBufferP;


	// ************************************************************************* //
	template<typename T>
	void VertexBuffer::Add(const T& _value)
	{
		if( IsStatic() ) {LOG_ERROR("Cannot add vertices to a static buffer."); return;}
		if( m_cursor == m_maxNumVertices )
		{
			// Grow on CPU side
			Resize( m_maxNumVertices * 2 );
		}

		// At least the vertex at the cursor is dirty now
		m_firstDirtyIndex = Math::min((int)m_cursor, m_firstDirtyIndex);
		m_lastDirtyIndex = Math::max((int)m_cursor, m_lastDirtyIndex);

		memcpy(m_data + m_cursor * m_vertexSize, &_value, m_vertexSize);
		++m_cursor;
	}

	// ************************************************************************* //
	template<typename T>
	void VertexBuffer::Set(unsigned _index, const T& _value)
	{
		if( IsStatic() ) {LOG_ERROR("Cannot set vertices in a static buffer."); return;}
		assert( 0<=_index && _index < GetNumVertices() );
		memcpy(m_data + _index * m_vertexSize, _value, m_vertexSize);

		m_firstDirtyIndex = Math::min(_index, m_firstDirtyIndex);
		m_lastDirtyIndex = Math::max(_index, m_lastDirtyIndex);
	}

	// ************************************************************************* //
	template<typename T>
	const T* VertexBuffer::Get(unsigned _index) const
	{
		if( IsStatic() ) {LOG_ERROR("Cannot get vertices in a static buffer."); return nullptr;}
		assert( 0<=_index && _index < GetNumVertices() );
		return (T*)(m_data + _index * m_vertexSize);
	}

};