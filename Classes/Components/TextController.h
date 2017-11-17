#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class TextController {
private:
	Text* m_text;
	char* m_buff;

	float m_alpha;
	int m_oldInt, m_newInt;
	std::string m_textStr;

public:
	TextController();
	~TextController();

	void initText(Text*);
	void setTextStr(std::string);
	void setTextInt(int);
	void processInteger();
	void update(float dt);
};
