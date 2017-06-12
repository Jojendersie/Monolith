#include "messagebox.hpp"

using namespace ei;

MessageBox::MessageBox(Vec2 _position, Vec2 _size) :
ScreenTexture("simpleWindow", _position, _size),
m_textRender(Vec2(0.f), Anchor(),
&Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT))
{
	ei::Vec2 captionDim = m_textRender.GetCharSize();
	m_textRender.SetDefaultSize(1.f);

	m_textRender.SetPosition(m_position + Vec2(0.02f, -m_textRender.GetCharSize()[1]));

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
		m_textRender.SetRectangle(m_sizeTextArea, true);
	}
	else
	{
		m_textRender.SetText("");

		//hide
		if (!m_showPermanent)
		{
			m_active = false;
			m_visible = false;
		}
	}
}