#include "MainMenu.h"
#include "GameScene.h"
#include "MiniGame.h"
#include "DemoScene.h"
#include "SkiingGame/SkiingGame.h"
#include "Components/Battle.h"
#include "editor-support/cocostudio/CCSGUIReader.h"

#define START		1
#define EXIT		2
#define MINIGAME	3
#define DEMO		4

std::vector<std::string> temp;

bool MainMenu::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/MainMenu.json");

	Helper::seekWidgetByTag(widget, START)->addClickEventListener(CC_CALLBACK_0(MainMenu::StartEvent, this));
	Helper::seekWidgetByTag(widget, EXIT)->addClickEventListener(CC_CALLBACK_0(MainMenu::ExitEvent, this));
	Helper::seekWidgetByTag(widget, MINIGAME)->addClickEventListener(CC_CALLBACK_0(MainMenu::MiniGameEvent, this));
	Helper::seekWidgetByTag(widget, DEMO)->addClickEventListener(CC_CALLBACK_0(MainMenu::DemoEvent, this));

	temp.clear();
	
	/*temp.push_back("abra");
	temp.push_back("venusaur");
	temp.push_back("blastoise");
	temp.push_back("charmander");
	temp.push_back("wartortle");*/

	/*temp.push_back("articuno");
	temp.push_back("zapdos");
	temp.push_back("moltres");
	temp.push_back("haunter");
	temp.push_back("gengar");*/

	/*temp.push_back("gastly");
	temp.push_back("victreebell");
	temp.push_back("bellsprout");
	temp.push_back("ponyta");
	temp.push_back("rapidash");*/

	/*temp.push_back("pikachu");
	temp.push_back("raichu");
	temp.push_back("squirtle");
	temp.push_back("ivysaur");
	temp.push_back("bulbasaur");*/

	/*temp.push_back("magnemite");
	temp.push_back("magneton");
	temp.push_back("lapras");
	temp.push_back("weepinbell");
	temp.push_back("charmeleon");*/

	temp.push_back("kadabra");
	temp.push_back("alakazam");
	temp.push_back("mewtwo");
	temp.push_back("shellder");
	temp.push_back("cloyster");

	addChild(widget);

	return true;
}

void MainMenu::onEnter()
{
	m_isInputLocked = false;
	Scene::onEnter();
}

void MainMenu::StartEvent()
{
	if (m_isInputLocked)
		return;
	m_isInputLocked = true;
	Battle::getInstance()->reset();
	Battle::getInstance()->init("battles/battle.json", temp);
	Director::getInstance()->pushScene(GameScene::create());
}

void MainMenu::ExitEvent()
{
	//Battle::destroyInstance();
	Director::getInstance()->popScene();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void MainMenu::MiniGameEvent()
{
	if (m_isInputLocked)
		return;
	m_isInputLocked = true;
	Director::getInstance()->pushScene(MiniGame::create());
}

void MainMenu::DemoEvent()
{
	if (m_isInputLocked)
		return;
	m_isInputLocked = true;
	Director::getInstance()->pushScene(SkiingGame::create());
}
