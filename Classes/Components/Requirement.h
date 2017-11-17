#include "json/document.h"
#include "ui/CocosGUI.h"

class Requirement
{
private:
	std::string m_type;
	std::string m_itemName;
	std::string m_time;
	int m_itemAmount;

public:
	Requirement(rapidjson::Value&);
	~Requirement();

	void show(cocos2d::ui::ImageView*);
	void init(rapidjson::Value&);
	bool isMeet();
};

class RequirementArray
{
private:
	std::vector<Requirement*> m_requirementList;

public:
	RequirementArray(rapidjson::Value&);
	~RequirementArray();

	Requirement* getItem(int i) { return m_requirementList[i]; }
	int count() { return m_requirementList.size(); }
	void init(rapidjson::Value&);
	bool isMeet();
};