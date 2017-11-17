#include "Requirement.h"
#include "Managers/PlayerInfo.h"

Requirement::Requirement(rapidjson::Value& doc)
{
	init(doc);
}


Requirement::~Requirement()
{
}

void Requirement::show(cocos2d::ui::ImageView* imgView)
{
	char str[50];
	sprintf(str, "others/%s.png", m_itemName.c_str());
	imgView->loadTexture(str);
	sprintf(str, "%d", m_itemAmount);
	((cocos2d::ui::Text*)imgView->getChildren().front())->setString(str);
	imgView->setVisible(true);
}

void Requirement::init(rapidjson::Value& doc)
{
	m_type =		doc["type"].GetString();
	m_itemName =	doc["itemName"].GetString();
	m_itemAmount =	doc["itemAmount"].GetInt();
	//m_time =		doc["time"].GetString();
}

bool Requirement::isMeet()
{
	return PlayerInfo::getInstance()->getItemAmount(m_itemName.c_str()) >= m_itemAmount;
}

RequirementArray::RequirementArray(rapidjson::Value& doc)
{
	init(doc);
}


RequirementArray::~RequirementArray()
{
	for (Requirement* requirement : m_requirementList) {
		delete requirement;
	}
	m_requirementList.clear();
}



void RequirementArray::init(rapidjson::Value& doc)
{
	if (doc.IsArray()){
		for (rapidjson::SizeType i = 0; i < doc.Size();i++) {
			m_requirementList.push_back(new Requirement(doc[i]));
		}
	}
}

bool RequirementArray::isMeet()
{
	for (Requirement* requirement : m_requirementList) {
		if (!requirement->isMeet()) return false;
	}
	return true;
}
