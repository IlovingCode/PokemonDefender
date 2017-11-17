#include "AnimatedObject.h"

#define FRAME_RATE		10
#define LOOPS			-1
#define SHADOW_SCALE	0.5f
#define USE_SHADOW_3D	0

AnimatedObject::~AnimatedObject()
{
	m_animateList.clear();
}

SpriteFrame * AnimatedObject::getFirstFrameOfAnimation(int animationId)
{
	return m_animateList.at(animationId)->getAnimation()->getFrames().front()->getSpriteFrame();
}

Animate * AnimatedObject::getAnimate(int tag)
{
	return static_cast<Animate*>(m_sprite->getActionByTag(tag));
}

Rect AnimatedObject::getBoundingBox() const
{
	return RectApplyAffineTransform(m_sprite->getBoundingBox(), getNodeToParentAffineTransform());
}

// deprecated
//void AnimatedObject::pause()
//{
//	Node::pause();
//	for (Node* child : getChildren())
//		child->pause();
//}
//
//void AnimatedObject::resume()
//{
//	Node::resume();
//	for (Node* child : getChildren())
//		child->resume();
//}

void AnimatedObject::setScale(float scale)
{
	m_sprite->setScale(scale);
	m_shadow->setScaleX(scale);
	m_shadow->setScaleY(scale * SHADOW_SCALE);
}

float AnimatedObject::getScale() const
{
	return m_sprite->getScale();
}

void AnimatedObject::setHeight(Vec2 original, float maxHeight)
{
	m_shadow->setPosition(original - getPosition());
	m_shadow->setOpacity(MathUtil::lerp(150, 0, -m_shadow->getPosition().y / maxHeight));
}

void AnimatedObject::setAnchorPoint(const Vec2 &anchorPoint)
{
	// this method is used only for non-shadow object
	if (m_shadow->isVisible())
		return;

	m_sprite->setAnchorPoint(anchorPoint);
}

void AnimatedObject::enableShadow(bool enable)
{
	if (m_shadow)
		m_shadow->setVisible(enable);

	if (enable)
		m_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
}

void AnimatedObject::cloneProperties(AnimatedObject* obj)
{
	for (Animate* animate : obj->m_animateList) {
		m_animateList.pushBack(animate->clone());
	}
	//Init(obj->getName().c_str(), obj->getPosition());

	m_shadow = Sprite::create();
	m_shadow->setAnchorPoint(obj->m_shadow->getAnchorPoint());
	m_shadow->setSpriteFrame(obj->m_shadow->getSpriteFrame());
	// ScaleX != ScaleY, need clone two values
	m_shadow->setScaleX(obj->m_shadow->getScaleX());
	m_shadow->setScaleY(obj->m_shadow->getScaleY());
	// apply new shadow simulation
	// use 3D rotation instead of scalation
	m_shadow->setRotation3D(obj->m_shadow->getRotation3D());
	m_shadow->setFlippedX(obj->m_shadow->isFlippedX());
	m_shadow->setColor(obj->m_shadow->getColor());
	m_shadow->setOpacity(obj->m_shadow->getOpacity());
	m_shadow->setLocalZOrder(obj->m_shadow->getLocalZOrder());

	m_sprite = Sprite::create();
	m_sprite->setAnchorPoint(obj->m_sprite->getAnchorPoint());
	m_sprite->setSpriteFrame(obj->m_sprite->getSpriteFrame());
	m_sprite->setScale(obj->m_sprite->getScale());
	m_sprite->setFlippedX(obj->m_sprite->isFlippedX());

	addChild(m_sprite);
	addChild(m_shadow);
	m_currentAnimateId = obj->m_currentAnimateId;
	setName(obj->getName());
	setPosition(obj->getPosition());
	// Node has no need to copy anchorPoint
	//Node::setAnchorPoint(obj->getAnchorPoint());
	setLocalZOrder(obj->getLocalZOrder());
	setTag(obj->getTag());
	scheduleUpdate();

	if (m_currentAnimateId >= 0) {
		m_shadow->runAction(m_animateList.at(m_currentAnimateId)->clone());
		m_sprite->runAction(m_animateList.at(m_currentAnimateId));
	}
}

void AnimatedObject::setDefaultSpriteFrame(int animateId)
{
	m_shadow->setSpriteFrame(getFirstFrameOfAnimation(animateId));
	m_sprite->setSpriteFrame(getFirstFrameOfAnimation(animateId));
}
//Deprecated
void AnimatedObject::addAnimation(const char* path, int frameCount, int begin,  bool isLoop)
{
	auto animation = Animation::create();

	char str[50] = { 0 };
	for (int i = 0; i < frameCount; i++)
	{
		sprintf(str, "%s%d.png", path, i + begin);
		animation->addSpriteFrameWithFile(str);
	}
	animation->setLoops(isLoop ? LOOPS : 1);
	animation->setDelayPerUnit(Director::getInstance()->getAnimationInterval());

	m_animateList.pushBack(Animate::create(animation));
}
//Deprecated
void AnimatedObject::addAnimation(Texture2D *texture, std::vector<Rect> rectList, bool isLoop)
{
	auto animation = Animation::create();

	for (Rect rect : rectList) {
		animation->addSpriteFrame(SpriteFrame::createWithTexture(texture, rect));
	}
	animation->setLoops(isLoop ? LOOPS : 1);
	animation->setDelayPerUnit(1.0f / FRAME_RATE);

	m_animateList.pushBack(Animate::create(animation));
}

void AnimatedObject::runAnimate(int id, bool isFlippedX, int tag)
{
	if (id < 0 || id >= m_animateList.size()) return;

	m_shadow->setFlippedX(isFlippedX);
	m_sprite->setFlippedX(isFlippedX);

	if (m_currentAnimateId == id && m_sprite->getActionByTag(0)) {
		return;
	}

	m_currentAnimateId = id;
	m_animateList.at(id)->setTag(tag);
	m_shadow->stopAllActions();
	m_sprite->stopAllActions();
	m_shadow->runAction(m_animateList.at(id)->clone());
	m_sprite->runAction(m_animateList.at(id));
}

void AnimatedObject::runSpriteAction(Action *action, bool needStopAll)
{
	if (needStopAll) {
		m_shadow->stopAllActions();
		m_sprite->stopAllActions();
	}
	m_shadow->runAction(action->clone());
	m_sprite->runAction(action);
}

void AnimatedObject::Init(const char* name, Vec2 pos)
{
	m_shadow = Sprite::create();
	m_shadow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
#if USE_SHADOW_3D
	// apply new shadow simulation
	// use 3D rotation instead of scalation
	m_shadow->setRotation3D(Vec3(-45, -10, 0));
#else
	m_shadow->setScaleY(SHADOW_SCALE);
#endif
	m_shadow->setColor(Color3B::BLACK);
	m_shadow->setOpacity(150);
	m_shadow->setLocalZOrder(-500);

	m_sprite = Sprite::create();
	m_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);

	addChild(m_sprite);
	addChild(m_shadow);
	m_currentAnimateId = -1;
	setName(name);
	setPosition(pos);
	// Do not need to set anchorPoint
	//Node::setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	setDefaultSpriteFrame();
	setVisible(false);
	setLocalZOrder(-getPosition().y);
}