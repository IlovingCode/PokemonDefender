#include "ShapeMap.h"

#define PI 3.14159265359f

ShapeMap::~ShapeMap()
{
	m_next = nullptr;
	CC_SAFE_DELETE_ARRAY(m_heightMap);
	CC_SAFE_DELETE_ARRAY(m_rotationMap);
}

ShapeMap * ShapeMap::create(const char * filename)
{
	ShapeMap *shapeMap = new (std::nothrow) ShapeMap();
	if (shapeMap && shapeMap->initWithFile(filename))
	{
		shapeMap->autorelease();
		shapeMap->load(filename);
		return shapeMap;
	}
	CC_SAFE_DELETE(shapeMap);
	return nullptr;
}

void ShapeMap::load(const char *name)
{
	// make height map from resource
	////////////////////////////////////
	Image image;
	image.initWithImageFile(name);
	unsigned char* data = image.getData();
	std::vector<Vec4> pixelArray;

	// parse to pixel array
	for (int i = 0; i < image.getDataLen() - 3; i += 4) {
		pixelArray.push_back(Vec4(data[i], data[i + 1], data[i + 2], data[i + 3]));
	}

	int width = image.getWidth();
	int height = image.getHeight();
	int delta = 10;

	m_heightMap = new float[width];
	m_rotationMap = new float[width];
	m_next = nullptr;
	m_isHead = false;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (pixelArray[j*width + i].w) {
				m_heightMap[width - i - 1] = height - j - 1;
				break;
			}
		}
	}

	for (int i = 0; i < width; i++) {
		if (i < delta || i + delta >= width) {
			m_rotationMap[i] = 0;
			continue;
		}

		Vec2 temp = Vec2(i + delta, m_heightMap[i + delta]) - Vec2(i - delta, m_heightMap[i - delta]);
		m_rotationMap[i] = temp.getAngle() * 180.0f / PI;
	}
}

bool ShapeMap::Update(float dt)
{
	int x = getPositionX() - dt;
	float w = getContentSize().width;
	setPositionX(x);
	if (!m_next->m_isHead){
		if (m_next->Update(dt)) {
			float posY = getParent()->getPositionY();
			for (Node* node : getParent()->getChildren()) {
				node->setPositionY(node->getPositionY() + posY);
			}
			getParent()->setPositionY(0);
			m_next->setPositionBelong2PreviousMap(this);
		}
	}
	if (m_isHead) {
		if (x >= 0 && x < w) {
			getParent()->setPositionY(m_alignHeight - m_heightMap[x] - getPositionY());
			return false;
		}
	}
	else {
		if (x > -w) {
			return false;
		}
	}
	return true;
}

float ShapeMap::getAngle()
{
	return m_rotationMap[(int)getPositionX()];
}

float ShapeMap::getBeginHeight()
{
	return m_heightMap[0];
}

float ShapeMap::getEndHeight()
{
	return m_heightMap[(int)getContentSize().width - 1];
}

void ShapeMap::setHead(bool isHead)
{
	m_isHead = isHead;
}

void ShapeMap::setHeight(float height)
{
	m_alignHeight = height;
}

void ShapeMap::setPositionBelong2PreviousMap(ShapeMap* prevMap)
{
	Vec2 prevPos = prevMap->getPosition();
	setPositionX(prevPos.x + prevMap->getContentSize().width);
	setPositionY(prevPos.y + prevMap->getBeginHeight() - getEndHeight());
}