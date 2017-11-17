#include "AnimatedObject.h"

typedef std::function<void(float)> actionCall;

class Skill : public Node
{
private:
	std::string m_type;
	float m_range;
	int m_actionType;
	actionCall m_actionCall;

	void actionType1(float);
	void actionType2(float);
	void actionType3(float);
	void actionType4(float);

	~Skill() {}
	CREATE_FUNC(Skill);
	void update(float) override;
	void setScale(float) override;
	//float getScale() const override;
	void Init(const char*, Vec2, int);

public:
	static Skill* create(const char*, int);
	Skill* clone();
	void setSpeed(float);
	void setDamage(float);
};

