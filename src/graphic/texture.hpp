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
	private:
		unsigned m_textureID;		///< OpenGL texture handle
		unsigned m_bindingPoint;	///< The constructor determines GL_TEXTURE_2D_ARRAY,... 
		friend class Device;
	};

}; // namespace Graphic