#include "cocos2d.h"
#define RAND_RANGE 0.35f

USING_NS_CC;
using namespace std;

class Tower;
class Enemy;

class Battle
{
private:
	Vector<Tower*> m_towerList;
	Vector<Enemy*> m_enemyList;
	vector<string> m_pathList;
	Vec2 m_begin;
	string m_background;
	string m_bossName;
	int m_width, m_height;
	unsigned int m_count, m_round;
	int m_signalMap[20][12];
public:
	int getTowerListSize() { return m_towerList.size(); }
	int getEnemyListSize() { return m_enemyList.size(); }
	string getMap() { return m_background; }
	string getBossName(){ return m_bossName; }
	string getPath() { return m_pathList[random(0, (int)m_pathList.size() - 1)]; }
	Vec2 getBegin() { return m_begin + Vec2(random(-RAND_RANGE, RAND_RANGE), 
												random(-RAND_RANGE, RAND_RANGE) - 0.15f); }
	bool isPlaceable(int x, int y);
	bool isPlaceable(Vec2);
	Vec2 standardizePosition(Vec2);
	Node* getEnemy(int = 0);
	Node* getTower(int, Vec2);
	Tower* getRootTower(int);
	void removeTower(Node*);
	void upgradeTower(Node*);
	unsigned int getCount() { return m_round-- > 1 ? m_count : 0; }
	unsigned int getRound() { return m_round; }
	void init(const char*, vector<string>);
	inline void processPath();
	void reset();
	static Battle* getInstance();
	static void destroyInstance();

private:
	Battle();
	~Battle();
};