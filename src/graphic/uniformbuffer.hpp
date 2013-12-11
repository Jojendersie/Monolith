#pragma once

#include <string>
#include <unordered_map>
#include "../math/math.hpp"

namespace Graphic {

	/// \brief A wrapper for uniform buffer objects.
	/// \details This class maps string-keys to locations to allow array access.
	///
	///		Each key is aligned according to the order of addition. Make sure
	///		that the order aligns well with your ubo in shader.
	class UniformBuffer
	{
	public:
		/// \brief Create a uniform buffer object.
		/// \param [in] _name Name of the buffer in the shader file for linking.
		UniformBuffer( const std::string& _name );

		~UniformBuffer();

		enum struct ATTRIBUTE_TYPE {
			FLOAT = 4,
			VEC3 = 12,
			VEC4 = 16,
			MATRIX = 64
		};

		/// \brief Add a new attribute at the end of the uniform buffer.
		void AddAttribute( const std::string& _name, ATTRIBUTE_TYPE _type );

		/// \brief A subclass required to make nice syntactic assignments with
		///		different types.
		class UniformVar {
			void* bufferPosition;
			UniformBuffer* buffer;	///<< Parent access to mark things as dirty
		public:
			/// \brief Create the variable with access to a certain buffer position.
			UniformVar( void* _p, UniformBuffer* _buffer ) : bufferPosition(_p), buffer(_buffer)	{}

			/// \brief Assign a matrix to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Mat4x4& _m)	{ *(Math::Mat4x4*)bufferPosition = _m; buffer->m_isDirty = true; }

			/// \brief Assign a 4D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec4& _v)	{ *(Math::Vec4*)bufferPosition = _v; buffer->m_isDirty = true; }

			/// \brief Assign a 3D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec3& _v)	{ *(Math::Vec3*)bufferPosition = _v; buffer->m_isDirty = true; }

			/// \brief Assign a 2D vector to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const Math::Vec2& _v)	{ *(Math::Vec2*)bufferPosition = _v; buffer->m_isDirty = true; }

			/// \brief Assign a float value to the uniform location.
			/// \details This will not check for type validity!
			void operator=(const float _f)			{ *(float*)bufferPosition = _f; buffer->m_isDirty = true; }

			/// \brief Cast to matrix without type check.
			operator Math::Mat4x4 () const { return *(Math::Mat4x4*)bufferPosition; }
			/// \brief Cast to vector 4 without type check.
			operator const Math::Vec4& () const { return *(Math::Vec4*)bufferPosition; }
			/// \brief Cast to vector 3 without type check.
			operator const Math::Vec3& () const { return *(Math::Vec3*)bufferPosition; }
			/// \brief Cast to vector 2 without type check.
			operator const Math::Vec2& () const { return *(Math::Vec2*)bufferPosition; }
			/// \brief Cast to float without type check.
			operator float () const				{ return *(float*)bufferPosition; }
		};

		/// \brief Read and write access to variables
		/// \param [in] _name Name for the new attribute. Using the same name
		///		twice is not allowed.
		UniformVar operator [] ( const std::string& _name );

		/// \brief Read access to variables.
		/// \param [in] _name Name for the new attribute. Using the same name
		///		twice is not allowed.
		const UniformVar operator [] ( const std::string& _name ) const;

		/// \brief Load the latest changes up to the GPU.
		/// \details Due to driver issues this should be called at most once
		///		before each draw call.
		void Commit();

		/// \brief Get the name of the buffer object which should be the same
		///		name as used in the shaders.
		const std::string& GetName() const	{ return m_name; }

		/// \brief Get the OpenGL buffer id.
		unsigned GetBufferID() const		{ return m_bufferID; }
		/// \brief Get the location where the buffer is bound in OpenGL.
		unsigned GetBufferBaseIndex() const	{ return m_index; }

	private:
		std::string m_name;		///< Name of the buffer inside shaders
		unsigned m_index;		///< glBindBufferBase Index
		unsigned m_bufferID;	///< Address of uniform buffer object
		void* m_memory;			///< RAM double buffer: this is edited and uploaded if necessary
		unsigned m_size;		///< Number of used bytes in m_memory

		bool m_isDirty;			///< Did something change since last commit?

		std::unordered_map<std::string, int> m_attributes;
	};
};