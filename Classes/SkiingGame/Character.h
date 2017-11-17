#include "Components/AnimatedObject.h"


class Character : public AnimatedObject
{
public:
	static Character* create(const char*, Vec2);
	Character* clone(Vec3);

protected:
	Vec3 m_speed;

	CREATE_FUNC(Character);
	void update(float) override;
	void Init(const char*, Vec2) override;
};