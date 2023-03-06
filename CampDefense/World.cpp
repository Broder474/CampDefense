#include "World.h"
#include <iostream>
#include "Functions.h"

// class World
World::World(Resources* res, Settings* settings, unsigned int day, unsigned int hour): res(res), settings(settings), day(day)
{
	addHours(hour);
}

int World::findFreeId()
{
	int id = 0;
	for (auto& entity1 : entities)
	{
		bool free_id_found = true;
		for (auto& entity2 : entities)
			if (id != entity2.first)
				continue;
			else
			{
				free_id_found = false;
				break;
			}
		if (free_id_found == true)
			break;
		id++;
	}
	return id;
}

void World::addHours(int hours)
{
	hour += hours;
	if (int add_day = hour / 24)
	{
		this->day += add_day;
		hour %= 24;
		for (auto& entity : entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				character.addSatiety(-(int)character.getConsumption()); // decrease satiety on consumption
				feedCharacter(character);

				// triggers
				character.trigger.satiety = true;
				trigger.food = true;
				if (character.getSatiety() == 0)
					character.addHealth(-(int)character.getMaxHealth() / 10);

				// healing if characters have not full health
				if (character.getHealth() < character.getMaxHealth())
				{
					int health_flaw = character.getMaxHealth() - character.getHealth();
					int can_be_added_health = character.getMaxHealth() / 10;
					if (health_flaw <= can_be_added_health)
					{
						character.addSatiety(-health_flaw);
						character.addHealth(health_flaw);
					}
					else
					{
						character.addSatiety(-can_be_added_health);
						character.addHealth(can_be_added_health);
					}
					feedCharacter(character);
				}
			}
		trigger.day = true;
	}
	if (status == Peace)
	{
		if (hour >= fight_start || hour < fight_end)
		{
			zombie_spawn_timer = SDL_GetTicks64();
			status = Fight;
		}
			minutes = 0;
	}
	else if (status == Fight)
		if (hour >= fight_start || hour < fight_end)
			minutes = 0;
	trigger.time = true;
}

void World::addMinutes(int minutes)
{
	this->minutes += minutes;
	if (int add_hours = this->minutes / 60)
	{
		addHours(add_hours);
		this->minutes %= 60;
	}
	trigger.time = true;
}

void World::update()
{
	ticks++;

	updateZombies();
	updateCharacters();

	// check if characters are alive
	for (auto entity = entities.begin(); entity != entities.end();)
	{
		if (typeid(*entity->second) == typeid(Character))
			if (dynamic_cast<Character&>(*entity->second).trigger.dead)
			{
				entity = entities.erase(entity);
				trigger.characters_list_changed = true;
			}
		if (entity != entities.end())
			entity++;
	}
	// check if zombies are alive
	for (auto entity = entities.begin(); entity != entities.end();)
	{
		if (typeid(*entity->second) == typeid(Zombie))
			if (dynamic_cast<Zombie&>(*entity->second).trigger.dead)
				entity = entities.erase(entity);
		if (entity != entities.end())
			entity++;
	}

	if (status == Fight)
		spawnZombies();

	if (ticks % 5 == 0) // 1 real second = 4 game minutes
		if (status == Fight)
		{
			// add time
			if (hour >= fight_end && hour < fight_start)
			{
				status = Peace;
				return;
			}
			addMinutes(1);
		}
}

bool World::addFood(int food)
{
	trigger.food = true;
	if (this->food + food < 0)
		return false;
	else 
		this->food += food;
	return true;
}

bool World::addResources(int resources) // return value: true - if resources was added, false - not added
{
	if (areZombiesExist())
		return false;
	trigger.resources = true;
	if (this->resources + resources < 0)
		return false;
	else
		this->resources += resources;
	return true;
}

