#include "MiniGame.h"
#include "editor-support/cocostudio/CCSGUIReader.h"

#define SIZE_OF_CELL	60.0f
#define COLNUM			10
#define ROWNUM			6
#define OFFSET			Vec2(130, 90)
#define HALF			Vec2(SIZE_OF_CELL, SIZE_OF_CELL) / 2

int point2id(Vec2 point)
{
	return (int)point.x * ROWNUM + (int)point.y;
}

int point2id(int x, int y)
{
	return x * ROWNUM + y;
}

Vec2 id2point(int id)
{
	return Vec2(id / ROWNUM, id % ROWNUM);
}

void MiniGame::onExit()
{
	CC_SAFE_DELETE(m_array);
	//important to call parent's method
	Scene::onExit();
}

bool MiniGame::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto widget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("scenes/MiniGame.json");

	Helper::seekWidgetByTag(widget, 1)->addClickEventListener(CC_CALLBACK_0(MiniGame::StartEvent, this));
	Helper::seekWidgetByTag(widget, 2)->addClickEventListener(CC_CALLBACK_0(Director::popScene, Director::getInstance()));
	Helper::seekWidgetByTag(widget, 0)->addTouchEventListener(CC_CALLBACK_2(MiniGame::TouchEvent, this));

	addChild(widget);

	m_array = new int[COLNUM * ROWNUM];
	std::vector<int> indexArray;
	//clear the array
	for (int i = 0; i < COLNUM * ROWNUM; i++) {
		m_array[i] = -1;
		indexArray.push_back(i);
	}

	std::random_shuffle(indexArray.begin(), indexArray.end());

	while (indexArray.size()) {
		//create random pokemon. maximum 400 types in resource
		int id = random(0, 300);
		int col = id / 20;
		int row = id % 20;

		auto sprite1 = Sprite::create("minigame/hgss.png", Rect(col * 80, row * 80, 80, 80));
		auto sprite2 = Sprite::create("minigame/hgss.png", Rect(col * 80, row * 80, 80, 80));

		sprite1->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sprite2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sprite1->setScale(SIZE_OF_CELL / 80);
		sprite2->setScale(SIZE_OF_CELL / 80);

		//put it into two random positions
		int place = indexArray[indexArray.size() - 1];
		m_array[place] = id;
		sprite1->setPosition(OFFSET + id2point(place) * SIZE_OF_CELL);
		sprite1->setTag(place + 1);
		indexArray.pop_back();

		place = indexArray[indexArray.size() - 1];
		m_array[place] = id;
		sprite2->setPosition(OFFSET + id2point(place) * SIZE_OF_CELL);
		sprite2->setTag(place + 1);
		indexArray.pop_back();
		//
		addChild(sprite1);
		addChild(sprite2);
	}

	m_cursor1 = Sprite::create("minigame/frame.png");
	m_cursor1->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_cursor1->setVisible(false);
	m_cursor1->setLocalZOrder(100);

	m_cursor2 = Sprite::create("minigame/frame.png");
	m_cursor2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_cursor2->setVisible(false);
	m_cursor2->setLocalZOrder(100);
	
	addChild(m_cursor1);
	addChild(m_cursor2);

	m_selected1 = m_selected2 = -1;

	return true;
}

void MiniGame::StartEvent()
{
}

void MiniGame::TouchEvent(Ref *pSender, Widget::TouchEventType type)
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
		touchPos = (touchPos - OFFSET + HALF) / SIZE_OF_CELL;
		if (m_selected1 < 0) {
			m_selected1 = (int)touchPos.x * ROWNUM + (int)touchPos.y;
			if (m_array[m_selected1] < 0) {
				m_selected1 = -1;
			}
			else {
				m_cursor1->setPosition(OFFSET + id2point(m_selected1) * SIZE_OF_CELL);
				m_cursor1->setVisible(true);
			}
		}
		else if (m_selected2 < 0) {
			m_selected2 = (int)touchPos.x * ROWNUM + (int)touchPos.y;
			if (m_array[m_selected2] < 0) {
				m_selected2 = -1;
			}
			else {
				m_cursor2->setPosition(OFFSET + id2point(m_selected2) * SIZE_OF_CELL);
				m_cursor2->setVisible(true);

				Process();
			}
		}

		break;
	}
}

Vec2 find(Vec2 point, int* arr)
{
	Vec2 temp;
	int left, right;

	temp = point;
	temp.x--;
	while (temp.x >= 0 && arr[point2id(temp)] < 0) {
		temp.x--;
	}
	left = temp.x;

	temp = point;
	temp.x++;
	while (temp.x < ROWNUM && arr[point2id(temp)] < 0) {
		temp.x++;
	}
	right = temp.x;

	return Vec2(left, right);
}

void MiniGame::Process()
{
	Vec2 selected1 = id2point(m_selected1);
	Vec2 selected2 = id2point(m_selected2);
	Vec2 a = find(selected1, m_array);
	Vec2 b = find(selected2, m_array);

	int ymax = MAX(selected1.y, selected2.y);
	int ymin = MIN(selected1.y, selected2.y);
	int imax = MIN(a.y, b.y);
	int imin = MAX(a.x, b.x);

	int weight = COLNUM * ROWNUM;
	bool isPath = false;

	for (int i = imin; i < imax; i++) {
		for (int j = ymin; j < ymax; j++) {
			if (m_array[point2id(i, j)] >= 0) {
				isPath = false;
				break;
			}
			else {
				isPath = true;
			}
		}
		if (isPath) {
			//int w =		
		}
	}

	// check if two selections are same or not
	if (m_array[m_selected1] == m_array[m_selected2]){
		removeChildByTag(m_selected1 + 1);
		removeChildByTag(m_selected2 + 1);
		m_array[m_selected1] = m_array[m_selected2] = -1;
	}

	m_selected1 = m_selected2 = -1;
	m_cursor1->setVisible(false);
	m_cursor2->setVisible(false);
}