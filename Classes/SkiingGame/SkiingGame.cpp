#include "SkiingGame.h"
#include "SkiingGame/Tree.h"
#include "Managers/ShapeMapManager.h"
#include "editor-support/cocostudio/CCSGUIReader.h"


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

	auto background = Sprite::create("skiinggame/tree_bg.jpg");
	background->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	background->setPosition(Vec2(visibleSize.width / 2, 0));
	addChild(background);

	// load map
	m_shapeMapManager = ShapeMapManager::create();
	m_shapeMapManager->Init("skiinggame/_config.json"); 
	addChild(m_shapeMapManager);

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("skiinggame/SkiingGame.json");

	Helper::seekWidgetByTag(widget, 2)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, 0)->addClickEventListener(CC_CALLBACK_0(SkiingGame::ClickEvent, this));

	addChild(widget, 500);

	m_character = Sprite::create("skiinggame/boat.png");
	m_character->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_character->setPosition(Vec2(100, HALFHEIGHT));
	addChild(m_character);

	m_shapeMapManager->setPositionX(m_character->getPositionX() - visibleSize.width / 2);
	StartEvent();

	updateCharacterPosition();

	return true;
}

void SkiingGame::update(float dt)
{
	m_shapeMapManager->update(dt);
	updateCharacterPosition();
}

void SkiingGame::updateCharacterPosition()
{
	m_character->setRotation(m_shapeMapManager->getAngle());
}

void SkiingGame::StartEvent()
{
	scheduleUpdate();
}

void SkiingGame::ClickEvent()
{
	m_shapeMapManager->jump();
}
