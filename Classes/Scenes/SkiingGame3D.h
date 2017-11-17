/* deprecated
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "json/document.h"
#include "Components/TextController.h"

USING_NS_CC;
using namespace ui;

class SkiingGame : public Scene
{
private:
	Sprite* m_ground;
	TextController m_textController;

	int m_score;

public:
	CREATE_FUNC(SkiingGame);
	void update(float) override;

private:
	virtual bool init();

	void StartEvent();
	void RightEvent();
	void LeftEvent();
};
*/
