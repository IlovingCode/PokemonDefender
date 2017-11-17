#include "GameScene.h"
#include "CatchScene.h"
#include "Components/Cursor.h"
#include "Components/Battle.h"
#include "Components/Tower.h"
#include "Components/Enemy.h"
#include "Managers/PlayerInfo.h"

#include "editor-support/cocostudio/CCSGUIReader.h"

#define WIDGETROOT		2310
#define BACKGROUND		100
#define PAUSE			101
#define EXIT			102
#define SPEED			103
#define BORDER			104
#define CURSOR			105
#define BATTERY			106
#define ROUND			110
#define HEART			111
#define WAVE_DELAY		2
#define ENEMY_INTERVAL	0.6f
#define BATTLE			Battle::getInstance()
#define HEARTMAX		5

void GameScene::onExit()
{
	Battle::destroyInstance();
	Cursor::destroyInstance();
	PlayerInfo::destroyInstance();
	//if you do not remove all custom event of this scene, CRASH!!!
	getEventDispatcher()->removeCustomEventListeners("countDown");
	//important to call parent's method
	Scene::onExit();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init()) {
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = ImageView::create(BATTLE->getMap());
	background->setPosition(visibleSize / 2);
	addChild(background, -visibleSize.height);

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/GameScene.json");
	Helper::seekWidgetByTag(widget, PAUSE)->addClickEventListener(CC_CALLBACK_1(GameScene::PauseEvent, this));
	Helper::seekWidgetByTag(widget, EXIT)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, SPEED)->addClickEventListener(CC_CALLBACK_1(GameScene::SpeedEvent, this));
	Helper::seekWidgetByTag(widget, BACKGROUND)->addTouchEventListener(CC_CALLBACK_2(GameScene::TouchEvent, this));

	m_heart = (ImageView*)Helper::seekWidgetByTag(widget, HEART);
	m_battery = (ImageView*)Helper::seekWidgetByTag(widget, BATTERY);
	applyShader(m_heart, "shaders/Cutoff.vsh", "shaders/Cutoff.fsh");
	applyShader(m_battery, "shaders/Cutoff.vsh", "shaders/Cutoff.fsh");
	m_round = (Text*)Helper::seekWidgetByTag(widget, ROUND);
	Cursor::getInstance()->Init(Helper::seekWidgetByTag(widget, CURSOR));

	auto border = Helper::seekWidgetByTag(widget, BORDER)->getChildren();

	char str[50] = { 0 };
	std::vector<std::string> temp;

	for (int i = 0; i < BATTLE->getTowerListSize(); i++) {
		Tower* tower = BATTLE->getRootTower(i);
		sprintf(str, "towers/%s/avatar.png", tower->getName().c_str());
		((ImageView*)border.at(i))->loadTexture(str);
		((Widget*)border.at(i))->addTouchEventListener(CC_CALLBACK_2(GameScene::SelectTowerEvent, this));
		sprintf(str, "%d", tower->getPrice());
		((Text*)border.at(10 + i))->setString(str);
		unsigned int j = 0;
		for (; j < temp.size(); j++) {
			if (temp.at(j) == tower->getEvolutionStone()) {
				break;
			}
		}
		if (j == temp.size()) {
			temp.push_back(tower->getEvolutionStone());
			sprintf(str, "others/%s.png", temp.back().c_str());
			((ImageView*)border.at(5 + j))->loadTexture(str);
			sprintf(str, "%d", PlayerInfo::getInstance()->getStone(temp.back().c_str()));
			((Text*)border.at(15 + j))->setString(str);
		}
	}

	//init event for Game Scene
	getEventDispatcher()->addCustomEventListener("countDown", CC_CALLBACK_1(GameScene::countDown, this));
	widget->setTag(WIDGETROOT);
	addChild(widget, visibleSize.height);

	m_count					= 0;
	m_heartRemain			= HEARTMAX;
	m_selectedTowerIndex	= -1;
	m_bpaused				= false;
	m_bspeed				= true;
	//Director::getInstance()->setAnimationInterval(1.0f / 60);
	
	m_energyRemain = PlayerInfo::getInstance()->getInitEnergy();
	int energyMax = PlayerInfo::getInstance()->getEnergyMax();
	setUniformAttribWidget(m_heart, "v_fragmentColor", Vec4::ONE / 4);
	setUniformAttribWidget(m_battery, "v_fragmentColor", Vec4::ZERO);
	setUniformAttribWidget(m_heart, "fx_threshold", 1);
	setUniformAttribWidget(m_heart, "fy_threshold", 0);
	setUniformAttribWidget(m_battery, "fx_threshold", (float)m_energyRemain / energyMax);
	setUniformAttribWidget(m_battery, "fy_threshold", 0);

	refreshTower();
	scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::waveCall), WAVE_DELAY);
		
    return true;
}

void GameScene::refreshTower()
{
	auto widget = (Widget*)getChildByTag(WIDGETROOT);
	auto border = Helper::seekWidgetByTag(widget, BORDER)->getChildren();

	for (int i = 0; i < BATTLE->getTowerListSize(); i++) {
		auto tower = BATTLE->getRootTower(i);
		border.at(i)->setColor(m_energyRemain < tower->getPrice() ? Color3B::GRAY : Color3B::WHITE);
	}
}

void GameScene::applyShader(Widget *widget, const char *vertFilename, const char* fragFilename)
{
	auto fileUtiles = FileUtils::getInstance();

	auto fullPath = fileUtiles->fullPathForFilename(vertFilename);
	auto vertSource = fileUtiles->getStringFromFile(fullPath);

	fullPath = fileUtiles->fullPathForFilename(fragFilename);
	auto fragSource = fileUtiles->getStringFromFile(fullPath);

	auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
	getWigetSprite(widget)->setGLProgram(glprogram);
}

