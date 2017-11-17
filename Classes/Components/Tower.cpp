#include "Tower.h"
#include "Skill.h"
#include "Cursor.h"
#include "Requirement.h"

Tower::~Tower() {
	if (m_requirementList && !m_nextGrade) // just delete if object is root tower
		CC_SAFE_DELETE(m_requirementList);
}

Direction Tower::getDirectionTo(Vec2 target)
{
	Vec2 t = target - getPosition();
	if (t.x > 0) {
		if (t.y > 0) {
			return Direction::BACK_RIGHT;
		}
		else {
			return Direction::FRONT_RIGHT;
		}
	}
	else {
		if (t.y > 0) {
			return Direction::BACK_LEFT;
		}
		else {
			return Direction::FRONT_LEFT;
		}
	}
}

inline std::string convertTypeToStone(std::string type)
{
	if (type == "fire") return "stoneFire";
	if (type == "water") return "stoneWater";
	if (type == "electric") return "stoneThunder";
	if (type == "grass") return "stoneLeaf";
	if (type == "spychic") return "stoneMoon";
	return "stoneMoon";
}

std::string Tower::getEvolutionStone()
{
	return convertTypeToStone(m_type);
}

void Tower::onSelect(Vec2 pos)
{
	if (isVisible()) {
		if (getBoundingBox().containsPoint(pos)) {
			Vec2 t = getPosition();
			t.y += getBoundingBox().size.height / 2;
			// cheat :(
			getChildren().at(1)->setGlobalZOrder(1000);
			Cursor::getInstance()->onBegin(this, t, m_range, m_price / 2, m_requirementList);
		}
	}
	else {
		setPositionY(getBoundingBox().size.height / 2);
		Cursor::getInstance()->onBegin(this, pos, m_range);
	}
}

void Tower::onDeselect()
{
	// cheat :(
	getChildren().at(1)->setGlobalZOrder(0);
}

void Tower::onSpawn()
{
	float scale = getScale();
	setScale(0);
	runAction(EaseBackOut::create(ScaleTo::create(1.5f, scale)));
}

void Tower::upgrade()
{
	// to do
	if (upgradeable()) {
		getParent()->addChild(m_nextGrade->clone(getPosition()));
		removeFromParent();
	}
}

std::vector<Rect> getRectList(int col, int row, int frameCount, Rect& rect)
{
	std::vector<Rect> rectList;

	for (int i = rect.getMinY() / rect.size.height; i < row; i++)
	{
		for (int j = rect.getMinX() / rect.size.width; j < col; j++)
		{
			rectList.push_back(Rect(rect));
			rect.setRect(rect.getMaxX(), rect.getMinY(),
				rect.size.width, rect.size.height);
			if (!frameCount--)
				return rectList;
		}
		rect.setRect(0, rect.getMaxY(),
			rect.size.width, rect.size.height);
	}
	return rectList;
}

void Tower::addTowerAnimation(const char* textureFilePath, int frameWidth, int frameHeight)
{
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(textureFilePath);
	Size size = texture->getContentSizeInPixels();
	Rect rect(0, 0, frameWidth, frameHeight);
	int col = size.width / rect.size.width;
	int row = size.height / rect.size.height;
	int frameCount = col * row / 2;

	addAnimation(texture, getRectList(col, row, frameCount, rect), true); // idle
	addAnimation(texture, getRectList(col, row, frameCount, rect), true); // attack
}

void Tower::Init(const char *name, Vec2 pos)
{
	char str[50] = { 0 };
	sprintf(str, "towers/_config/%s.json", name);

	rapidjson::Document doc;
	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile(str).c_str());

	sprintf(str, "towers/%s/front.png", name);
	addTowerAnimation(str, doc["frontFrame"][0].GetInt(), doc["frontFrame"][1].GetInt());
	sprintf(str, "towers/%s/back.png", name);
	addTowerAnimation(str, doc["backFrame"][0].GetInt(), doc["backFrame"][1].GetInt());

	AnimatedObject::Init(name, pos);

	setTag(TOWER_TAG);
	//setDirection(Direction::FRONT_LEFT);
	//double scale = 60.0f / doc["frontFrame"][0].GetInt();
	setScale(doc["scale"].GetDouble());

	m_range = doc["range"].GetDouble();
	m_speed = doc["speed"].GetDouble();
	m_damage = doc["damage"].GetDouble();
	m_type = doc["type"].GetString();
	m_price = doc["price"].GetInt();

	m_move = Skill::create(doc["skill"].GetString(), m_range);
	if (m_move){
		m_move->setSpeed(m_speed);
		m_move->setDamage(m_damage);
		addChild(m_move);
	}

	if (doc.HasMember("nextGrade")) {
		auto tower = Tower::create(doc["nextGrade"].GetString());
		if (tower) addChild(tower);
	}

	if (doc.HasMember("requirement")) {
		m_requirementList = new RequirementArray(doc["requirement"]);
	}
	else {
		m_requirementList = nullptr;
	}

	m_target = nullptr;
	m_nextGrade = nullptr;
}

bool Tower::upgradeable()
{
	bool isMeetRequirement = true;

	// check requirement
	

	return m_nextGrade && isMeetRequirement;
}

Tower* Tower::create(const char* name, Vec2 pos)
{
	char str[50] = { 0 };
	sprintf(str, "towers/_config/%s.json", name);
	if (!cocos2d::FileUtils::getInstance()->isFileExist(str))
		return nullptr;

	auto ret = create();
	ret->Init(name, pos);

	return ret;
}

Tower * Tower::clone(Vec2 pos)
{
	auto clone = Tower::create();

	clone->cloneProperties(this);

	clone->m_range = m_range;
	clone->m_speed = m_speed;
	clone->m_damage = m_damage;
	clone->m_target = m_target;
	clone->m_nextGrade = m_nextGrade;
	clone->m_type = m_type;
	clone->m_price = m_price;
	clone->m_requirementList = m_requirementList;

	clone->m_move = m_move->clone();
	clone->addChild(clone->m_move);

	if (pos != Vec2::ZERO) {
		clone->setPosition(pos);
		clone->setLocalZOrder(-pos.y);
	}

	if (!m_nextGrade && getChildren().back()->getTag() == TOWER_TAG)
		clone->m_nextGrade = (Tower*)getChildren().back();

	if (!clone->getAnimate()) clone->setDirection(Direction::FRONT_LEFT, false);

	return clone;
}

void Tower::setDirection(Direction d, bool isAttacking)
{
	switch (d)
	{
	case Direction::FRONT_LEFT: runAnimate(0 + isAttacking); break;
	case Direction::BACK_LEFT: runAnimate(2 + isAttacking, true); break;
	case Direction::FRONT_RIGHT: runAnimate(0 + isAttacking, true); break;
	case Direction::BACK_RIGHT: runAnimate(2 + isAttacking); break;
	}
}

void Tower::update(float)
{
	if (m_target) {
		if (m_target->getTag() != ENEMY_TAG || !m_target->isVisible() ||
			getPosition().getDistance(m_target->getPosition()) > m_range) {
			setDirection(getDirectionTo(m_target->getPosition()), false);
			m_target = nullptr;
			m_move->setVisible(false);
			return;
		}
		setDirection(getDirectionTo(m_target->getPosition()), true);
	}
	else {
		// find enemy
		Vector<Node*> nodeList = getParent()->getChildren();
		for (Node* node : nodeList) {
			if (node->getTag() == ENEMY_TAG && node->isVisible() &&
				getPosition().getDistance(node->getPosition()) < m_range) {
				m_target = node;
				m_move->setVisible(true);
				break;
			}
		}
	}
}
