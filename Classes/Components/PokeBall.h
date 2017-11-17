#include "AnimatedObject.h"
#include "Components/Tower.h"

class PokeBall : public AnimatedObject
{
private:
	float	m_speed;
	bool	m_isActive;

	Vec2	m_origin;
	Size	m_vibrate;
	Tower*	m_target;

	~PokeBall() {};
	CREATE_FUNC(PokeBall);

	void moveTo(float dt, Vec2);

protected:
	void Init(const char*, Vec2) override;
	void update(float) override;

public:
	static PokeBall*	create(const char*, Vec2);
	PokeBall*			clone();
	void				setTarget(Tower*);
	void				setSpeed(float);
	void				onTouch(Vec2);
};
