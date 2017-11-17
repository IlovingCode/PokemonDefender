#include "Enemy.h"

Enemy* Enemy::create(const char* name, Vec2 pos, std::string path)
{
	//char str[50] = { 0 };
	//sprintf(str, "enemies/_config/%s.json", name);
	//if (!cocos2d::FileUtils::getInstance()->isFileExist(str))
		//return nullptr;

	auto ret = create();
	ret->Init(name, pos);
	ret->m_path = path;

	return ret;
}

Enemy * Enemy::clone(Vec2 pos, std::string path)
{
	auto clone = Enemy::create();

	clone->cloneProperties(this);

	clone->initHealthBar();

	clone->m_itemList = m_itemList;
	clone->m_baseSpeed = m_baseSpeed;
	clone->m_maxHealth = m_maxHealth;
	clone->m_currentHealth = m_currentHealth;
	clone->m_pathId = m_pathId;
	clone->m_path = m_path;
	clone->m_goal = m_goal;

	if (pos != Vec2::ZERO) {
		clone->m_goal = pos*DPC;
		clone->setPosition(clone->m_goal);
		clone->setLocalZOrder(-getPosition().y);
	}
	if (!path.empty()) {
		clone->m_path = path;
		clone->m_pathId = 1;
	}

	return clone;
}

void Enemy::update(float delta)
{
	if (getTag() != ENEMY_TAG) {
		if(!isVisible() || !getChildren().front()->getOpacity()) removeFromParent();
		return;
	}
	if (getPosition() == m_goal) {
		if (m_pathId < m_path.length()) {
			switch (m_path[m_pathId])
			{
			case'u':m_goal.y += DPC*(m_path[m_pathId - 1] - '0');
				m_direction = Vec2(0, 1); setDirection(Direction::UP);
				break;
			case'd':m_goal.y -= DPC*(m_path[m_pathId - 1] - '0');
				m_direction = Vec2(0, -1); setDirection(Direction::DOWN);
				break;
			case'l':m_goal.x -= DPC*(m_path[m_pathId - 1] - '0');
				m_direction = Vec2(-1, 0); setDirection(Direction::LEFT);
				break;
			case'r':m_goal.x += DPC*(m_path[m_pathId - 1] - '0');
				m_direction = Vec2(1, 0); setDirection(Direction::RIGHT);
				break;
			}
			m_pathId += 2;
		}
		else {
			setVisible(false);
			finish();
		}
	}
	else {
		auto speed = m_baseSpeed*delta;
		Vec2 temp = getPosition() + m_direction*speed;
		if (abs(temp.x - m_goal.x) < speed && abs(temp.y - m_goal.y) < speed) {
			setPosition(m_goal);
		}
		else {
			setPosition(temp);
		}
		setLocalZOrder(-getPosition().y);
	}
}

void Enemy::setDirection(Direction d)
{
	switch (d)
	{
	case Direction::UP: runAnimate(2); break;
	case Direction::DOWN: runAnimate(0); break;
	case Direction::LEFT: runAnimate(1); break;
	case Direction::RIGHT: runAnimate(1, true); break;
	}
}

void Enemy::Init(const char *name, Vec2 pos)
{
	char str[50] = { 0 };
	rapidjson::Document doc, data;
	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("enemies/_config.json").c_str());
	doc.Swap(doc[name]);

	data.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile(doc["rectInfo"].GetString()).c_str());
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(doc["texture"].GetString());
	
	for (int i = 0; i < doc["moveAnim"].Size(); i++) {
		std::vector<Rect> rectList;
		int frameCount = doc["moveAnim"][i].Size();
		for (int j = 0; j < frameCount; j++) {
			sprintf(str, "Rect%d", doc["moveAnim"][i][j].GetInt());
			rectList.push_back(Rect(data[str][0].GetInt(), data[str][1].GetInt(),
				data[str][2].GetInt(), data[str][3].GetInt()));
		}
		addAnimation(texture, rectList, true);
	}

	AnimatedObject::Init(name, pos*DPC);

	//initHealthBar();
	m_healthBar = nullptr;
	m_goal = pos*DPC;
	m_baseSpeed = doc["speed"].GetInt();
	m_maxHealth = doc["HP"].GetInt();
	m_currentHealth = m_maxHealth;
	m_pathId = 1;
	m_itemList = new ResourcePack;
	m_itemList->push_back(std::make_pair("energy", 5));
	setTag(ENEMY_TAG);
}

void Enemy::initHealthBar()
{
	/*auto sprite = Sprite::create("others/healthbar.png");
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	sprite->setPositionY(getBound().size.height);
	addChild(sprite);*/

	m_healthBar = Sprite::create("others/healthbar1.png");
	m_healthBar->setPositionY(getBoundingBox().size.height);
	m_healthBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_healthBar->setColor(Color3B(0, 150, 0));
	addChild(m_healthBar);
}

void Enemy::takeDamage(float damage)
{
	if (!m_healthBar) return;

	m_currentHealth -= damage;
	if (m_currentHealth <= 0) {
		//death
		m_healthBar->removeFromParent();
		m_healthBar = nullptr;
		runSpriteAction(FadeOut::create(0.7f));
		finish();
	}else {
		m_healthBar->setScaleX(m_currentHealth/ m_maxHealth);
	}
}

void Enemy::finish()
{
	if (isVisible()) {
		getEventDispatcher()->dispatchCustomEvent("countDown", m_itemList);
	}
	else {
		getEventDispatcher()->dispatchCustomEvent("countDown");
	}
	
	setTag(0);
}
