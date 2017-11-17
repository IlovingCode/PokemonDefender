#include "PlayerInfo.h"


#define FILENAME	"save.dat"

PlayerInfo* m_instance = nullptr;

const char* itemNameList[] = {
	"stoneFire",
	"stoneWater",
	"stoneLeaf",
	"stoneThunder",
	"stoneDawn",
	"stoneShiny",
	"stoneDusk",
	"stoneSun",
	"stoneMoon",
	"stoneEver"
};

PlayerInfo::PlayerInfo()
{
}


PlayerInfo::~PlayerInfo()
{
}

PlayerInfo * PlayerInfo::getInstance()
{
	if (!m_instance) {
		m_instance = new PlayerInfo;
		m_instance->load();
	}
	return m_instance;
}

void PlayerInfo::destroyInstance()
{
	if (m_instance) {
		m_instance->save();

		delete(m_instance);
		m_instance = nullptr;
	}
}

int PlayerInfo::getItemAmount(const char * itemName)
{
	return m_data[itemName].getType() == cocos2d::Value::Type::INTEGER ? m_data[itemName].asInt() : 0;
}

void PlayerInfo::onChange(const char* itemName, int amount)
{
	if(m_data[itemName].getType() == cocos2d::Value::Type::INTEGER) m_data[itemName] = amount;
}

void PlayerInfo::save()
{
	cocos2d::FileUtils::getInstance()->writeValueMapToFile(m_data, FILENAME);
}

void PlayerInfo::load()
{
	m_data = cocos2d::FileUtils::getInstance()->getValueMapFromFile(FILENAME);
}
