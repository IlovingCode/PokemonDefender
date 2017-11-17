#include "Skill.h"
#include "Bullet.h"

#define	RANGEPERCENT	0.7f

Skill * Skill::create(const char * name, int range)
{
	char str[50] = { 0 };
	sprintf(str, "moves/_config.json", name);
	if (cocos2d::FileUtils::getInstance()->isFileExist(str)){
		rapidjson::Document doc;
		doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("moves/_config.json").c_str());

		if (!doc.HasMember(name)){
			return nullptr;
		}
	}
	else{
		return nullptr;
	}

	auto ret = create();
	ret->Init(name, Vec2::ZERO, range);

	return ret;
}

Skill * Skill::clone()
{
	auto clone = Skill::create();

	clone->setVisible(isVisible());
	clone->setTag(getTag());
	clone->m_range = m_range;
	clone->m_actionType = m_actionType;
	switch (m_actionType)
	{
	case 1: clone->m_actionCall = CC_CALLBACK_1(Skill::actionType1, clone); break;
	case 2: clone->m_actionCall = CC_CALLBACK_1(Skill::actionType2, clone); break;
	case 3: clone->m_actionCall = CC_CALLBACK_1(Skill::actionType3, clone); break;
	case 4: clone->m_actionCall = CC_CALLBACK_1(Skill::actionType4, clone); break;
	}
	for (Node* bullet : getChildren()) {
		clone->addChild(((Bullet*)bullet)->clone());
	}

	clone->scheduleUpdate();

	return clone;
}

void Skill::setSpeed(float speed)
{
	//m_speed = speed;
	for (Node* bullet : getChildren()) {
		((Bullet*)bullet)->setSpeed(speed);
	}
}

void Skill::setDamage(float damage)
{
	//m_damage = damage;
	for (Node* bullet : getChildren()) {
		((Bullet*)bullet)->setDamage(damage);
	}
}

void Skill::update(float dt)
{
	m_actionCall(dt);
}

void Skill::setScale(float scale)
{
	for (Node* bullet : getChildren()) {
		bullet->setScale(scale);
	}
}

void Skill::Init(const char *name, Vec2 pos, int range)
{
	rapidjson::Document doc;
	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("moves/_config.json").c_str());

	Rect rect;
	rect.setRect(doc[name]["rectParam"][0].GetInt(),
		doc[name]["rectParam"][1].GetInt(),
		doc[name]["rectParam"][2].GetInt(),
		doc[name]["rectParam"][3].GetInt());

	bool isMovable = doc[name].HasMember("isMovable") ? doc[name]["isMovable"].GetBool() : false;
	int bulletAmount = doc[name].HasMember("amount") ? doc[name]["amount"].GetInt() : 1;
	float delay = doc[name].HasMember("delay") ? doc[name]["delay"].GetDouble() : 0;
	m_actionType = doc[name].HasMember("actionType") ? doc[name]["actionType"].GetInt() : 1;
	
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("moves/28883.png");
	std::vector<Rect> rectList;
	int frameCount = (int)(rect.size.height / rect.size.width);
	rect.setRect(rect.getMinX(), rect.getMinY(),
		rect.size.width, (int)rect.size.height / frameCount);

	for (int i = 0; i < frameCount; i++)
	{
		rectList.push_back(Rect(rect));
		rect.setRect(rect.getMinX(), rect.getMaxY(),
			rect.size.width, rect.size.height);
	}

	//should create one bullet and clone it, will do in future!!!
	bool isVertical = m_actionType > 2;
	bool isMovingWithAnimation = m_actionType == 4;
	for (int i = 1; i <= bulletAmount; i++) {
		auto bullet = Bullet::create(texture, Vec2::ZERO, rectList);
		if (bullet) {
			bullet->setMovable(isMovable);
			bullet->setVertical(isVertical);
			bullet->setMovingWithAnimation(isMovingWithAnimation);
			if (isMovingWithAnimation){
				bullet->setDelay(delay);
				bullet->setDuration(delay / bulletAmount * i * 5);
			}
			else{
				bullet->setDelay(delay / bulletAmount * i);
			}	
			addChild(bullet);
		}
	}
	
	
	//m_bulletList.at(0)->addAnimation("moves/28883.png", frameCount, rect);
	//AnimatedObject::Init(name, pos);
	
	setScale(doc[name]["scale"].GetDouble());
	setTag(SKILL_TAG);
	setVisible(false);
	//m_damage = m_speed = 0;
	m_range = range;
}

void Skill::actionType1(float dt)
{
	for (Node* node : getChildren()) {
		auto bullet = static_cast<Bullet*>(node);
		auto target = bullet->getTarget();
		if (target) {
			if (target->getTag() != ENEMY_TAG || !target->isVisible() ||
				getParent()->getPosition().getDistance(target->getPosition()) > m_range) {
				bullet->setTarget(nullptr);
			}
		}
		else {
			// find enemy
			Vector<Node*> nodeList = getParent()->getParent()->getChildren();
			for (Node* node : nodeList) {
				if (node->getTag() == ENEMY_TAG && node->isVisible() && isVisible() &&
					getParent()->getPosition().getDistance(node->getPosition()) < m_range) {
					bullet->setTarget(node);
					break;
				}
			}
		}
		bullet->doTargetAction(dt);
	}
}

void Skill::actionType2(float dt)
{
	float range = m_range;
	double k = 6.28 / getChildren().size();
	int i = 0;
	for (Node* node : getChildren()) {
		auto bullet = static_cast<Bullet*>(node);
		if (bullet->doMovingAction(dt) && isVisible()) {
			bullet->setPath(Vec2::ZERO, Vec2(cos(k*i), sin(k*i))*range);
		}
		i++;
	}
}

void Skill::actionType3(float dt)
{
	float range = m_range - 30;
	for (Node* node : getChildren()) {
		auto bullet = static_cast<Bullet*>(node);
		if (bullet->doMovingAction(dt) && isVisible()){
			Vec2 t = Vec2(random(-range, range), random(-range, range));
			t = t * RANGEPERCENT + t.getNormalized() * 30;
			bullet->setPath(t, t);
		}
	}
}

void Skill::actionType4(float dt)
{
	for (Node* node : getChildren()) {
		auto bullet = static_cast<Bullet*>(node);
		if (bullet->doCircleAction(dt) && isVisible()){
			Vec2 t = Vec2(random(-m_range, m_range), random(-m_range, m_range));
			t = t * RANGEPERCENT;
			bullet->setPath(t - Vec2(100,0), t);
		}
	}
}
