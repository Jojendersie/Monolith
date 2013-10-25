#pragma once

#include "../math/math.hpp"

namespace Graphic {

	class VertexBuffer
	{
	private:
		unsigned	m_iVBO;				///< Only one vertexbuffer is used for the whole array
		unsigned	m_iVAO;				///< The OpenGL ID for the whole mesh-vertices
		unsigned	m_iMaxNumVertices;	///< Maximum size set at construction
		unsigned	m_iCursor;			///< Cursor for stream write methods
		int			m_iVertexSize;		///< Size of one vertex in bytes if single interleaved buffer, size of all components in an not interleaved buffer.
		uint8_t*	m_pData;			///< A CPU copy of the data or nullptr for static buffers
		bool		m_bDirty;

		unsigned	m_iPositionOffset;	///< Byte offset inside a vertex to the position vector
		unsigned	m_iNormalOffset;	///< Byte offset inside a vertex to the normal vector
		unsigned	m_iTangentOffset;	///< Byte offset inside a vertex to the tangent vector

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
			COLOR4			= 7,
			COLOR5			= 8,
			COLOR6			= 9,
			COLOR7			= 10,
			VEC				= 11
		};

		/// \brief Creating a vertex buffer with arbitary interleaved data
		/// \param [in] _iMaxNumVertices Capacity/total number of usable vertices
		/// \param [in] _pcVD Vertex definition/declaration specifies the
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
		VertexBuffer(unsigned _iMaxNumVertices, const char* _pcVD);
		~VertexBuffer();

		/// \brief Add one vertex at the end of the buffer.
		/// \details This copies as much bytes as specified through _pcVD in
		///		constructor.
		void Add(void* _pVal);

		// \brief Overrides a index in the memory copy. Does nothing if static.
		/// \param [in] _iIndex Index of the vertex which should be in
		///		[0,GetNumVertices()[.
		/// \param [in] _pValue Pointer to a vertex structure.
		void Set(unsigned _iIndex, const void* _pValue);

		/// \brief Reads from memory copy. Does nothing if static.
		void* Get(unsigned _iIndex);

		/// \brief Uploads data and removes the memory copy for read and write
		///		operations.
		void MakeStatic();
		
		/// \brief Upload complete buffer objects to GPU.
		void Commit();

		/// \brief Moving the last vertex to the specified index and overwrites the
		///		one there. Then removing the last vertex.
		void DeleteVertex(unsigned _iIndex);

		/// \brief Invert the sign of all normal vectors.
		/// \details If there are no normals in vertexformat it does nothing.
		void FlipNormals();

		/// \brief Bind the buffer for the draw call
		void Bind();

		unsigned GetNumVertices()		{return m_iCursor;}
		int GetVertexSize()				{return m_iVertexSize;}
		void SetDirty()					{m_bDirty = true;}
		bool IsStatic()					{return m_pData==nullptr;}
		bool IsDirty()					{return m_bDirty;}

		unsigned GetPositionOffset()	{ return m_iPositionOffset; }
		unsigned GetNormalOffset()		{ return m_iNormalOffset; }
		unsigned GetTangentOffset()		{ return m_iTangentOffset; }
		
		const Math::Vec3& GetPosition(unsigned _iIndex)		{ return *(Math::Vec3*)(m_pData + _iIndex*m_iVertexSize + m_iPositionOffset); }
		const Math::Vec3& GetNormal(unsigned _iIndex)		{ return *(Math::Vec3*)(m_pData + _iIndex*m_iVertexSize + m_iNormalOffset); }
		const Math::Vec3& GetTangent(unsigned _iIndex)		{ return *(Math::Vec3*)(m_pData + _iIndex*m_iVertexSize + m_iTangentOffset); }
	};
	typedef VertexBuffer* VertexBufferP;

};