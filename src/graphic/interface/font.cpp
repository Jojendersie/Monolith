#include "font.hpp"
#include "../content.hpp"

//#define </cm(r,g,b,a) "</c + (char)r + (char)g + (char)b + (char)a"

namespace Graphic
{
	Font::Font(std::string _fontName) :
		m_texture("texture/"+_fontName+".png"),
		m_effect( "shader/font.vs", "shader/font.ps", "shader/font.gs")
	{
		m_effect.SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
		m_effect.SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::INV_SRC_ALPHA));
		m_effect.SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
		m_effect.BindTexture( "u_characterTex", 7, Graphic::Resources::GetSamplerState(SamplerStates::LINEAR) );
		m_effect.BindUniformBuffer( Graphic::Resources::GetUBO(UniformBuffers::GLOBAL) );

		Jo::Files::HDDFile file("texture/"+_fontName+".sraw");
		Jo::Files::MetaFileWrapper Wrap( file, Jo::Files::Format::SRAW );
		auto& PosX = Wrap.RootNode[std::string("positionX")];
		auto& PosY = Wrap.RootNode[std::string("positionY")];
		auto& sizeX = Wrap.RootNode[std::string("sizeX")];
		auto& sizeY = Wrap.RootNode[std::string("sizeY")];
		//get values
		for(int i = 0; i < 256; i++)
		{
			m_sizeTable[i] = Math::Vec2(sizeX[i],sizeY[i]);
			// Half pixel offset necessary - otherwise rounding errors in shader
			m_coordTable[i] = Math::Vec2(float(PosX[i]) + 0.5f/m_texture.Width(),PosY[i]);
		//	m_sizeTable[i] = Math::Vec2(1-0.01f*i,i*0.01f);//0.1; 0.25
		//	m_coordTable[i] = Math::Vec2(0.f,0.25f);
		}
	}

	TextRender::TextRender(Font* _font) :
		m_font(_font),
		m_characters( "222c11", VertexBuffer::PrimitiveType::POINT ),//222c11
		m_screenPos( 0.0f ),
		m_sizeD(2.f),
		m_colorD((uint8_t)255,(uint8_t)255,(uint8_t)255,(uint8_t)255),
		m_thicknessD(0.5f),
		m_active(true),
		m_sizeMax(0)
	{
		m_size = m_sizeD;
		m_color = m_colorD;
		m_thickness = m_thicknessD;
		SetText("no input");
	}


	void TextRender::SetText(const std::string& _text)
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

	int TextRender::CntrlChr(int _i)
	{
		int jmpCount = 0;
		switch (m_text[_i+1])
		{
		case 's': m_size = (uint8_t)(m_text[_i+3]*100+m_text[_i+4]*10+m_text[_i+5]-208) * 12.5f / 255.f;
			jmpCount = 5;
			break;
		case 't': m_thickness =(uint8_t) (m_text[_i+3]*100+m_text[_i+4]*10+m_text[_i+5]-208) * 6.25f / 255.f;
			jmpCount = 5;
			break;
		case 'c': //m_color = Utils::Color32F((uint8_t)m_text[_i+2],(uint8_t)m_text[_i+3],(uint8_t)m_text[_i+4],(uint8_t)m_text[_i+5]));
			m_color = Utils::Color8U(
				(uint8_t)(m_text[_i+3]*100+m_text[_i+4]*10+m_text[_i+5]-208), //offset: '0'(0x30) -> 0x00 5328 % 256
				(uint8_t)(m_text[_i+7]*100+m_text[_i+8]*10+m_text[_i+9]-208),
				(uint8_t)(m_text[_i+11]*100+m_text[_i+12]*10+m_text[_i+13]-208),
				(uint8_t)(m_text[_i+15]*100+m_text[_i+16]*10+m_text[_i+17]-208));
			jmpCount = 17;
			break;
		case '/': switch (m_text[_i+2]) //statement closed, return to default values
			{
			case 's' : m_size = m_sizeD; 
				break;
			case 'c' : m_color = m_colorD; 
				break;
			case 't' : m_thickness = m_thicknessD; 
				break;
			};
			jmpCount = 2;
			break;
		}
		//count chars til the formating end: '>'
		while(m_text[_i+(jmpCount++)] != '>');
		return jmpCount;
	}

	void TextRender::RenewBuffer()
	{
		m_characters.Clear();
		Math::Vec2 currentPos = m_screenPos;
		for(size_t i = 0; i<m_text.length(); i++)
		{
			CharacterVertex CV;
			if(m_size > m_sizeMax) m_sizeMax = m_size;
			CV.scale = m_size;//1.f 
			CV.size = m_font->m_sizeTable[(unsigned char)m_text[i]];
			CV.texCoord = m_font->m_coordTable[(unsigned char)m_text[i]];
			CV.position = Math::Vec2(currentPos[0],currentPos[1]); //(m_size-1.f) * CV.size[1]) 
			CV.thickness = m_thickness;
			CV.color = m_color.RGBA(); 
			//line break
			if(m_text[i] == '\n'){currentPos[0] = m_screenPos[0]; currentPos[1] -= GetDim()[1];}//offset to lower line space; requieres additional testing; m_font->m_sizeTable[m_text[i]][1]*m_size*0.666f
			else if(m_text[i] == '<'){ i += CntrlChr(i)-1; continue;} 
			else currentPos[0] += m_font->m_sizeTable[(unsigned char)m_text[i]][0]*m_size;  
 			m_characters.Add(CV);
		}
		m_characters.SetDirty();
	}
}; 