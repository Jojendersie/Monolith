#pragma once

#include <string>
#include <vector>
#include "ei/vector.hpp"

namespace Graphic {

	/// \brief Wrapper for an OpenGL texture.
	class Texture {
	public:
		/// \brief Format descriptor that converts a human readable description into OpenGL descriptors.
		/// See tables at https://www.opengl.org/wiki/GLAPI/glTexImage2D
		struct Format
		{
			/// Channel type description. Compatible to Jo::Files::ImageWrapper::ChannelType
			enum struct ChannelType
			{
				UINT,	///< Any unsigned int with 1,2,4,8,16 or 32 bit (depends on bit depth)
				INT,	///< Any signed int with 8, 16 or 32 bit (depends on bit depth)
				FLOAT	///< 32bit float with 16 or 32 bit (depends on bit depth)
			};

			/// Different texture format types.
			enum struct FormatType
			{
				COLOR,
				DEPTH,
				DEPTH_STENCIL
			};

			/// Constructs a new format form the given parameters.
			/// \param [in] _numChannels
			///   Number of channels for this format. Will be ignored for _formatType != FormatType::COLOR.
			/// \param [in] _bitDepth
			///   bitDepth per Channel. For stencil formats this value is ignored since only GL_DEPTH24_STENCIL8 is supported.
			///   (This means that neither GL_DEPTH32F_STENCIL8 nor GL_STENCIL_INDEX8 are supported)
			Format(unsigned int _numChannels, unsigned int _bitDepth, ChannelType _channelType, FormatType _formatType = FormatType::COLOR);
		
			// OpenGL internal format descriptors.
			unsigned int internalFormat;
			unsigned int format;
			unsigned int type;
		};

		/// \brief Creates an empty 2D texture.
		/// \param [in] _numMipLevels 0 will result in a full mipmap chain.
		/// \todo Is there a better channel description?
		Texture(unsigned int _width, unsigned int _height, const Format& _format, unsigned int _numMipLevels = 1);

		/// \brief Load a single 2D,3D or cubemap texture from file.
		///
		/// Currently supported: 2D TODO: the others
		/// Will create a full mipmap chain.
		Texture( const std::string& _fileName );

		/// \brief Load an 2D texture array. The texture index in the
		///		array is derived from its index in the vector.
		///
		/// Will create a full mipmap chain.
		/// \param [in] _fileNames List of image files to be loaded into
		///		the array.
		Texture( const std::vector<std::string>& _fileNames );

		/// \brief Creates an empty 3D texture array.
		/// \details The array can be filled with UploadData().
		///
		///		Since 3D texture arrays are not supported natively this is done
		///		by creating a (_width * _height) x _depth 2D texture.
		/// \param [in] _layers Number of textures in the array.
		/// \param [in] _numMipLevels 0 will result in a full mipmap chain.
		Texture( unsigned int _width, unsigned int _height, unsigned int _depth, unsigned int _layers, const Format& format, unsigned int _numMipLevels = 1 );

		/// \brief Release resource
		~Texture();
		
		/// \brief Return width of the loaded texture.
		unsigned int Width()		{ return m_width; }

		/// \brief Return width of the loaded texture.
		unsigned int Height()		{ return m_height; }

		/// \brief Return depth/layers of a volume texture or texture array.
		/// \details For standard 2D textures the return value is 1.
		unsigned int Depth()		{ return m_depth; }

		/// \brief Returns number of available mipLevels.
		unsigned int MipLevels()	{ return m_numMipLevels; }

		/// \brief Returns the 2d size of the texture as Vector2
		ei::IVec2 Size2D() { return ei::IVec2(m_width, m_height); }

		/// \brief Fill a texture with data.
		/// \details This calls glTexSubImage3D with the width and height for the
		///		target mipmap level.
		///
		///		Additional the texture is bound automatically.
		void UploadData(unsigned _layer, unsigned _mipLevel, const void* _buffer);

	private:
		/// Sets some default texture parameter to the binding point.
		void SetDefaultTextureParameter();

		/// Determines how many mipmap levels a full mipmap chain would contain.
		unsigned int GetMaxPossibleMipMapLevels();

		Format m_format;			///< Format desc created during construction

		unsigned m_textureID;		///< OpenGL texture handle
		unsigned m_bindingPoint;	///< The constructor determines GL_TEXTURE_2D_ARRAY,... 

		unsigned int m_width;		///< Just informative: width of the texture
		unsigned int m_height;		///< Just informative: height of the texture
		// TODO: the 3D texture handling changed (27.04.) most method might be incompatible
		unsigned int m_depth;		///< Just informative: depth of the texture (volume) or 1
		unsigned int m_layers;		///< number of slices in an array or 1

		unsigned int m_numMipLevels;

		friend class Device;
		friend class Framebuffer;
	};

}; // namespace Graphic