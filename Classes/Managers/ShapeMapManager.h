#include "Components/ShapeMap.h"

#define HALFHEIGHT	 Director::getInstance()->getVisibleSize().height / 3

class ShapeMapManager : public Node
{
private:
	ShapeMap*	m_shapeMapHead;
	float		m_speed;
	float		m_vMax;
	float		m_aMax;
	float		m_h;
	float		m_v;
	float		m_a;

	void	loadMap(const char*);

public:
	CREATE_FUNC(ShapeMapManager);

	void	update(float) override;
	void	Init(const char*);
	float	getAngle();
	void	jump();
};