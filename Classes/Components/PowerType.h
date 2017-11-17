class PowerType
{
	enum PowerTypeID {
		NORMAL, FIGHT, FLYING, POISON, GROUND, ROCK, 
		BUG, GHOST, STEEL, FIRE, WATER, GRASS, 
		ELECTRIC, PSYCHIC, ICE, DRAGON, DARK, FAIRY
	};

	PowerTypeID m_typeId;

public:
	PowerType(const char*);
	~PowerType();
};

