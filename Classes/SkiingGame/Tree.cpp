#include "Tree.h"

#define TREETAG		255

extern std::vector<Rect> getRectList(int col, int row, int frameCount, Rect& rect);

Tree * Tree::create(const char* name ,Vec2 pos)
{
	auto ret = Tree::create();
	ret->Init(name, pos);
	ret->setVisible(false);

	return ret;
}

Tree * Tree::clone(Vec3 pos)
{
	auto clone = Tree::create();

	clone->cloneProperties(this);

	clone->m_speed = Vec3(0, 0, 0.5f);

	clone->setPosition3D(pos);
	clone->runAnimate();

	return clone;
}

void Tree::update(float)
{
	setPosition3D(getPosition3D() + m_speed);
	if (getPositionZ() > -100) {
		m_speed.y = -0.03f;
	}
	setLocalZOrder(getPositionZ());
}

void Tree::Init(const char* name, Vec2 pos)
{
	char str[50] = { 0 };
	rapidjson::Document doc, data;
	doc.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile("skiinggame/_config.json").c_str());

	data.Parse<0>(cocos2d::FileUtils::getInstance()->getStringFromFile(doc["rectInfo"].GetString()).c_str());
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(doc["texture"].GetString());

	std::vector<Rect> rectList;
	int frameCount = doc["animation"].Size();
	for (int j = 0; j < frameCount; j++) {
		sprintf(str, "Rect%d", doc["animation"][j].GetInt());
		rectList.push_back(Rect(data[str][0].GetInt(), data[str][1].GetInt(),
			data[str][2].GetInt(), data[str][3].GetInt()));
	}
	addAnimation(texture, rectList, true);

	AnimatedObject::Init(name, pos);
	
	setTag(TREETAG);
}
