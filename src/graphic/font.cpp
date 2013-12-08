#include "font.hpp"

namespace Graphic
{
	Font::Font(std::string _fontName, Content* _globalPipelineData) :
		m_texture("texture/"+_fontName+".png"),
		m_effect( "shader/font.vs", "shader/font.gs", "shader/font.ps",
				Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID,
				Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false)
	{
		m_effect.BindTexture( "u_characterTex", 7, _globalPipelineData->linearSampler );
		m_effect.BindUniformBuffer( _globalPipelineData->cameraUBO );

		Jo::Files::HDDFile file("texture/"+_fontName+".sraw", true);
		Jo::Files::MetaFileWrapper Wrap( file, Jo::Files::Format::SRAW );
		auto& PosX = Wrap.RootNode[std::string("positionX")];
		auto& PosY = Wrap.RootNode[std::string("positionY")];
		auto& sizeX = Wrap.RootNode[std::string("sizeX")];
		auto& sizeY = Wrap.RootNode[std::string("sizeY")];
		//default values for testing
		for(int i = 0; i < 256; i++)
		{
			m_sizeTable[i] = Math::Vec2(sizeX[i],sizeY[i]);
			m_CoordTable[i] = Math::Vec2(PosX[i],PosY[i]);
		//	m_sizeTable[i] = Math::Vec2(1-0.01f*i,i*0.01f);//0.1; 0.25
		//	m_CoordTable[i] = Math::Vec2(0.f,0.25f);
		}
	}

	TextRender::TextRender(Font* _font) :
		m_font(_font),
		m_characters( 1000, "222c11", VertexBuffer::PrimitiveType::POINT ),//222c11
		m_screenPos( 0.0f )
	{
		SetText("bla");
	}


	void TextRender::SetText(std::string _text)
	{ 
		m_text = _text;
		RenewBuffer();
	}


	void TextRender::SetPos(Math::Vec2 _screenPos)
	{ 
		m_screenPos = _screenPos;
		RenewBuffer();
//		CharacterVertex* CharVertex = (CharacterVertex*)m_characters.Get( 0 );
//		for( int i=0; i<m_text.length(); i++)
//			CharVertex[i].position += CharVertex[i].position - _screenPos; 
	}

	void TextRender::Draw()
	{
		Graphic::Device::SetEffect( m_font->m_effect );
		Graphic::Device::SetTexture( m_font->m_texture, 7 );
		Graphic::Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices() );
	}

	void TextRender::RenewBuffer()
	{
		m_characters.Clear();
		Math::Vec2 currentPos = m_screenPos;
		for(size_t i = 0; i<m_text.length(); i++)
		{
			CharacterVertex CV;
			CV.position = currentPos;
			CV.scale = 1.f; 
			CV.size = m_font->m_sizeTable[m_text[i]];
			CV.texCoord = m_font->m_CoordTable[m_text[i]];
			CV.thickness = 0.5f;
			//line break
			if(m_text[i] == 13){currentPos.x = m_screenPos.x; currentPos.y -= m_font->m_sizeTable[m_text[i]].y;}
			else currentPos.x += m_font->m_sizeTable[m_text[i]].x; 
 			m_characters.Add(&CV);
		}
		m_characters.Commit();
	}
}; 