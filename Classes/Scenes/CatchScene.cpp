#include "CatchScene.h"
#include "Components/Battle.h"
#include "Components/PokeBall.h"
#include "editor-support/cocostudio/CCSGUIReader.h"

#define ASSERT(x) if(!x) return false;

bool CatchScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/CatchScene.json");

	Helper::seekWidgetByTag(widget, 1)->addClickEventListener(CC_CALLBACK_0(CatchScene::StartEvent, this));
	Helper::seekWidgetByTag(widget, 2)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, 0)->addTouchEventListener(CC_CALLBACK_2(CatchScene::TouchEvent, this));

	addChild(widget);

	auto ground = Sprite::create("others/land.png");
	ground->setPosition(visibleSize / 2);
	ground->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	addChild(ground);

	m_pokemon = Tower::create(Battle::getInstance()->getBossName().c_str())->clone(Vec2::ZERO);
	m_pokemon->setPosition(visibleSize / 2);
	m_pokemon->setLocalZOrder(0);
	m_pokemon->setScale(1.5f);
	addChild(m_pokemon);
	
	m_pokeBall = PokeBall::create("others/ball_sprite.png", Vec2(70, 50));
	m_pokeBall->setTarget(m_pokemon);
	m_pokeBall->setSpeed(400);
	addChild(m_pokeBall);

	return true;
}

void CatchScene::StartEvent()
{
}

void CatchScene::TouchEvent(Ref *pSender, Widget::TouchEventType type)
{
	Vec2 touchPos;
	//Widget* widget = (Widget*)pSender;
	switch (type)
	{
		/*case Widget::TouchEventType::BEGAN:
		touchPos = widget->getTouchBeganPosition();
		break;
		case Widget::TouchEventType::MOVED:
		touchPos = widget->getTouchMovePosition();
		break;
		case Widget::TouchEventType::CANCELED:
		touchPos = widget->getTouchEndPosition();
		break;*/
	case Widget::TouchEventType::ENDED:
		touchPos = ((Widget*)pSender)->getTouchEndPosition();
		m_pokeBall->onTouch(touchPos);
		break;
	}
}

void CatchScene::Process()
{
}