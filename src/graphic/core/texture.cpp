#include <cstdint>
#include <jofilelib.hpp>
#include "texture.hpp"
#include "../../opengl.hpp"
#include "../../math/math.hpp"

namespace Graphic {

// Maps number of channels to a type
static uint32_t MapToGLFormats( int _numChannels, int _bitDepth, Jo::Files::ImageWrapper::ChannelType _type )
{
	assert( _numChannels > 0 && _numChannels <= 4 );
	--_numChannels;
	static const uint32_t FLOAT_FORMATS[2][4] = {{GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F},
												 {GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F}};
	static const uint32_t INT_FORMATS[2][4] = {{GL_R8_SNORM, GL_RG8_SNORM, GL_RGB8_SNORM, GL_RGBA8_SNORM},
											   {GL_R16_SNORM, GL_RG16_SNORM, GL_RGB16_SNORM, GL_RGBA16_SNORM}};
	static const uint32_t UINT_FORMATS[2][4] = {{GL_R8, GL_RG8, GL_RGB8, GL_RGBA8},
											    {GL_R16, GL_RG16, GL_RGB16, GL_RGBA16}};
	switch( _type )
	{
	case Jo::Files::ImageWrapper::ChannelType::FLOAT:
		assert(_bitDepth == 32 || _bitDepth == 16);
		return FLOAT_FORMATS[_bitDepth/16-1][_numChannels];
	case Jo::Files::ImageWrapper::ChannelType::INT:
		assert(_bitDepth == 16 || _bitDepth == 8);
		return INT_FORMATS[_bitDepth/8-1][_numChannels];
	case Jo::Files::ImageWrapper::ChannelType::UINT:
		assert(_bitDepth == 16 || _bitDepth == 8);
		return UINT_FORMATS[_bitDepth/8-1][_numChannels];
	}

	throw std::string("Image format is not supported as texture.");
	return 0;
}

// The array dimension is numChannels
const uint32_t FORMAT[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

// The inner array dimension depends on bitDepth, the outer on the ChannelType
const uint32_t TYPE[][4] = {{GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, 0, GL_UNSIGNED_INT},
							{GL_BYTE, GL_SHORT, 0, GL_UNSIGNED_INT},
							{0, 0, 0, GL_FLOAT}};


Texture::Texture( const std::string& _fileName )
{
	glGenTextures( 1, &m_textureID );
	LogGlError("Could not create a texture");

	m_bindingPoint = GL_TEXTURE_2D;	// TODO swtich
	glBindTexture( GL_TEXTURE_2D, m_textureID );
	LogGlError("Could not bind a texture");

	Jo::Files::HDDFile file(_fileName);
	Jo::Files::ImageWrapper image(file, Jo::Files::Format::PNG );

	m_width = image.Width();
	m_height = image.Height();
	m_depth = 1;	// TODO: volume texture support

	uint32_t internalFormat = MapToGLFormats(image.NumChannels(), image.BitDepth(), image.GetChannelType());
	int width = image.Width() * 2;
	int height = image.Height() * 2;
	int level = 0;
	do {
        width = Math::max(1, (width / 2));
        height = Math::max(1, (height / 2));
        glTexImage2D(m_bindingPoint, level, internalFormat, width, height,
			0, FORMAT[image.NumChannels()-1],
			TYPE[int(image.GetChannelType())][image.BitDepth()/8-1],
			level==0? image.GetBuffer() : nullptr);
		++level;
    } while( width * height > 1 );

	LogGlError("Allocating memory for a texture failed");

	// Always set some texture parameters (required for some drivers)
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// Use nearest for more voxel feeling
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glGenerateMipmap( m_bindingPoint );

	LogGlError("Something in texture upload and setup failed");

	glBindTexture( GL_TEXTURE_2D, 0 );
}


Texture::Texture( const std::vector<std::string>& _fileNames )
{
	glGenTextures( 1, &m_textureID );
	LogGlError("Could not create a texture");
	m_bindingPoint = GL_TEXTURE_2D_ARRAY;
	glBindTexture( GL_TEXTURE_2D_ARRAY, m_textureID );
	LogGlError("Could not bind a texture");

	// Use first file as reference file.
	Jo::Files::HDDFile firstFile(_fileNames[0]);
	Jo::Files::ImageWrapper firstImage(firstFile, Jo::Files::Format::PNG );

	m_width = firstImage.Width();
	m_height = firstImage.Height();
	m_depth = (int)_fileNames.size();
	
	// Expect all textures to be ok. Some of the layers might be empty later.
	uint32_t internalFormat = MapToGLFormats(firstImage.NumChannels(), firstImage.BitDepth(), firstImage.GetChannelType());
	uint32_t type = TYPE[int(firstImage.GetChannelType())][firstImage.BitDepth()/8-1];
	uint32_t format = FORMAT[firstImage.NumChannels()-1];
	int width = firstImage.Width() * 2;
	int height = firstImage.Height() * 2;
	int level = 0;
	do {
        width = Math::max(1, (width / 2));
        height = Math::max(1, (height / 2));
        glTexImage3D(m_bindingPoint, level, internalFormat, width, height, _fileNames.size(),
			0, format, type, nullptr);
		++level;
    } while( width * height > 1 );
	LogGlError("Allocating memory for a texture failed");

	// Upload pixel data of first image.
	glTexSubImage3D( m_bindingPoint, 0, 0, 0, 0, firstImage.Width(), firstImage.Height(), 1,
		format, type, firstImage.GetBuffer() );
	LogGlError("glTexSubImage3D failed");

	// Load all images from file.
	for( size_t i=1; i<_fileNames.size(); ++i )
	{
		try {
			Jo::Files::HDDFile file(_fileNames[i]);
			Jo::Files::ImageWrapper image(file, Jo::Files::Format::PNG );
			assert(image.Width() == firstImage.Width() && image.Height() == firstImage.Height());
			// Upload pixel data.
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, firstImage.Width(), firstImage.Height(), 1,
					format, type, image.GetBuffer() );
		} catch( std::string _message ) {
			LOG_ERROR(_message + ". Will use first texture of the texture array as fall back.");
			// Upload alternative pixel data.
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, firstImage.Width(), firstImage.Height(), 1,
				format, type, firstImage.GetBuffer() );
		}
		LogGlError("glTexSubImage3D failed");
	}

	// Always set some texture parameters (required for some drivers)
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// Use nearest for more voxel feeling
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glGenerateMipmap( GL_TEXTURE_2D_ARRAY );

	LogGlError("Something in texture upload and setup failed");

	// Unbind to avoid later side effects
	glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
}

Texture::~Texture()
{
	glDeleteTextures( 1, &m_textureID );
}

}; // namespace Graphic