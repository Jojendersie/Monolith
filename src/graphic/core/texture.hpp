#pragma once

#include <string>
#include <vector>

namespace Graphic {

	/// \brief Wrapper for one OpenGL texture.
	class Texture {
	public:
		/// \brief Load a single 2D,3D or cubemap texture from file.
		// Currently supported: 2D TODO: the others
		Texture( const std::string& _fileName );

		/// \brief Load an 2D texture array. The texture index in the
		///		array is derived from its index in the vector.
		/// \param [in] _fileNames List of image files to be loaded into
		///		the array.
		Texture( const std::vector<std::string>& _fileNames );

		/// \brief Release resource
		~Texture();

		/// \brief Return width of the loaded texture.
		int Width()			{ return m_width; }

		/// \brief Return width of the loaded texture.
		int Height()		{ return m_height; }

		/// \brief Return depth/layers of a volume texture or texture array.
		/// \details For standard 2D textures the return value is 1.
		int Depth()			{ return m_depth; }
	private:
		unsigned m_textureID;		///< OpenGL texture handle
		unsigned m_bindingPoint;	///< The constructor determines GL_TEXTURE_2D_ARRAY,... 

		int m_width;				///< Just informative: width of the texture
		int m_height;				///< Just informative: height of the texture
		int m_depth;				///< Just informative: depth of the texture (volume), or number of slices in an array or 1
		friend class Device;
	};

}; // namespace Graphic