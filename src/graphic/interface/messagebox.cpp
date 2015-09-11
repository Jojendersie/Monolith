#include "messagebox.hpp"

using namespace Math;

MessageBox::MessageBox(Jo::Files::MetaFileWrapper* _posMap,
	Math::Vec2 _position, Math::Vec2 _size) :
ScreenTexture(_posMap, "simpleWindow", _position, _size),
m_textRender(&Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT))
{
	Math::Vec2 captionDim = m_textRender.GetDim();
	m_textRender.SetDefaultSize(1.f);

	m_textRender.SetPos(m_pos + Vec2(0.02f, -m_textRender.GetDim()[1]));

	m_sizeTextArea = m_size - Vec2(0.04f, 0.02f);
}


MessageBox::~MessageBox()
{
}

void MessageBox::Process(float _deltaTime)
{
	if (m_pause || !m_messageQue.size()) return;

	m_timeToChange -= _deltaTime;

	if (m_timeToChange <= 0)
	{
		m_messageQue.erase(m_messageQue.begin());

		UpdateMsg();
	}
}

void MessageBox::DisplayMsg(const std::string& _msg, float _duration, bool _inQue)
{
	if (!_inQue) ClearQue();
	//make visible
	m_active = true;
	m_messageQue.emplace_back(_msg, _duration);

	if (m_messageQue.size() == 1)
		UpdateMsg();
}

void MessageBox::ClearQue()
{
	m_messageQue.clear();
}

void MessageBox::UpdateMsg()
{
	if (m_messageQue.size())
	{
		auto& newMsg = *m_messageQue.begin();

		m_timeToChange = newMsg.second;
		m_textRender.SetText(newMsg.first);
		m_textRender.SetExpanse(m_sizeTextArea, true);
	}
	else
	{
		m_textRender.SetText("");

		//hide
		if (!m_showPermanent)
			m_active = false;
	}
}