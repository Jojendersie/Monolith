#pragma once

#include "hudelements.hpp"
#include "font.hpp"

class MessageBox : public Graphic::ScreenTexture
{
public:
	MessageBox(ei::Vec2 _position, ei::Vec2 _size);
	~MessageBox();

	void Process(float _deltaTime);

	/// \brief Displays the given message on the screen
	/// \details The message is shown for _duration seconds.
	/// When _inQue ist true previous messages while be displayed first for their complete duration.
	void DisplayMsg(const std::string& _msg, float _duration, bool _inQue = true);

	void ClearQue();

	void Pause(bool _state) { m_pause = _state; };

	Graphic::TextRender m_textRender;
private:

	void UpdateMsg();

	std::list < std::pair < std::string, float > > m_messageQue;

	ei::Vec2 m_sizeTextArea;

	float m_timeToChange;

	bool m_pause;
	bool m_showPermanent;
};

