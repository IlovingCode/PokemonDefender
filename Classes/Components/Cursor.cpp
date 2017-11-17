#include "Cursor.h"
#include "Battle.h"
#include "Tower.h"
#include "Requirement.h"

static Cursor* m_instance = nullptr;

void Cursor::showControlPanel(bool isShow)
{
	for (Node* child : m_panel->getChildren()) {
		child->setVisible(isShow);
	}
	m_IsControlPanelShown = isShow;
	m_range->setVisible(true);
}

void Cursor::onSell()
{
	onEnd();
	Battle::getInstance()->removeTower(m_tower);
}

void Cursor::onUpgrade()
{
	onEnd();
	Battle::getInstance()->upgradeTower(m_tower);
}

Cursor::Cursor()
{
}


Cursor::~Cursor()
{
}

void Cursor::Init(Widget* panel)
{
	m_panel = panel;
	((Widget*)m_panel->getChildren().at(1))->addClickEventListener(CC_CALLBACK_0(Cursor::onSell, this));
	((Widget*)m_panel->getChildren().back())->addClickEventListener(CC_CALLBACK_0(Cursor::onUpgrade, this));
	m_range = m_panel->getChildren().front();
	m_panel->setVisible(false);
	m_tower = nullptr;
}

Cursor * Cursor::getInstance()
{
	if (!m_instance) {
		m_instance = new Cursor;
	}
	return m_instance;
}

void Cursor::destroyInstance()
{
	CC_SAFE_DELETE(m_instance);
}

void Cursor::onBegin(Node* pSender, Vec2 pos, int range, int sellPrice, RequirementArray* requirementList)
{
	if(isVisible()) onEnd(); // make sure onBegin is called after onEnd
	if (pSender == nullptr) return;

	m_tower = pSender;
	m_range->setColor(Color3B::GREEN);
	m_range->setScale(2 * range / m_range->getContentSize().width);
	m_panel->setPosition(pos);
	m_panel->setVisible(true);
	showControlPanel(m_tower->isVisible());

	if (!m_tower->isVisible()) {
		m_panel->addChild(m_tower);
		m_range->setPositionY(m_tower->getPositionY());
		m_range->setColor(Color3B::RED);
		m_tower->setPositionY(0);
		m_tower->setVisible(true);
	}
	else {
		char str[5];
		sprintf(str, "%d", sellPrice);
		((Text*)m_panel->getChildren().at(1)->getChildren().front())->setString(str);
		if (requirementList) {
			auto content = m_panel->getChildren().at(2)->getChildren();
			for (int i = 0; i < requirementList->count(); i++) {
				requirementList->getItem(i)->show((ImageView*)content.at(i));
			}
		}
	}
}

void Cursor::onMove(Vec2 pos, bool isPlaceable)
{
	m_panel->setPosition(pos);
	m_range->setColor(isPlaceable ? Color3B::GREEN : Color3B::RED);
}

void Cursor::onEnd()
{
	if (!m_IsControlPanelShown) {
		m_tower->setVisible(false);
		m_range->setPositionY(0);
		m_panel->removeChild(m_tower);
	}
	else {
		((Tower*)m_tower)->onDeselect();
	}
	
	auto content = m_panel->getChildren().at(2)->getChildren();
	for (int i = 0; i < content.size(); i++) {
		content.at(i)->setVisible(false);
	}
	m_panel->setVisible(false);
}

bool Cursor::isVisible()
{
	return m_panel->isVisible();
}
