#include "cocos2d.h"

USING_NS_CC;

class MainMenu : public Scene
{
public:
	CREATE_FUNC(MainMenu);
	void onEnter() override;

private:
	bool m_isInputLocked;

	virtual bool init();

	void StartEvent();
	void ExitEvent();
	void MiniGameEvent();
	void DemoEvent();
};
