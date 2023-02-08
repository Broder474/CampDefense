#include "World.h"

// class Entity
Entity::~Entity() {};

// class Character
Character::~Character() {};

void Character::add_combat_xp(unsigned int combat_xp)
{
	this->combat_xp += combat_xp;
	while (this->combat_xp > combat_xp_upgrade)
	{
		this->combat_xp -= combat_xp_upgrade;
		combat_lvl++;
		calc_combat_xp_upgrade();
	}
}

void Character::add_survival_xp(unsigned int survival_xp)
{
	this->survival_xp += survival_xp;
	while (this->survival_xp > survival_xp_upgrade)
	{
		this->survival_xp -= survival_xp_upgrade;
		survival_lvl++;
		calc_survival_xp_upgrade();
	}
}

void Character::calc_combat_xp_upgrade()
{
	combat_xp_upgrade = (combat_lvl + 1) * 1000;
}

void Character::calc_survival_xp_upgrade()
{
	survival_xp_upgrade = (survival_lvl + 1) * 1000;
}

// class Weapon
Weapon::~Weapon() {};