#include "Bullet.h"
#include "Enemy.h"

Bullet::~Bullet()
{
}

bool Bullet::isAnimateDone(Animate* animate)
{
	if (animate->getCurrentFrameIndex() + 1 ==
		animate->getAnimation()->getFrames().size())
	{
		stopAllActions();
		setDefaultSpriteFrame();
		setVisible(false);
		m_goal = Vec2::ZERO;
		return true;
	}
	return false;
}

void Bullet::dealDamageInRange(float range)
{
	// get Tower
	auto tower = getParent()->getParent();
	Vector<Node*> nodeList = tower->getParent()->getChildren();
	//Vec2 pos = tower->getPosition() + getPosition();
	Rect rect = getBoundingBox();
	rect.origin += tower->getPosition();
	for (Node* node : nodeList) {
		if (node->getTag() == ENEMY_TAG && node->isVisible() &&
			//pos.getDistance(node->getPosition()) < 10.0f
			rect.intersectsRect(node->getBoundingBox())
		) 
		{
			((Enemy*)node)->takeDamage(m_damage);
		}
	}
}

void Bullet::moveTo(float speed, Vec2 pos)
{
	Vec2 t = pos - getPosition();
	if (t.getLength() >= speed) {
		setPosition(getPosition() + t.getNormalized()*speed);
	}
	else {
		setVisible(true);
		setPosition(pos);
		if(!m_isMovingWithAnimation){
			runAnimate();
		}
	}
}

void Bullet::Init(Texture2D* texture, Vec2 pos, std::vector<Rect> rectList)
{
	addAnimation(texture, rectList, false);
	AnimatedObject::Init("bullet", pos);
	m_target = nullptr;
	m_goal = Vec2::ZERO;
	m_isMovable = false;
	m_isVertical = false;
	m_isMovingWithAnimation = false;
	setVisible(m_isMovable);
}

Bullet * Bullet::create(Texture2D* texture, Vec2 pos, std::vector<Rect> rectList)
{
	auto ret = Bullet::create();
	ret->Init(texture, pos, rectList);

	return ret;
}

Bullet * Bullet::clone()
{
	auto clone = Bullet::create();

	clone->m_target = m_target;
	clone->m_speed = m_speed;
	clone->m_damage = m_damage;
	clone->m_delay = m_delay;
	clone->m_duration = m_duration;
	clone->m_isMovable = m_isMovable;
	clone->m_isVertical = m_isVertical;
	clone->m_isMovingWithAnimation = m_isMovingWithAnimation;

	clone->cloneProperties(this);
	clone->setVisible(isVisible());
	clone->unscheduleUpdate();

	return clone;
}

void Bullet::setSpeed(float speed)
{
	m_speed = speed;
}

void Bullet::setDamage(float damage)
{
	m_damage = damage;
}

void Bullet::setTarget(Node *target)
{
	if (!m_target || !target) {
		m_target = target;
		setVisible(target != nullptr && m_isMovable);
		if (!target) setPosition(Vec2::ZERO);
	}
}

Node * Bullet::getTarget()
{
	return m_target;
}

void Bullet::setDelay(float delay)
{
	m_delay = delay;
}

void Bullet::setDuration(float duration)
{
	m_duration = duration;
}

void Bullet::setPath(Vec2 begin, Vec2 end)
{
	if (m_isVertical) {
		setPosition(begin + Vec2(0, m_speed *m_delay));
	}
	else {
		setPosition(begin);
	}
	m_goal = end;
	setVisible(m_isMovable);
	if(m_isMovingWithAnimation){
		runAnimate();
	}
}

void Bullet::setMovingWithAnimation(bool isMovingWithAnimation)
{
	m_isMovingWithAnimation = isMovingWithAnimation;
}

void Bullet::setMovable(bool isMovable)
{
	m_isMovable = isMovable;
	setVisible(isMovable);
}

void Bullet::setVertical(bool isVertical)
{
	m_isVertical = isVertical;
}

bool Bullet::doTargetAction(float dt)
{
	Animate *action = getAnimate();
	if (action) {
		if (isAnimateDone(action)) {
			if (m_target) 
				((Enemy*)m_target)->takeDamage(m_damage);
			setPosition(Vec2::ZERO);
			return true;
		}
		else {
			if (m_target) {
				setPosition(getParent()->convertToNodeSpace(m_target->getPosition()));
				setLocalZOrder(-getPosition().y);
			}
			else {
				return true;
			}
		}
	}

	if (!action && m_target) {
		moveTo(m_speed*dt, getParent()->convertToNodeSpace(m_target->getPosition()));
		setLocalZOrder(-getPosition().y);
	}
	return false;
}

bool Bullet::doCircleAction(float dt)
{
	if (m_duration)
	{
		m_duration -= dt;
		if (m_duration < 0)
			m_duration = 0;
		return false;
	}
	Animate *action = getAnimate();
	if (action){
		if (action->getCurrentFrameIndex() + 3 == action->getAnimation()->getFrames().size()) {
			if (action->getCurrentFrameIndex() + 1 == action->getAnimation()->getFrames().size()) {
				stopAllActions();
				setDefaultSpriteFrame();
				setVisible(false);
				return true;
			}
		}
		else{
			moveTo(m_speed*dt, m_goal);
		}
		setLocalZOrder(-getPosition().y);
	}
	return action == nullptr;
}

bool Bullet::doMovingAction(float dt)
{
	Animate *action = getAnimate();
	if (action) {
		if (isAnimateDone(action)) {
			//if (m_target) ((Enemy*)m_target)->takeDamage(m_damage);
			dealDamageInRange(10.0f);
		}
	}
	else {
		if (m_goal == Vec2::ZERO) {
			return true;
		}
		else {
			moveTo(m_speed*dt, m_goal);
		}
		if (m_isVertical) {
			setHeight(m_goal, m_speed *m_delay);
			setLocalZOrder(-m_goal.y); // it should be that!!!
		}
		else {
			setLocalZOrder(-getPosition().y);
		}
	}
	return false;
}