Node * GameScene::getWigetSprite(Widget *widget)
{
	return ((Scale9Sprite*)widget->getVirtualRenderer())->getSprite();
}

void GameScene::setUniformAttribWidget(Widget *widget, const char *name, float value)
{
	getWigetSprite(widget)->getGLProgramState()->setUniformFloat(name, value);
}

void GameScene::setUniformAttribWidget(Widget *widget, const char *name, Vec4 value)
{
	getWigetSprite(widget)->getGLProgramState()->setUniformVec4(name, value);
}

void GameScene::waveCall(float delta)
{
	char str[15] = { 0 };
	sprintf(str, "Round : %d", BATTLE->getRound());
	m_round->setString(str);

	m_count = BATTLE->getCount();
	if(m_count)
		schedule(CC_SCHEDULE_SELECTOR(GameScene::enemyCall), ENEMY_INTERVAL, m_count - 1, 0);
}

void GameScene::enemyCall(float delta)
{
	auto enemy = BATTLE->getEnemy(random(0, (int)BATTLE->getEnemyListSize() - 1));
	if(enemy) addChild(enemy);
}

void GameScene::countDown(EventCustom* event)
{
	if (event->getUserData()){
		int energyMax = PlayerInfo::getInstance()->getEnergyMax();
		ResourcePack* items = (ResourcePack*)event->getUserData();
		m_energyRemain += items->at(RESOURCEPACK_ENERGY).second;
		if (m_energyRemain > energyMax) {
			m_energyRemain = energyMax;
		}
		setUniformAttribWidget(m_battery, "fx_threshold", (float)m_energyRemain / energyMax);
		refreshTower();
	}
	else {
		m_heartRemain--;
		if (m_heartRemain == 0) {
			Director::getInstance()->popScene();
			Director::getInstance()->pushScene(CatchScene::create());
		}
		setUniformAttribWidget(m_heart, "fy_threshold", 1 - (float)m_heartRemain / HEARTMAX);
	}
	if (--m_count == 0)
		scheduleOnce(CC_SCHEDULE_SELECTOR(GameScene::waveCall), WAVE_DELAY);
}

void GameScene::SelectTowerEvent(Ref *pSender, Widget::TouchEventType type)
{
	Vec2 touchPos;
	Widget* widget = (Widget*)pSender;
	switch (type){
	case Widget::TouchEventType::BEGAN:
		if (BATTLE->getRootTower(widget->getTag())->getPrice() <= m_energyRemain) {
			m_selectedTowerIndex = widget->getTag();
			BATTLE->getRootTower(m_selectedTowerIndex)->onSelect(widget->getTouchBeganPosition());
		}
		break;
	case Widget::TouchEventType::MOVED:
		touchPos = BATTLE->standardizePosition(widget->getTouchMovePosition());
		Cursor::getInstance()->onMove(touchPos, BATTLE->isPlaceable(touchPos));
		break;
	case Widget::TouchEventType::ENDED:
		m_selectedTowerIndex = -1;
		Cursor::getInstance()->onEnd();
		break;
	case Widget::TouchEventType::CANCELED:
		if (m_selectedTowerIndex >= 0){
			auto tower = BATTLE->getTower(m_selectedTowerIndex, widget->getTouchEndPosition());
			if (tower) {
				this->addChild(tower);
				int energyMax = PlayerInfo::getInstance()->getEnergyMax();
				m_energyRemain -= BATTLE->getRootTower(m_selectedTowerIndex)->getPrice();
				setUniformAttribWidget(m_battery, "fx_threshold", (float)m_energyRemain / energyMax);
				refreshTower();
			}
		}
		m_selectedTowerIndex = -1;
		Cursor::getInstance()->onEnd();
		break;
	}
}

void GameScene::PauseEvent(Ref *pSender)
{
	if (Director::getInstance()->isPaused()){
		Director::getInstance()->resume();
		((Button*)pSender)->loadTextureNormal("others/pause.png");
		//resume();
		//for (Node* child : getChildren()){
		//	child->resume();
		//}
	}
	else{
		Director::getInstance()->pause();
		((Button*)pSender)->loadTextureNormal("others/play.png");
		//pause();
		//for (Node* child : getChildren()) {
		//	child->pause();
		//}
	}
	m_bpaused = !m_bpaused;
}

void GameScene::SpeedEvent(Ref *pSender)
{
	Director::getInstance()->popScene();
	Director::getInstance()->pushScene(CatchScene::create());

/*
	m_bspeed = !m_bspeed;
	float animationInterval = Director::getInstance()->getAnimationInterval();
	if (m_bspeed){
		((Button*)pSender)->setColor(Color3B::WHITE);
		Director::getInstance()->setAnimationInterval(animationInterval / 2);
	}
	else{
		((Button*)pSender)->setColor(Color3B::GRAY);
		Director::getInstance()->setAnimationInterval(animationInterval * 2);
	}*/
}

void GameScene::TouchEvent(Ref *pSender, Widget::TouchEventType type)
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
		if (Cursor::getInstance()->isVisible()) {
			Cursor::getInstance()->onEnd();
		}
		else {
			for (Node* child : getChildren()) {
				if (child->getTag() == TOWER_TAG) {
					((Tower*)child)->onSelect(touchPos);
					if (Cursor::getInstance()->isVisible()) break;
				}
			}
		}
		break;
	}
}