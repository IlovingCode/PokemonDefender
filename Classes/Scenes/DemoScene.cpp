#include "DemoScene.h"
#include "editor-support/cocostudio/CCSGUIReader.h"

#define DELAY	0.5f

bool DemoScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/DemoScene.json");

	Helper::seekWidgetByTag(widget, 1)->addClickEventListener(CC_CALLBACK_1(DemoScene::PauseEvent, this));
	Helper::seekWidgetByTag(widget, 2)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, 0)->addClickEventListener(CC_CALLBACK_0(DemoScene::StartEvent, this));
	Helper::seekWidgetByTag(widget, 3)->addClickEventListener(CC_CALLBACK_0(DemoScene::SkipEvent, this));
	Helper::seekWidgetByTag(widget, 4)->addClickEventListener(CC_CALLBACK_0(DemoScene::BackEvent, this));

	m_round = (Text*)Helper::seekWidgetByTag(widget, 110);
	addChild(widget);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("enemies/0.png");
	m_doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("enemies/data.json").c_str());
	m_sprite = Sprite::createWithTexture(texture);
	m_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_sprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 4));
	m_sprite->setScale(10);
	addChild(m_sprite);

	m_id = 0;
	m_bpause = false;
	schedule(CC_SCHEDULE_SELECTOR(DemoScene::ClickEvent), DELAY, m_doc.MemberCount(), 0);
	StartEvent();

	return true;
}

void DemoScene::StartEvent()
{
	ClickEvent(0);
}

void DemoScene::SkipEvent()
{
	m_id += 10;
}

void DemoScene::BackEvent()
{
	if (m_id < 10) m_id = 0;
	else m_id -= 10;
}

void DemoScene::ClickEvent(float)
{	
	if (m_bpause) return;

	if (m_id >= m_doc.MemberCount()) m_id = 0;

	char str[50] = { 0 };
	sprintf(str, "Rect%d", m_id++);
	m_round->setString(str);

	Rect rect;
	rect.setRect(m_doc[str][0].GetInt(), m_doc[str][1].GetInt(),
		m_doc[str][2].GetInt(), m_doc[str][3].GetInt());

	m_sprite->setTextureRect(rect);
}

void DemoScene::PauseEvent(Ref *pSender)
{
	m_bpause = !m_bpause;
	if (m_bpause) {
		((Button*)pSender)->loadTextureNormal("others/play.png");
	}
	else {
		((Button*)pSender)->loadTextureNormal("others/pause.png");
	}
}