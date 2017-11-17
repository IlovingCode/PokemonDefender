#include "Battle.h"
#include "Tower.h"
#include "Enemy.h"
#include "json/document.h"

static Battle* m_instance = nullptr;

Battle* Battle::getInstance()
{
	if (!m_instance){
		m_instance = new Battle;
	}
	return m_instance;
}

void Battle::destroyInstance()
{
	CC_SAFE_DELETE(m_instance);
}

bool Battle::isPlaceable(int x, int y)
{
	return x >= 0 && x < m_width &&
		y >= 0 && y < m_height &&
		!m_signalMap[x][y];
}

bool Battle::isPlaceable(Vec2 pos)
{
	return isPlaceable((int)pos.x / DPC, (int)pos.y / DPC);
}

Node* Battle::getEnemy(int i)
{
	if (i < 0 || i >= m_enemyList.size())
		return nullptr;
	//return Enemy::create(m_enemyList.at(i)->getName().c_str(), getBegin(), getPath());
	return m_enemyList.at(i)->clone(getBegin(), getPath());
}

Node * Battle::getTower(int i, Vec2 pos)
{
	Vec2 t = pos / DPC;
	if (isPlaceable((int)t.x, (int)t.y))
	{
		m_signalMap[(int)t.x][(int)t.y] = 1;
		pos = standardizePosition(pos);
		//return Tower::create(m_towerList.at(i)->getName().c_str(), pos);
			return m_towerList.at(i)->clone(pos);
	}
	return nullptr;
}

Tower* Battle::getRootTower(int id)
{
	if (id >= 0 && id < m_towerList.size()) {
		return m_towerList.at(id);
	}
	return nullptr;
}

void Battle::removeTower(Node * tower)
{
	if (tower) {
		Vec2 t = tower->getPosition() / DPC;
		if (t.x >= 0 && t.x < m_width && t.y >= 0 && t.y < m_height) {
			m_signalMap[(int)t.x][(int)t.y] = 0;
		}
		tower->removeFromParent();
	}
}

void Battle::upgradeTower(Node *tower)
{
	if (tower && tower->getTag() == TOWER_TAG) {
		((Tower*)tower)->upgrade();
	}
}

void Battle::init(const char *filename, vector<string> pokemonList)
{
	rapidjson::Document doc;
	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile(filename).c_str());

	m_background = doc["background"].GetString();
	//m_bossName = doc.HasMember("bossName") ? doc["bossName"].GetString() : "";
	m_begin.setPoint(doc["beginX"].GetDouble(), doc["beginY"].GetDouble());

	for (string str : pokemonList) {
		auto tower = Tower::create(str.c_str());
		if (tower) m_towerList.pushBack(tower);
	}

	m_count = doc["count"].GetUint();
	m_round = doc["round"].GetUint();

	for (int i = 0; i < doc["enemy"].Size(); i++)
		m_enemyList.pushBack(Enemy::create(doc["enemy"][i].GetString()));

	m_bossName = m_enemyList.back()->getName();

	for (int i = 0; i < doc["path"].Size(); i++)
		m_pathList.push_back(doc["path"][i].GetString());

	processPath();
}

void Battle::processPath()
{
	Vec2 pivot;
	m_signalMap[(int)m_begin.x][(int)m_begin.y] = 1;
	for (string path : m_pathList) {
		pivot = m_begin;
		for (int i = 1; i < path.length(); i+=2) {
			for (int j = 0; j < (path[i - 1] - '0'); j++) {
				switch (path[i])
				{
				case 'u': pivot.y++; break;
				case 'd': pivot.y--; break;
				case 'l': pivot.x--; break;
				case 'r': pivot.x++; break;
				}
				m_signalMap[(int)pivot.x][(int)pivot.y] = 1;
			}
			
		}
	}
}

void Battle::reset()
{
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			m_signalMap[i][j] = (i == 0 || j == 0 
								|| i == m_width - 1 
								|| j == m_height - 1);
		}
	}
}

Vec2 Battle::standardizePosition(Vec2 pos)
{ // DPC = 40
	Vec2 result;
	result.x = (int)pos.x / DPC;
	result.y = (int)pos.y / DPC;
	result.x += 0.5f;
	return result * DPC;
}

Battle::Battle()
{
	m_height = 12;
	m_width = 20;
}


Battle::~Battle()
{
	for (int i = 0; i < m_pathList.size(); i++) {
		m_pathList[i].clear();
	}

	m_pathList.clear();
	m_background.clear();
	m_towerList.clear();
	m_enemyList.clear();
}
