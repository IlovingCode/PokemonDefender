#include "ShapeMapManager.h"


void ShapeMapManager::loadMap(const char* filename)
{
	rapidjson::Document doc;

	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile(filename).c_str());

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 pos = Vec2(visibleSize.width - 1, 0);
	m_speed = doc["speed"].GetInt();
	m_vMax = doc["v"].GetDouble();
	m_aMax = doc["a"].GetDouble();
	m_v = m_a = m_h = 0;

	ShapeMap* shapeMap = nullptr;
	int count = doc["mapList"].Size();
	for (int i = 0; i < count; i++){
		if (shapeMap){
			shapeMap->m_next = ShapeMap::create(doc["mapList"][i].GetString());
			shapeMap->m_next->setPositionBelong2PreviousMap(shapeMap);
			shapeMap = shapeMap->m_next;
		}
		else{
			shapeMap = ShapeMap::create(doc["mapList"][i].GetString());
			m_shapeMapHead = shapeMap;
		}

		shapeMap->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		shapeMap->setHeight(HALFHEIGHT);
		addChild(shapeMap);
	}
	shapeMap->m_next = m_shapeMapHead;
	m_shapeMapHead->setHead(true);
}

void ShapeMapManager::update(float dt)
{
	if (m_shapeMapHead->Update((int)(m_speed*dt))) {
		m_shapeMapHead->setHead(false);
		m_shapeMapHead = m_shapeMapHead->m_next;
		m_shapeMapHead->setHead(true);
		m_shapeMapHead->setHeight(HALFHEIGHT);
	}
	if (m_a) {
		m_v += m_a*dt;
		m_h -= m_v*dt;
		if (m_h > HALFHEIGHT) {
			m_h = HALFHEIGHT;
			m_a = 0;
			m_v = 0;
		}
		m_shapeMapHead->setHeight((int)m_h);
	}
}

void ShapeMapManager::Init(const char* filename)
{
	m_shapeMapHead = nullptr;
	loadMap(filename);

	if (m_shapeMapHead)
		m_shapeMapHead->Update(0);
}

float ShapeMapManager::getAngle()
{
	if (!m_shapeMapHead || m_a)
		return -15;
	return m_shapeMapHead->getAngle();
}

void ShapeMapManager::jump()
{
	if (m_a) {
		return;
	}
	m_a = -m_aMax;
	m_v = m_vMax;
	m_h = HALFHEIGHT;
}
