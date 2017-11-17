#include "Components/AnimatedObject.h"


class Tree : public AnimatedObject
{
public:
	static Tree* create(const char*, Vec2);
	Tree* clone(Vec3);

protected:
	Vec3 m_speed;

	CREATE_FUNC(Tree);
	void update(float) override;
	void Init(const char*, Vec2) override;
};