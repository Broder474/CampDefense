#include "World.h"
#include <iostream>

// class World
World::World(unsigned int day, unsigned int hour): day(day)
{
	addHours(hour);
}

void World::addHours(int hours)
{
	hour += hours;
	day += hour / 24;
	hour %= 24;
}

void World::update()
{
	ticks++;
}

bool World::addProducts(int products)
{
	if (this->products + products < 0)
		return false;
	else 
		this->products += products;
	return true;
}

bool World::addMaterials(int materials)
{
	if (this->materials + materials < 0)
		return false;
	else
		this->materials += materials;
	return true;
}

void World::searchProducts()
{
	int found_products = 0;
	for (auto& entity : entities)
		if (typeid(*entity) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity);
			found_products += 1 + rand() % character.getSurvivalLvl();
			dynamic_cast<Character&>(*entity).add_survival_xp(100);
		}
	addProducts(found_products);
}

void World::searchMaterials()
{

}
// class Entity
Entity::~Entity() {};

// class Character
Character::Character(int gender, unsigned int health, float speed, unsigned int strength, Resources* res, Weapon* weapon, unsigned int consumption,
	unsigned int combat_lvl, unsigned int combat_xp, unsigned int survival_lvl, unsigned int survival_xp) : Entity(health, speed, strength),
	weapon(weapon), consumption(consumption), combat_lvl(combat_lvl), survival_lvl(survival_lvl)
{
	try
	{
		if (gender != Male && gender != Female)
			throw "Gender is not defined";
		this->gender = gender;
		std::string names_tree = "";
		if (gender == Male)
			names_tree = "male";
		else if (gender == Female)
			names_tree = "female";
		int names_count = res->lang["names"][names_tree].size();
		if (names_count > 0)
			name = res->lang["names"][names_tree].at(rand() % names_count);
	}
	catch (std::string error)
	{
		std::cout << error << std::endl;
	}
	calc_combat_xp_upgrade();
	add_combat_xp(combat_xp);
	calc_survival_xp_upgrade();
	add_survival_xp(survival_xp);
};

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

// class Zombie
Zombie::~Zombie() {};	