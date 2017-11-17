#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class RequirementArray;

class Cursor
{
private:
	Node* m_range;
	Widget* m_panel;
	Node* m_tower;
	bool m_IsControlPanelShown;

	void showControlPanel(bool);
	void onSell();
	void onUpgrade();

	Cursor();
	~Cursor();
public:
	static Cursor* getInstance();
	static void destroyInstance();

	void Init(Widget*);
	void onBegin(Node*, Vec2, int, int = 0, RequirementArray* = nullptr);
	void onMove(Vec2, bool);
	void onEnd();
	bool isVisible();
};

