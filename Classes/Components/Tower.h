#include "AnimatedObject.h"

class Skill;
class RequirementArray;

class Tower : public AnimatedObject
{
private:
	float				m_range, m_damage, m_speed;
	int					m_price;
	RequirementArray*	m_requirementList;
	Tower*				m_nextGrade;
	Node*				m_target;
	Skill*				m_move;

	~Tower();
	CREATE_FUNC(Tower);

protected:
	void		setDirection(Direction, bool);
	void		update(float) override;
	Direction	getDirectionTo(Vec2);
	void		addTowerAnimation(const char*, int, int);
	void		Init(const char*, Vec2) override;
	bool		upgradeable();

public:
	static Tower*	create(const char*, Vec2 = Vec2::ZERO);
	Tower*			clone(Vec2 = Vec2::ZERO);
	float			getRange() { return m_range; }
	int				getPrice() { return m_price; }
	std::string		getEvolutionStone();
	void			onSelect(Vec2);
	void			onDeselect();
	void			onSpawn();
	void			upgrade();
};