void World::searchFood()
{
	if (hour >= fight_start || hour < fight_end || areZombiesExist())
		return;
	int found_food = 0;
	for (auto& entity : entities)
		if (typeid(*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			int survival_level = character.getSurvivalLvl();
			if (rand() % 101 > 33 - survival_level)
				found_food += 1;
			found_food += rand() % (survival_level + 1);
			character.add_survival_xp(100);
		}
	addFood(found_food);
	addHours(1);
}

void World::searchResources()
{
	if (hour >= fight_start || hour < fight_end || areZombiesExist())
		return;
	int found_resources = 0;
	for (auto& entity : entities)
		if (typeid (*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			if (rand() % 101 > 66 - (int)character.getSurvivalLvl())
				found_resources += 1;
			if (character.getSurvivalLvl() > 0)
				found_resources += rand() % character.getSurvivalLvl() + 1;
			character.add_survival_xp(100);
		}
	addResources(found_resources);
	addHours(1);
}

void World::feedCharacter(Character& character)
{
	int need_satiety = character.getMaxSatiety() - character.getSatiety(); // need satiety to max satiety
	int can_be_added_satiety;
	food - need_satiety >= 0 ? can_be_added_satiety = need_satiety : can_be_added_satiety = food;
	food -= can_be_added_satiety;
	character.addSatiety(can_be_added_satiety);
}

void World::addCharacter()
{
	int id = findFreeId();
	int gender = rand() % 2;
	unsigned int max_health = 50 + rand() % 11 * 10;
	if (gender == Character::Male)
		max_health += 10;
	unsigned int health = max_health;
	float speed_per_tact = 4.0f + (float)(rand() % 101) / 100; // min - 4.0f, max - 5.0f
	unsigned int strength = 5; // minimum melee attack
	unsigned int chance = rand() % 101;
	if (chance > 90)
		strength += rand() % 41;
	else if (chance > 66)
		strength += rand() % 31;
	else if (chance > 33)
		strength += rand() % 21;
	else
		strength += rand() % 16;
	unsigned int consumption = (int)(5 + (max_health - 50) * 0.05f);
	unsigned int max_satiety = consumption * 10;
	unsigned int satiety = max_satiety;
	std::string name = "";

	std::string names_tree = "";
	if (gender == Character::Sex::Male)
		names_tree = "male";
	else if (gender == Character::Sex::Female)
		names_tree = "female";
	int names_count = (int)(res->lang["names"][names_tree].size());
	if (names_count > 0) // check if json file contain names
		name = res->lang["names"][names_tree].at(rand() % names_count); // get random name

	// check for name coicidence with exist names 
	int coicidence_count = 0; // how much same names found
	for (auto& entity : entities)
		if (typeid(*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			if (character.getName().find(name) != -1) // true = found same name in characters
			{
				coicidence_count++;
				std::string new_name = name + " " + std::to_string(coicidence_count);
				character.setName(new_name);
			}
		}
	if (coicidence_count != 0) // change name for current character if coicidence found
		name += " " + std::to_string(coicidence_count + 1);

	// get random weapon hidden name
	std::string weapon_hidden_name = res->weapons_data["samples"][rand() % res->weapons_data["samples"].size()];

	// get random json object name for textures
	SDL_Texture* texture = res->entities_textures[res->entities_data["characters"][rand() % res->entities_data["characters"].size()]].texture;

	std::shared_ptr<Character> character(new Character(res, settings, gender, max_health, health, speed_per_tact, strength, texture, name,
		weapon_hidden_name, consumption, max_satiety, satiety, 0u, 0u, 0u, 0u));
	entities[id] = character;
	trigger.characters_list_changed = true;
}

void World::removeCharacter(std::string name)
{
	std::string real_name = name;
	if (name.find(" ") != -1)
		real_name.erase(name.find(" ")); // erase to first space
	int coicidence_count = 0;
	for(auto entity = entities.begin(); entity != entities.end(); entity++)
		if (typeid(*entity->second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity->second);
			if (character.getName() == name)
				entities.erase(entity);
			else if (character.getName().find(real_name) != -1 && character.getName().size() != real_name.size()) // rename all same names
			{
				coicidence_count++;
				std::string new_name = real_name + " " + std::to_string(coicidence_count);
				character.setName(new_name);
			}
		}
	if (coicidence_count < 2) // if count of same words were 2 than coicidence_count will be 0 or 1 depending on the position of search word
		for (auto& entity : entities)
			if (typeid(*entity.second) == typeid(Character))
			{
				Character& character = dynamic_cast<Character&>(*entity.second);
				if (character.getName().find(real_name) != -1)
					character.setName(real_name);
			}
	trigger.characters_list_changed = true;
}

void World::spawnZombies()
{
	if (SDL_GetTicks64() < zombie_spawn_timer)
		return;
	int power = 100 + 20 * day;
	int group_power = 20 + (float)day * 0.5;

	// setting next timer to spawn zombies
	zombie_spawn_timeout = 120 / ((float)power / group_power + rand() % 10) * 1000;
	zombie_spawn_timer += zombie_spawn_timeout;

	// power ranges of zombies
	int range_weak[2] = { 5, 15 };
	int range_simple[2] = { 15, 25 };
	int range_strong[2] = { 25, 35 };

	std::vector<int>zombies_weak;
	std::vector<int>zombies_simple;
	std::vector<int>zombies_strong;

	while (group_power >= range_weak[0])
	{
		int chance = rand() % 101; // chance to spawn some type of zombie
		if (chance >= 40 && group_power >= range_weak[0])
		{
			if (group_power < range_weak[1]) // decrease the appearing chance of unused power
			{
				zombies_weak.push_back(group_power);
				group_power = 0;
				break;
			}
			int zombie_power = range_weak[0] + rand() % (range_weak[1] - range_weak[0] + 1) % (group_power + 1);
			zombies_weak.push_back(zombie_power);
			group_power -= zombie_power;
		}
		else if (chance >= 10 && group_power >= range_simple[1])
		{
			int zombie_power = range_simple[0] + rand() % (range_simple[1] - range_simple[0] + 1) % (group_power + 1);
			zombies_simple.push_back(zombie_power);
			group_power -= zombie_power;
		}
		else if (chance >= 0 && group_power >= range_strong[1])
		{
			int zombie_power = range_strong[0] + rand() % (range_strong[1] - range_strong[0] + 1) % (group_power + 1);
			zombies_strong.push_back(zombie_power);
			group_power -= zombie_power;
		}
	}
	// if unused power exist then give it random zombie
	if (group_power > 0)
	{
		if (zombies_weak.size() > 0) { zombies_weak.at(rand() % zombies_weak.size()) += group_power; }
		else if (zombies_simple.size() > 0) { zombies_simple.at(rand() % zombies_simple.size()) += group_power; }
		else if (zombies_strong.size() > 0) { zombies_strong.at(rand() % zombies_strong.size()) += group_power; }
		group_power = 0;
	}

	int health;
	int strength;
	float speed;
	// power splitting on health, strength and speed for each type of zombie
	for (auto& it : zombies_weak)
	{
		float zombie_power = it;
		float health_factor = 0.4f + (float)(rand() % 21) / 100;
		health = 10 + zombie_power * 10 * health_factor;
		zombie_power *= 1 - health_factor;
		float strength_factor = 0.2f + (float)(rand() % 81) / 100;
		strength = 2 + zombie_power * 2 * strength_factor;
		zombie_power *= 1 - strength_factor;
		speed = 3.0f + zombie_power * 0.1f;

		std::shared_ptr<Zombie>zombie(new Zombie(res, settings, health, speed, strength, 
			res->entities_textures[res->entities_data["zombies"].at(rand() % res->entities_data["zombies"].size())].texture));
		zombie->setX(1920);
		zombie->setY(rand() % 1081 + zombie->getDestRect().h / settings->getScale() / 2);
		entities[findFreeId()] = zombie;
	}

	for (auto& it : zombies_simple)
	{
		float zombie_power = it;
		float health_factor = 0.5f + (float)(rand() % 21) / 100;
		health = 10 + zombie_power * 10 * health_factor;
		zombie_power *= 1 - health_factor;
		float strength_factor = 0.2f + (float)(rand() % 81) / 100;
		strength = 2 + zombie_power * 2 * strength_factor;
		zombie_power *= 1 - strength_factor;
		speed = 3.0f + zombie_power * 0.1f;

		std::shared_ptr<Zombie>zombie(new Zombie(res, settings, health, speed, strength,
			res->entities_textures[res->entities_data["zombies"].at(rand() % res->entities_data["zombies"].size())].texture));
		zombie->setX(1900);
		zombie->setY(zombie->getDestRect().h / settings->getScale() + rand() % 1080 - 2 * zombie->getDestRect().h / settings->getScale());
		entities[findFreeId()] = zombie;
	}

	for (auto& it : zombies_strong)
	{
		float zombie_power = it;
		float health_factor = 0.5f + (float)(rand() % 21) / 100;
		health = 10 + zombie_power * 10 * health_factor;
		zombie_power *= 1 - health_factor;
		float strength_factor = 0.2f + (float)(rand() % 81) / 100;
		strength = 2 + zombie_power * 2 * strength_factor;
		zombie_power *= 1 - strength_factor;
		speed = 3.0f + zombie_power * 0.1f;

		std::shared_ptr<Zombie>zombie(new Zombie(res, settings, health, speed, strength,
			res->entities_textures[res->entities_data["zombies"].at(rand() % res->entities_data["zombies"].size())].texture));
		zombie->setX(1900);
		zombie->setY(zombie->getDestRect().h / settings->getScale() + rand() % 1080 - 2 * zombie->getDestRect().h / settings->getScale());
		entities[findFreeId()] = zombie;
	}
}

void World::updateZombies()
{
	for (auto& entity : entities)
		if (typeid(*entity.second) == typeid(Zombie))
		{
			Zombie& zombie = dynamic_cast<Zombie&>(*entity.second);
			int nearest_character_id = findEntity(entity.first, typeid(Character), Nearest);
			if (nearest_character_id != -1)
				zombie.setTarget(nearest_character_id);
			else
				zombie.setTarget(-1); // if no characters exist

			int target_id = zombie.getTarget();
			if (target_id != -1) // if target set
			{
				float x1 = zombie.getX(), y1 = zombie.getY();
				float x2 = entities[target_id]->getX(), y2 = entities[target_id]->getY();
				float dist_x = x2 - x1;
				float dist_y = y2 - y1;
				float distance = sqrt(pow(dist_x, 2) + pow(dist_y, 2));
				if (distance > melee_distance) // moving to target
				{
					zombie.addX(dist_x / (abs(dist_x) + abs(dist_y)) * zombie.getSpeedPerTick());
					zombie.addY(dist_y / (abs(dist_x) + abs(dist_y)) * zombie.getSpeedPerTick());
				}
				else if (SDL_GetTicks64() > zombie.getMeleeTimer()) // attack target
				{
					zombie.addMeleeTimer(zombie.getMeleeTimeout());
					entities[zombie.getTarget()]->hit(-(int)zombie.getStrength());
				}
			}

			// passive update
			if (SDL_GetTicks64() > zombie.getMeleeTimer())
				zombie.addMeleeTimer(zombie.getMeleeTimeout());
		}
}

void World::updateCharacters()
{
	for (auto& entity : entities)
		if (typeid(*entity.second) == typeid(Character))
		{
			Character& character = dynamic_cast<Character&>(*entity.second);
			int nearest_zombie_id = findEntity(entity.first, typeid(Zombie), Nearest);
			int before_entity = character.getTarget();
			character.setTarget(nearest_zombie_id);
			if (before_entity == -1 && character.getTarget() != -1)
			{
				if (character.getWeapon() != nullptr)
				{
					const type_info& wpn_type = typeid(*character.getWeapon());
					if (wpn_type == typeid(Weapon_SingleShot))
					{
						Weapon_SingleShot& weapon = dynamic_cast<Weapon_SingleShot&>(*character.getWeapon());
						if (weapon.getShotTimer() < SDL_GetTicks64())
							weapon.setShotTimer(SDL_GetTicks64());
					}
					else if (wpn_type == typeid(Weapon_BurstFire))
					{
						Weapon_BurstFire& weapon = dynamic_cast<Weapon_BurstFire&>(*character.getWeapon());
						if (weapon.getBurstTimer() < SDL_GetTicks64())
							weapon.setBurstTimer(SDL_GetTicks());
						if (weapon.getShotTimer() < SDL_GetTicks64())
							weapon.setShotTimer(SDL_GetTicks64());
					}
					else if (wpn_type == typeid(Weapon_Shotgun))
					{
						Weapon_Shotgun& weapon = dynamic_cast<Weapon_Shotgun&>(*character.getWeapon());
						if (weapon.getShotTimer() < SDL_GetTicks64())
							weapon.setShotTimer(SDL_GetTicks64());
					}
				}
			}
				if (nearest_zombie_id != -1) 
				{
					float character_x = character.getX(), character_y = 1920 - character.getY(), zombie_x = entities[nearest_zombie_id]->getX(), 
						zombie_y = 1920 - entities[nearest_zombie_id]->getY();

					float distance = Calc2dDistance(character_x, character_y, zombie_x, zombie_y);
					if (distance <= melee_distance) // melee attack
					{
						if (SDL_GetTicks64() > character.getMeleeTimer())
						{
							entities[nearest_zombie_id]->hit(-(int)character.getStrength());
							character.addMeleeTimer(character.getMeleeTimeout());
						}
					}
					else if (character.getWeapon() != nullptr) // if weapon exist
					{
						float angle = (int)(acos(((10 * (-character_x + zombie_x)) / (10 * sqrt(pow(-character_x + zombie_x, 2) + pow(-character_y + zombie_y, 2)))))
							* 180 / M_PI) % 90;
						character.setWeaponAngle(angle); // rotate weapon to nearest zombie

						const type_info& wpn_type = typeid(*character.getWeapon());
						if (wpn_type == typeid(Weapon_SingleShot)) // single shot weapons
						{
							Weapon_SingleShot& weapon = dynamic_cast<Weapon_SingleShot&>(*character.getWeapon());
							if (weapon.getShotTimer() < SDL_GetTicks64())
							{
								weapon.addShotTimer(weapon.getShotTimeout());
								if (distance <= character.getWeapon()->getMaxDistance())
									if (rand() % 101 <= weapon.getShotAccuracy() + character.getCombatLvl())
									{
										entities[nearest_zombie_id]->hit(-(int)weapon.getShotDamage());
										character.add_combat_xp(100);
										character.setShooting(true);
										character.setShootingTimer(SDL_GetTicks64() + character.getShootingTimeout());
									}
							}
						}
						else if (wpn_type == typeid(Weapon_BurstFire)) // burst fire weapons
						{
							Weapon_BurstFire& weapon = dynamic_cast<Weapon_BurstFire&>(*character.getWeapon());
							if (weapon.getShotTimer() < SDL_GetTicks64())
							{
								weapon.addShotTimer(weapon.getShotTimeout());
								if (weapon.getBurstTimer() < SDL_GetTicks64()) // if false then burst reloading is now
									if (distance <= weapon.getMaxDistance())
										if (weapon.getBurstStep() < weapon.getBurstLength())
										{
											if (rand() % 101 <= weapon.getShotAccuracy() + character.getCombatLvl())
											{
												entities[nearest_zombie_id]->hit(-(int)weapon.getShotDamage());
												character.add_combat_xp(50);
												character.setShooting(true);
												character.setShootingTimer(SDL_GetTicks64() + character.getShootingTimeout());
											}
											weapon.addBurstStep(1);
										}
										else
										{
											weapon.setBurstStep(0);
											weapon.addBurstTimer(weapon.getBurstTimeout());
										}
							}
						}
						else if (wpn_type == typeid(Weapon_Shotgun)) // shotguns
						{
							Weapon_Shotgun& weapon = dynamic_cast<Weapon_Shotgun&>(*character.getWeapon());
							if (weapon.getShotTimer() < SDL_GetTicks64())
							{
								weapon.addShotTimer(weapon.getShotTimeot());
								if (distance <= weapon.getMaxDistance())
								{
									int damage = 0;
									int pellet_count = weapon.getPelletCount(), accuracy = weapon.getPelletAccuracy() + character.getCombatLvl(),
										pellet_damage = -(int)weapon.getPelletDamage();
									for (int i = 0; i < pellet_count; i++) // calculating hit chance for every pellet
										if (rand() % 101 < accuracy)
											damage += pellet_damage;
									if (damage)
									{
										entities[nearest_zombie_id]->hit(damage);
										character.add_combat_xp(100);
										character.setShooting(true);
										character.setShootingTimer(SDL_GetTicks64() + character.getShootingTimeout());
									}
								}
							}
						}
					}
				}

			// passive update 
			if (SDL_GetTicks64() > character.getMeleeTimer())
				character.addMeleeTimer(character.getMeleeTimeout());
		}
}

int World::findEntity(int basic_entity_id, const type_info& type_search_entity, int search_type)
{
	int found_id = -1;
	float distance = 0;
	float x1 = entities[basic_entity_id]->getX(), y1 = entities[basic_entity_id]->getY();

	for (auto& entity : entities)
		if (typeid(*entity.second) == type_search_entity)
		{
			if (found_id == -1) // selecting fist found entity
			{
				distance = Calc2dDistance(x1, y1, entity.second->getX(), entity.second->getY());
				found_id = entity.first;
			}
			if (search_type == Nearest)
			{
				float new_distance = Calc2dDistance(x1, y1, entity.second->getX(), entity.second->getY());
				if (new_distance < distance)
				{
					distance = new_distance;
					found_id = entity.first;
				}
			}
			else if (search_type == Farthest)
			{
				float new_distance = Calc2dDistance(x1, y1, entity.second->getX(), entity.second->getY());
				if (new_distance > distance)
				{
					distance = new_distance;
					found_id = entity.first;
				}
			}
		}
	return found_id;
}

bool World::areZombiesExist()
{
	return std::any_of(entities.begin(), entities.end(), [](std::pair<const int, std::shared_ptr<Entity>>& entity) {
		return typeid(*entity.second) == typeid(Zombie); });
}

// class Entity
Entity::Entity(Resources* res, Settings* settings, unsigned int max_health, unsigned int health, float speed_per_tick, 
	unsigned int strength, SDL_Texture* texture, int melee_timeout) :
	res(res), settings(settings), max_health(max_health), health(health), speed_per_tick(speed_per_tick), strength(strength), texture(texture),
	melee_timeout(melee_timeout)
{
	SDL_Rect tex_size{ 0 };
	SDL_QueryTexture(texture, nullptr, nullptr, &tex_size.w, &tex_size.h);
	dstrect.w = tex_size.w;
	dstrect.h = tex_size.h;
	dstrect.w /= settings->getScale();
	dstrect.h /= settings->getScale();

	// start time for timers
	melee_timer = SDL_GetTicks64();
};

Entity::~Entity() {};

void Entity::setX(float x)
{
	if (is_hit)
		is_hit = false;
	dstrect.x = x;
}

void Entity::setY(float y)
{
	if (is_hit)
		is_hit = false;
	dstrect.y = y;
}

void Entity::addX(float x)
{
	if (!x)
		return;
	if (is_hit)
		is_hit = false;
	if (x < 0)
		angle = 180;
	else if (x > 0)
		angle = 0;
	dstrect.x += x;
}

void Entity::addY(float y)
{
	if (!y)
		return;
	if (is_hit)
		is_hit = false;
	dstrect.y += y;
}

void Entity::hit(int damage)
{
	addHealth(damage);
	is_hit = true;
	hit_pos.x = dstrect.x / settings->getScale() + rand() % (int)(dstrect.w - res->icons_textures["hit"].dstrect.w);
	hit_pos.y = dstrect.y / settings->getScale() + rand() % (int)(dstrect.h - res->icons_textures["hit"].dstrect.h);
	hit_timer = SDL_GetTicks64() + 200;
}

// class Character
Character::Character(Resources* res, Settings* settings, int gender, unsigned int max_health, unsigned int health, float speed, unsigned int strength, 
	SDL_Texture* texture, std::string name, std::string weapon_name, unsigned int consumption, unsigned int max_satiety, 
	unsigned int satiety, unsigned int combat_lvl, unsigned int combat_xp, unsigned int survival_lvl,  unsigned int survival_xp) : 
	Entity(res, settings, max_health, health, speed, strength, texture, 750), name(name), weapon(weapon), consumption(consumption), 
	max_satiety(max_satiety), satiety(satiety), combat_lvl(combat_lvl), survival_lvl(survival_lvl)
{
	calc_combat_xp_upgrade();
	add_combat_xp(combat_xp);
	calc_survival_xp_upgrade();
	add_survival_xp(survival_xp);
	setWeapon(weapon_name);
};

Character::~Character() 
{
	weapon->~Weapon();
};

void Character::add_combat_xp(unsigned int combat_xp)
{
	this->combat_xp += combat_xp;
	trigger.combat_xp = true;
	while (this->combat_xp >= combat_xp_upgrade)
	{
		this->combat_xp -= combat_xp_upgrade;
		combat_lvl++;
		trigger.combat_lvl = true;
		calc_combat_xp_upgrade();
	}
}

void Character::add_survival_xp(unsigned int survival_xp)
{
	this->survival_xp += survival_xp;
	trigger.survival_xp = true;
	while (this->survival_xp >= survival_xp_upgrade)
	{
		this->survival_xp -= survival_xp_upgrade;
		survival_lvl++;
		trigger.survival_lvl = true;
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

int Character::addSatiety(int satiety)
{
	if ((int)this->satiety + satiety > 100)
	{
		int spent_satiety = 100 - this->satiety;
		this->satiety = 100;
		return spent_satiety;
	}
	else if ((int)this->satiety + satiety < 1)
	{
		trigger.dead = true;
		return this->satiety;
	}
	this->satiety += satiety;
	return satiety;
}

bool Character::setSatiety(unsigned int satiety)
{
	if (satiety < 1 || satiety > max_satiety)
		return false;
	this->satiety = satiety;
	return true;
}

void Character::addHealth(int health)
{
	if ((int)this->health + health <= 0)
	{
		this->health = 0;
		trigger.dead = true;
	}
	else if (this->health + health > max_health)
		this->health = max_health;
	else
		this->health += health;
	trigger.health = true;
}

void Character::setWeapon(std::string hidden_name)
{
	if (hidden_name == "") // empty name mean character does not have weapon
	{
		weapon->~Weapon();
		return;
	}
	std::string weapon_type = res->weapons_data[hidden_name]["type"];
	weapon = nullptr;
	if (weapon_type == "single-shot")
		weapon = new Weapon_SingleShot(hidden_name, res->weapons_data, res->weapons_textures);
	else if (weapon_type == "burst-fire")
		weapon = new Weapon_BurstFire(hidden_name, res->weapons_data, res->weapons_textures);
	else if (weapon_type == "shotgun")
		weapon = new Weapon_Shotgun(hidden_name, res->weapons_data, res->weapons_textures);
}

void Character::setWeaponAngle(int angle)
{
	if (angle < -90 || angle > 90)
		return;
	weapon_angle = angle;
}

// class Weapon
Weapon::~Weapon() {};

Weapon::Weapon(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures) : hidden_name(hidden_name)
{
	name = weapons_data[hidden_name]["name"];
	max_distance = weapons_data[hidden_name]["max_distance"];
	tex_simple = weapons_textures[hidden_name + " S"].texture;
	tex_fire = weapons_textures[hidden_name + " F"].texture;
	dstrect = weapons_textures[hidden_name + " S"].dstrect;
};

// class Weapon_SingleShot

Weapon_SingleShot::Weapon_SingleShot(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures) 
	: Weapon(hidden_name, weapons_data, weapons_textures)
{
	shot_damage = weapons_data[hidden_name]["shot_damage"];
	shot_accuracy = weapons_data[hidden_name]["shot_accuracy"];
	shot_timeout = weapons_data[hidden_name]["shot_timeout"];
};

// class Weapon_BurstFire

Weapon_BurstFire::Weapon_BurstFire(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures)
	: Weapon(hidden_name, weapons_data, weapons_textures)
{
	shot_damage = weapons_data[hidden_name]["shot_damage"];
	shot_accuracy = weapons_data[hidden_name]["shot_accuracy"];
	shot_timeout = weapons_data[hidden_name]["shot_timeout"];
	burst_length = weapons_data[hidden_name]["burst_length"];
	burst_timeout = weapons_data[hidden_name]["burst_timeout"];
};

// class Weapon_Shotgun

Weapon_Shotgun::Weapon_Shotgun(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures)
	: Weapon(hidden_name, weapons_data, weapons_textures)
{
	pellet_damage = weapons_data[hidden_name]["pellet_damage"];
	pellet_accuracy = weapons_data[hidden_name]["pellet_accuracy"];
	pellet_count = weapons_data[hidden_name]["pellet_count"];
	shot_timeout = weapons_data[hidden_name]["shot_timeout"];
};

// class Zombie
Zombie::Zombie(Resources* res, Settings* settings, unsigned int health, float speed, unsigned int strength, SDL_Texture* texture) :
	Entity(res, settings, health, health, speed, strength, texture, 1000) 
{
};

Zombie::~Zombie() {};	

void Zombie::addHealth(int health)
{
	if ((int)this->health + health <= 0)
	{
		this->health = 0;
		trigger.dead = true;
	}
	else if (this->health + health > max_health)
		this->health = max_health;
	else
		this->health += health;
}