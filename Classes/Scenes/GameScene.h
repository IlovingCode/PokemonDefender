#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class GameScene : public Scene
{
private:
	bool m_bpaused, m_bspeed;
	int m_count, m_selectedTowerIndex;
	int m_energyRemain;
	char m_heartRemain;
	ImageView* m_heart, *m_battery;
	Text* m_round;

public:
	CREATE_FUNC(GameScene);
	void onExit() override;

private:
    virtual bool init();

	void refreshTower();
	void applyShader(Widget*, const char*, const char*);
	Node* getWigetSprite(Widget*);
	void setUniformAttribWidget(Widget*, const char*, float);
	void setUniformAttribWidget(Widget*, const char*, Vec4);
	void waveCall(float);
	void enemyCall(float);
	void SelectTowerEvent(Ref*, Widget::TouchEventType);
	void PauseEvent(Ref*);
	void SpeedEvent(Ref*);
	void TouchEvent(Ref*, Widget::TouchEventType);
	void countDown(EventCustom*);
};
