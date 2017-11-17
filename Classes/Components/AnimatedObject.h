#pragma once
#include "cocos2d.h"
#include "json/document.h"

USING_NS_CC;

#define ENEMY_TAG	255
#define TOWER_TAG	256
#define SKILL_TAG	250

enum Direction {
	BACK_LEFT, BACK_RIGHT, FRONT_RIGHT, FRONT_LEFT, RIGHT, LEFT, UP, DOWN
};

enum PokeType {
	NORMAL, FIRE, WATER, GRASS, ELECTRIC, ICE,
	GROUND, FLYING, POISON, FIGHTING, PSYCHIC, DARK,
	ROCK, BUG, GHOST, STEEL, DRAGON, FAIRY
};

class AnimatedObject : public Node
{
private:
	int					m_currentAnimateId;
	Vector<Animate*>	m_animateList;
	Sprite*				m_sprite;
	Sprite*				m_shadow;
	Sprite*				m_effect;

protected:
	std::string m_type;

	CREATE_FUNC(AnimatedObject);
	SpriteFrame*	getFirstFrameOfAnimation(int);
	Animate*		getAnimate(int = 0);
	virtual void	Init(const char*, Vec2); // Sprite has its own function init()
	//deprecated
	//void pause() override;
	//void resume()override;
	void setHeight(Vec2, float);
	void setAnchorPoint(const Vec2&) override;
	void enableShadow(bool);
	void cloneProperties(AnimatedObject*);
	void setDefaultSpriteFrame(int = 0);
	void addAnimation(const char*, int, int = 0, bool = true); //deprecated
	void addAnimation(Texture2D*, std::vector<Rect>, bool = true);
	void runAnimate(int = 0, bool = false, int = 0);
	void runSpriteAction(Action*, bool = true);
	~AnimatedObject();

public:
	void	setScale(float) override;
	float	getScale() const override;
	Rect	getBoundingBox() const override;
};
