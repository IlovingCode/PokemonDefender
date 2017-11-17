/* deprecated
#include "SkiingGame.h"
#include "SkiingGame/Tree.h"
#include "editor-support/cocostudio/CCSGUIReader.h"

#define RANGE	30


void SkiingGame::update(float dt)
{
	m_textController.update(dt);
}

bool SkiingGame::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = ImageView::create("skiinggame/tree_bg.jpg");
	background->setPosition(visibleSize / 2);
	background->setLocalZOrder(-500);
	addChild(background, -visibleSize.height);

	m_ground = Sprite::create("skiinggame/ground.png");
	m_ground->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_ground->setPosition(Vec2(400, 0));
	m_ground->setLocalZOrder(-240);
	addChild(m_ground);

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/SkiingGame.json");

	Helper::seekWidgetByTag(widget, 2)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, 3)->addClickEventListener(CC_CALLBACK_0(SkiingGame::RightEvent, this));
	Helper::seekWidgetByTag(widget, 4)->addClickEventListener(CC_CALLBACK_0(SkiingGame::LeftEvent, this));

	Text* text = static_cast<Text*>(Helper::seekWidgetByTag(widget, 110));
	m_textController.initText(text);
	m_score = 0;

	addChild(widget, 500);
	scheduleUpdate();
	StartEvent();

	return true;
}

void SkiingGame::StartEvent()
{
	rapidjson::Document doc;

	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("skiinggame/map.json").c_str());
	int y = doc["y"].GetInt();
	int count = doc["milestones"].Size();
	doc.Swap(doc["milestones"]);
	Vec3 temp;

	Tree* tree = Tree::create("tree", Vec2::ZERO);

	for (int i = 0; i < count; i++) {
		temp.x = doc[i][0].GetInt() + 400;
		temp.y = y;
		temp.z = doc[i][1].GetInt();

		m_ground->addChild(tree->clone(temp + Vec3(RANGE, 0 ,0))); // right
		m_ground->addChild(tree->clone(temp - Vec3(RANGE, 0, 0))); // left
	}

	//CC_SAFE_DELETE(tree);
}

void SkiingGame::RightEvent()
{
	m_ground->setPositionX(m_ground->getPositionX() - 3);
	m_score += 100;
	m_textController.setTextInt(m_score);
}

void SkiingGame::LeftEvent()
{
	m_ground->setPositionX(m_ground->getPositionX() + 3);
	m_score += 100;
	m_textController.setTextInt(m_score);
}
*/
