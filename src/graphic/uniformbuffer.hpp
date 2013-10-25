#pragma once

#include <string>
#include <unordered_map>

namespace Graphic {

	/// \brief A wrapper for uniform buffer objects.
	/// \details This class maps string-keys to locations to allow array access.
	///
	///		Each key is alligned according to the order of addition. Make sure
	///		that the order aligns well with your ubo in shader.
	class UniformBuffer
	{
	public:
		/// \brief Create a uniform buffer object.
		/// \param [in] _sName Name of the buffer in the shader file for linking.
		UniformBuffer( const std::string& _sName );

		~UniformBuffer();

		enum struct ATTRIBUTE_TYPE {
			FLOAT = 4,
			VEC3 = 12,
			VEC4 = 16,
			MATRIX = 64
		};

		/// \brief Add a new attribute at the end of the uniform buffer.
		void AddAttribute( const std::string& _sName, ATTRIBUTE_TYPE _Type );

		/// \brief Read and write access to variables
		/// \param [in] _sName Name for the new attribute. Using the same name
		///		twice is not allowed.
		void* operator [] ( const std::string& _sName );

		/// \brief Each buffer must be bound to a program once to be used.
		/// \param [in] _iProgramID OpenGL id of a valid linked shader program
		void BindToShader( unsigned _iProgramID );

		/// \brief Load the latest changes up to the GPU.
		/// \details Due to driver issues this should be called at most once
		///		before each draw call.
		void Commit();

	private:
		std::string m_sName;	///< Name of the buffer inside shaders
		unsigned m_iIndex;		///< glBindBufferBase Index
		unsigned m_iBufferID;	///< Address of uniform buffer object
		void* m_pMemory;		///< RAM double buffer: this is edited and uploaded if necessary
		unsigned m_iSize;		///< Number of used bytes in m_pMemory

		std::unordered_map<std::string, int> m_Attributes;
	};
};