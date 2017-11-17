#pragma once
#include "cocos2d.h"

USING_NS_CC;

class ResourceFactory
{
private:
	std::vector<std::vector<Animate*>> m_animateList;

public:
	static ResourceFactory* getInstance();
	void destroyInstance();
	void addAnimate(Animate*, int&);
	Animate* getAnimate(int, int);
	ResourceFactory();
	~ResourceFactory();
};