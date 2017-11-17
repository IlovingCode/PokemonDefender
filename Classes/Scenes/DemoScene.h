#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "json/document.h"

USING_NS_CC;
using namespace ui;

class DemoScene : public Scene
{
private:
	Text *m_round;
	Sprite *m_sprite;
	int m_id;
	bool m_bpause;
	rapidjson::Document m_doc;

public:
	CREATE_FUNC(DemoScene);

private:
	virtual bool init();

	void StartEvent();
	void SkipEvent();
	void BackEvent();
	void ClickEvent(float);
	void PauseEvent(Ref *);
};

