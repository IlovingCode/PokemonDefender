#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "json/document.h"

USING_NS_CC;
using namespace ui;

class ShapeMapManager;

class SkiingGame : public Scene
{
private:
	ShapeMapManager* m_shapeMapManager;
	Sprite* m_character;

public:
	CREATE_FUNC(SkiingGame);

private:
	virtual bool init();
	void update(float) override;
	void updateCharacterPosition();

	void StartEvent();
	void ClickEvent();
};
