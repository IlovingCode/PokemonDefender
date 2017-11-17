#include "CCFileUtils.h"

class PlayerInfo
{
private:
	std::string m_name;
	cocos2d::ValueMap m_data;
	
	PlayerInfo();
	~PlayerInfo();
	void save();
	void load();

public:
	static PlayerInfo* getInstance();
	static void destroyInstance();

	int getItemAmount(const char* itemName);
	int getGold() { return getItemAmount("gold"); }
	int getInitEnergy() { return getItemAmount("initEnergy"); }
	int getStone(const char* stoneName) { return getItemAmount(stoneName); }
	int getEnergyMax() { return getItemAmount("energyMax"); }
	const char* getName() { return m_name.c_str(); }

	void setName(std::string name) {m_name = name; }
	void onChange(const char*, int);
};

