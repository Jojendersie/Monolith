#include <cstdint>
#include <jofilelib.hpp>
#include "texture.hpp"
#include "opengl.hpp"
#include "math/math.hpp"
#include "utilities/assert.hpp"

namespace Graphic {

// Maps number of channels to a type
Texture::Format::Format(unsigned int _numChannels, unsigned int _bitDepth, Format::ChannelType _type, FormatType _formatType)
{
	Assert(_numChannels > 0 && _numChannels <= 4, "More than 4 channels are not supported!");
	Assert(_formatType == FormatType::COLOR || _numChannels == 1, "Multiple channels are not possible for depth/stencil formats!");

	if (_formatType == FormatType::COLOR)
	{
		--_numChannels;
		static const uint32_t FLOAT_FORMATS[2][4] = { { GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F },
		{ GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F } };
		static const uint32_t INT_FORMATS[2][4] = { { GL_R8_SNORM, GL_RG8_SNORM, GL_RGB8_SNORM, GL_RGBA8_SNORM },
		{ GL_R16_SNORM, GL_RG16_SNORM, GL_RGB16_SNORM, GL_RGBA16_SNORM } };
		static const uint32_t UINT_FORMATS[2][4] = { { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 },
		{ GL_R16, GL_RG16, GL_RGB16, GL_RGBA16 } };


 		switch (_type)
		{
		case Format::ChannelType::FLOAT:
			Assert(_bitDepth == 32 || _bitDepth == 16, "Float format is only available in 16 and 32 bit.");
			internalFormat = FLOAT_FORMATS[_bitDepth / 16 - 1][_numChannels];
			break;
		case Format::ChannelType::INT:
			Assert(_bitDepth == 16 || _bitDepth == 8, "Int format is currently only available in 8 and 16 bit.");
			internalFormat = INT_FORMATS[_bitDepth / 8 - 1][_numChannels];
			break;
		case Format::ChannelType::UINT:
			Assert(_bitDepth == 16 || _bitDepth == 8, "Int format is currently only available in 8 and 16 bit.");
			internalFormat = UINT_FORMATS[_bitDepth / 8 - 1][_numChannels];
			break;

		default:
			throw std::string("Image format is not supported as texture.");
		}


		// The array dimension is numChannels
		const uint32_t FORMAT[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		format = FORMAT[_numChannels];
	}
	else if (_formatType == FormatType::DEPTH)
	{
		if (_type == ChannelType::FLOAT)
		{
			Assert(_bitDepth == 32, "Unsupported depth format bitdepth!");
			internalFormat = GL_DEPTH_COMPONENT32;
		}
		else
		{
			Assert(_bitDepth == 32 || _bitDepth == 24 || _bitDepth == 16, "Unsupported depth format bitdepth!");
			static const GLuint depthFormats[] = { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F };
			internalFormat = depthFormats[(_bitDepth - 16) / 8];
		}
		
		format = GL_DEPTH_COMPONENT;
	}
	else //  if (_formatType == FormatType::DEPTH_STENCIL)
	{
		internalFormat = GL_DEPTH24_STENCIL8;
		format = GL_DEPTH_STENCIL;
	}

	// The inner array dimension depends on bitDepth, the outer on the ChannelType
	const uint32_t TYPE[][4] = { { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, 0, GL_UNSIGNED_INT },
									{ GL_BYTE, GL_SHORT, 0, GL_UNSIGNED_INT },
									{ 0, 0, 0, GL_FLOAT } };
	type = TYPE[int(_type)][_bitDepth / 8 - 1];
}

Texture::Texture(unsigned int _width, unsigned int _height, const Format& format, unsigned int _numMipLevels) :
	m_width(_width),
	m_height(_height),
	m_depth(1),
	m_numMipLevels(_numMipLevels),
	m_bindingPoint(GL_TEXTURE_2D)
{
	if (m_numMipLevels == 0)
	{
		m_numMipLevels = GetMaxPossibleMipMapLevels();
	}
	else
	{
		Assert(m_numMipLevels < GetMaxPossibleMipMapLevels(), "Invalid mipmap count!");
	}
	Assert(_width != 0 && _height != 0,  "Invalid texture dimension!");

	GL_CALL(glGenTextures, 1, &m_textureID);
	GL_CALL(glBindTexture, m_bindingPoint, m_textureID);  // Todo: Tell device that a texture has changed

	for (unsigned int i = 0; i < m_numMipLevels; ++i)
	{
		GL_CALL(glTexImage2D, m_bindingPoint, 0, format.internalFormat, m_width, m_height, 0, format.format, format.type, nullptr);
	}

	SetDefaultTextureParameter();

	GL_CALL(glBindTexture, m_bindingPoint, 0);  // Todo: Tell device that a texture has changed
}

Texture::Texture( const std::string& _fileName ) :
	m_numMipLevels(1)
{
	GL_CALL(glGenTextures, 1, &m_textureID);

	m_bindingPoint = GL_TEXTURE_2D;	// TODO switch
	GL_CALL(glBindTexture, m_bindingPoint, m_textureID); // Todo: Tell device that a texture has changed

	Jo::Files::HDDFile file(_fileName);
	Jo::Files::ImageWrapper image(file, Jo::Files::Format::PNG );

	m_width = image.Width();
	m_height = image.Height();
	m_depth = 1;	// TODO: volume texture support

	Format format = Format(image.NumChannels(), image.BitDepth(), static_cast<Format::ChannelType>(image.GetChannelType()));
	int width = image.Width() * 2;
	int height = image.Height() * 2;
	int level = 0;
	do {
        width = Math::max(1, (width / 2));
        height = Math::max(1, (height / 2));
		GL_CALL(glTexImage2D, m_bindingPoint, level, format.internalFormat, width, height, 0,
								format.format, format.type, level==0? image.GetBuffer() : nullptr);
		++level;
    } while( width * height > 1 );

	
	m_numMipLevels = GetMaxPossibleMipMapLevels();
	SetDefaultTextureParameter();

	GL_CALL(glGenerateMipmap, m_bindingPoint);

	GL_CALL(glBindTexture, m_bindingPoint, 0); // Todo: Tell device that a texture has changed
}


Texture::Texture( const std::vector<std::string>& _fileNames ) :
	m_numMipLevels(1)
{
	GL_CALL(glGenTextures, 1, &m_textureID);

	m_bindingPoint = GL_TEXTURE_2D_ARRAY;
	GL_CALL(glBindTexture, GL_TEXTURE_2D_ARRAY, m_textureID); // Todo: Tell device that a texture has changed

	// Use first file as reference file.
	Jo::Files::HDDFile firstFile(_fileNames[0]);
	Jo::Files::ImageWrapper firstImage(firstFile, Jo::Files::Format::PNG );

	m_width = firstImage.Width();
	m_height = firstImage.Height();
	m_depth = (int)_fileNames.size();
	
	// Expect all textures to be ok. Some of the layers might be empty later.
	Format format = Format(firstImage.NumChannels(), firstImage.BitDepth(), static_cast<Format::ChannelType>(firstImage.GetChannelType()));
	int width = firstImage.Width() * 2;
	int height = firstImage.Height() * 2;
	int level = 0;
	do {
        width = Math::max(1, (width / 2));
        height = Math::max(1, (height / 2));
		GL_CALL(glTexImage3D, m_bindingPoint, level, format.internalFormat, width, height, _fileNames.size(),
							  0, format.format, format.type, nullptr);
		++level;
	} while (width * height > 1);
	// Upload pixel data of first image.
	GL_CALL(glTexSubImage3D, m_bindingPoint, 0, 0, 0, 0, firstImage.Width(), firstImage.Height(), 1,
							format.format, format.type, firstImage.GetBuffer());

	// Load all images from file.
	for( size_t i=1; i<_fileNames.size(); ++i )
	{
		try
		{
			Jo::Files::HDDFile file(_fileNames[i]);
			Jo::Files::ImageWrapper image(file, Jo::Files::Format::PNG );
			Assert(image.Width() == firstImage.Width() && image.Height() == firstImage.Height(), "All images in texture array need to have the same dimensions.");
			// Upload pixel data.
			GL_CALL(glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, firstImage.Width(), firstImage.Height(), 1,
				format.format, format.type, image.GetBuffer());
		}
		catch( std::string _message )
		{
			LOG_ERROR(_message + ". Will use first texture of the texture array as fall back.");
			// Upload alternative pixel data.
			GL_CALL(glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, firstImage.Width(), firstImage.Height(), 1,
				format.format, format.type, firstImage.GetBuffer());
		}
	}

	m_numMipLevels = GetMaxPossibleMipMapLevels();
	SetDefaultTextureParameter();

	GL_CALL(glGenerateMipmap, m_bindingPoint);


	// Unbind to avoid later side effects
	GL_CALL(glBindTexture, m_bindingPoint, 0); // Todo: Tell device that a texture has changed
}

void Texture::SetDefaultTextureParameter()
{
	// Always set some texture parameters (required for some drivers)
	GL_CALL(glTexParameteri, m_bindingPoint, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Use nearest by default for more voxel feeling
	GL_CALL(glTexParameteri, m_bindingPoint, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Define valid mipmap range. See https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
	GL_CALL(glTexParameteri, m_bindingPoint, GL_TEXTURE_BASE_LEVEL, 0);
	GL_CALL(glTexParameteri, m_bindingPoint, GL_TEXTURE_MAX_LEVEL, m_numMipLevels - 1);
}

unsigned int Texture::GetMaxPossibleMipMapLevels()
{
	unsigned int x = m_width;
	unsigned int y = m_height;
	unsigned int z = m_depth;
	unsigned int numMipLevels = 0;

	while (x > 1 || y > 1 || z > 1)
	{
		++numMipLevels;
		x /= 2;
		y /= 2;
		z /= 2;
	}
	
	return numMipLevels;
}

Texture::~Texture()
{
	GL_CALL(glDeleteTextures, 1, &m_textureID);
}

}; // namespace Graphic