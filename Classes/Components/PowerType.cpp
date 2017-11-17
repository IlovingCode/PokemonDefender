#include <string.h>
#include "PowerType.h"

const char* typeNameList[] = {
	"normal",
	"fight",
	"flying",
	"poison",
	"ground",
	"rock",
	"bug",
	"ghost",
	"steel",
	"fire",
	"water",
	"grass",
	"electric",
	"psychic",
	"ice",
	"dragon",
	"dark",
	"fairy",
	0
};

PowerType::PowerType(const char *typeName)
{
	for (int i = 0; typeNameList[i] != 0; i++) {
		if (!strcmp(typeNameList[i], typeName))
			m_typeId = static_cast<PowerTypeID>(i);
	}
}



PowerType::~PowerType()
{
}
