#include "PokeBall.h"

extern std::vector<Rect> getRectList(int col, int row, int frameCount, Rect& rect);

void PokeBall::moveTo(float speed, Vec2 pos)
{
	Vec2 t = pos - getPosition();
	if (t.getLength() >= speed) {
		setPosition(getPosition() + t.getNormalized()*speed);
	}
	else {
	}
}

void PokeBall::Init(const char *name, Vec2 pos)
{
	auto texture = Director::getInstance()->getTextureCache()->addImage(name);

	Rect rect = Rect(0, 0, 64, 64);
	auto rectList = getRectList(12, 4, 12, rect);
	addAnimation(texture, rectList, true);

	AnimatedObject::Init("PokeBall", pos);

	m_target	= nullptr;
	m_isActive	= false;
	m_origin	= pos;
	m_speed		= 0;
	m_vibrate	= getBoundingBox().size / 8;

	setVisible(true);
	enableShadow(false);
	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	setLocalZOrder(0);
	scheduleUpdate();
}

void PokeBall::update(float dt)
{
	if (m_isActive) {
		moveTo(m_speed*dt, Vec2(m_target->getPositionX(), m_target->getBoundingBox().getMaxY()));
	}
	else {
		setPosition(m_origin + Vec2(random(-m_vibrate.width, m_vibrate.width), random(-m_vibrate.height, m_vibrate.height)));
	}
	
}

PokeBall * PokeBall::create(const char* name, Vec2 pos)
{
	if (!cocos2d::FileUtils::getInstance()->isFileExist(name))
		return nullptr;

	auto ret = PokeBall::create();
	ret->Init(name, pos);

	return ret;
}

PokeBall * PokeBall::clone()
{
	auto clone = PokeBall::create();

	clone->m_speed		= m_speed;
	clone->m_target		= m_target;
	clone->m_origin		= m_origin;
	clone->m_vibrate	= m_vibrate;
	clone->m_isActive	= m_isActive;

	clone->cloneProperties(this);

	return clone;
}

void PokeBall::setTarget(Tower* target)
{
	m_target = target;
}

void PokeBall::setSpeed(float speed)
{
	m_speed = speed;
}

void PokeBall::onTouch(Vec2 pos)
{
	m_isActive = true;

	
}
