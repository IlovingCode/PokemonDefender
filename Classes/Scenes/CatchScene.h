#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class Tower;
class PokeBall;

class CatchScene : public Scene
{
private:

public:
	CREATE_FUNC(CatchScene);
	Tower*		m_pokemon;
	PokeBall*	m_pokeBall;

private:
	virtual bool init();

	void StartEvent();
	void TouchEvent(Ref*, Widget::TouchEventType);
	void Process();
};
