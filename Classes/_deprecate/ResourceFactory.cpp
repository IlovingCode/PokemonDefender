#include "ResourceFactory.h"

static ResourceFactory* m_instance = nullptr;

ResourceFactory * ResourceFactory::getInstance()
{
	if (!m_instance) m_instance = new ResourceFactory;
	return m_instance;
}

void ResourceFactory::destroyInstance()
{
	CC_SAFE_DELETE(m_instance);
}

void ResourceFactory::addAnimate(Animate* animate, int& tag)
{
	if (!animate) return;
	if (tag < 0 || tag >= m_animateList.size()) {
		m_animateList.resize(m_animateList.size() + 1);
		tag = m_animateList.size() - 1;
	}
	m_animateList[tag].push_back(animate);
}

Animate * ResourceFactory::getAnimate(int id, int tag)
{
	if(tag + id < 0 || tag >= m_animateList.size()
		|| id >= m_animateList[tag].size())
		return nullptr;
	return m_animateList[tag][id];
}

ResourceFactory::ResourceFactory()
{
	m_animateList.clear();
}


ResourceFactory::~ResourceFactory()
{
	for (std::vector<Animate*> t : m_animateList)
		t.clear();
	m_animateList.clear();
}
