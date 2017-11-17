#include "TextController.h"

TextController::TextController()
{
}

TextController::~TextController()
{
	CC_SAFE_DELETE_ARRAY(m_buff);
}

void TextController::initText(Text * text)
{
	m_text = text;
	m_text->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	m_alpha = 1;
	m_oldInt = m_newInt = 0;
	m_buff = new char[20];
}

void TextController::setTextStr(std::string str)
{
	m_textStr = str;
}

void TextController::setTextInt(int integer)
{
	m_oldInt = m_newInt;
	m_newInt = integer;
	m_alpha = 0;
}

void TextController::processInteger()
{
	int temp = m_oldInt + (m_newInt - m_oldInt) * m_alpha;
	sprintf(m_buff, "%d", temp);

	float scale = m_alpha < 0.5f ? m_alpha : (1.0f - m_alpha);
	m_text->setString(m_buff);
	m_text->setScale(scale * 2 + 1.0f);
}

void TextController::update(float dt)
{
	if (m_alpha < 1) {
		m_alpha += dt;
		processInteger();
	}
}
