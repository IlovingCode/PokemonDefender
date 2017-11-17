#include "cocos2d.h"
#include "json/document.h"

USING_NS_CC;

class ShapeMap: public Sprite
{
private:
	float*	m_heightMap;
	float*	m_rotationMap;
	bool	m_isHead;
	float	m_alignHeight;

public:
	ShapeMap*	m_next;

	CREATE_FUNC(ShapeMap);
	~ShapeMap();

	static	ShapeMap*	create(const char*);
	void	load(const char*);
	bool	Update(float);
	float	getAngle();
	float	getBeginHeight();
	float	getEndHeight();
	void	setHead(bool);
	void	setHeight(float);
	void	setPositionBelong2PreviousMap(ShapeMap*);
};