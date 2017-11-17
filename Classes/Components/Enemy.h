#include "AnimatedObject.h"

#define	ENEMYFRAMECOUNT		3
#define DPC					40
#define	RESOURCEPACK_ENERGY	0

typedef std::vector<std::pair<const char*, int>> ResourcePack;

class Enemy : public AnimatedObject
{
private:
	std::string m_path;
	int m_pathId;
	float m_baseSpeed, m_currentHealth, m_maxHealth;
	Vec2 m_goal, m_direction;
	Sprite* m_healthBar;
	ResourcePack* m_itemList;

	~Enemy() { if (m_path.empty())CC_SAFE_DELETE(m_itemList); }
	CREATE_FUNC(Enemy);

protected:
	void Init(const char*, Vec2) override;
	void initHealthBar();
	void setDirection(Direction);
	void update(float) override;
	void finish();

public:
	static Enemy* create(const char*, Vec2 = Vec2::ZERO, std::string = "");
	Enemy* clone(Vec2 = Vec2::ZERO, std::string = "");
	void takeDamage(float);
};

