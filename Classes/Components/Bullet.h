#include "AnimatedObject.h"

class Bullet : public AnimatedObject
{
private:
	Node* m_target;
	float m_speed, m_damage, m_delay, m_duration;
	bool m_isMovable, m_isVertical, m_isMovingWithAnimation;

	Vec2 m_goal;

	~Bullet();
	CREATE_FUNC(Bullet);

	bool isAnimateDone(Animate*);
	void dealDamageInRange(float);
	void moveTo(float dt, Vec2);

protected:
	void Init(Texture2D*, Vec2, std::vector<Rect>);

public:
	static Bullet* create(Texture2D*, Vec2, std::vector<Rect>);
	Bullet* clone();
	void setSpeed(float);
	void setDamage(float);
	void setTarget(Node*);
	Node* getTarget();
	void setDelay(float);
	void setDuration(float);
	void setPath(Vec2, Vec2);
	void setMovingWithAnimation(bool);
	void setMovable(bool);
	void setVertical(bool);

	bool doTargetAction(float);
	bool doCircleAction(float);
	bool doMovingAction(float);

};

