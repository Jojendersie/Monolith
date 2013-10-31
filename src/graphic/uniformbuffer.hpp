#pragma once

#include <string>
#include <unordered_map>
#include "../math/math.hpp"

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

		/// \brief A subclass required to make nice syntactic assignments with
		///		different types.
		class UniformVar {
			void* pBufferPosition;
		public:
			/// \brief Create the variable with access to a certain buffer position.
			UniformVar( void* _p ) : pBufferPosition(_p)	{}

			/// \brief Assign a matrix to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Matrix& _m)	{ *(Math::Matrix*)pBufferPosition = transpose(_m); }

			/// \brief Assign a 4D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec4& _v)	{ *(Math::Vec4*)pBufferPosition = _v; }

			/// \brief Assign a 3D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec3& _v)	{ *(Math::Vec3*)pBufferPosition = _v; }

			/// \brief Assign a 2D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec2& _v)	{ *(Math::Vec2*)pBufferPosition = _v; }

			/// \brief Assign a float value to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const float _f)	{ *(float*)pBufferPosition = _f; }

			/// \brief Cast to matrix without type check.
			operator Math::Matrix () const { return transpose(*(Math::Matrix*)pBufferPosition); }
			/// \brief Cast to vector 4 without type check.
			operator const Math::Vec4& () const { return *(Math::Vec4*)pBufferPosition; }
			/// \brief Cast to vector 3 without type check.
			operator const Math::Vec3& () const { return *(Math::Vec3*)pBufferPosition; }
			/// \brief Cast to vector 2 without type check.
			operator const Math::Vec2& () const { return *(Math::Vec2*)pBufferPosition; }
			/// \brief Cast to float without type check.
			operator float () const				{ return *(float*)pBufferPosition; }
		};

		/// \brief Read and write access to variables
		/// \param [in] _sName Name for the new attribute. Using the same name
		///		twice is not allowed.
		UniformVar operator [] ( const std::string& _sName );

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