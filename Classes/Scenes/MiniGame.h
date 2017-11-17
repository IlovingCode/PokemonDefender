#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class MiniGame : public Scene
{
private:
	int* m_array;
	int m_selected1, m_selected2;
	Sprite *m_cursor1, *m_cursor2;

public:
	CREATE_FUNC(MiniGame);
	void onExit() override;

private:
	virtual bool init();

	void StartEvent();
	void TouchEvent(Ref*, Widget::TouchEventType);
	void Process();
};
