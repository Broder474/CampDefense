#include <string>
#include <list>
#include <memory>
#include <SDL.h>
#include "Resources.h"

class Entity;
class Weapon;
class World;

class World
{
public:
	World(unsigned int day, unsigned int hour);
	std::list<std::shared_ptr<Entity>> entities;
	void addHours(int hours);
	int getDay() { return day; }
	int getHour() { return hour; }
	Uint64 getTicks() { return ticks; }
	void update();

	int getProducts() { return products; }
	bool addProducts(int products);
	int getMaterials() { return materials; }
	bool addMaterials(int materials);
	void searchProducts();
	void searchMaterials();
private:
	int day = 0;
	int hour = 0;
	Uint64 ticks = 0;

	int products = 0;
	int materials = 0;
};

class Entity
{
public:
	Entity(unsigned int health, float speed_per_tick, unsigned int strength): health(health), speed_per_tick(speed_per_tick), strength(strength) 
	{
		
	};
	virtual ~Entity() = 0;
	unsigned int getHealth() { return health; }
	float getSpeedPerTick() { return speed_per_tick; }
	unsigned int getStrength() { return strength; }
protected:
	unsigned int health = 0;
	float speed_per_tick = 0.0f; // how much cells the entity can pass per one tact
	float speed_per_second = 0.0f; // equal speed_per_tick * tacts per second
	unsigned int strength = 0; // melee damage
};

class Character : public Entity
{
public:
	Character(int gender, unsigned int health, float speed, unsigned int strength, Resources* res, Weapon* weapon, unsigned int consumption,
		unsigned int combat_lvl, unsigned int combat_xp, unsigned int survival_lvl, unsigned int survival_xp);
	~Character() override;

	enum Sex { Male, Female, Undefined };

	std::string getName() { return name; }
	void setName(std::string name) { this->name = name; }
	Weapon* getWeapon() { return weapon; }
	unsigned int getConsumption() { return consumption; }
	int getGender() { return gender; }

	// combat 
	unsigned int getCombatLvl() { return combat_lvl; }
	unsigned int getCombatXp() { return combat_xp; }
	unsigned int getCombatXpUpgrade() { return combat_xp_upgrade; }
	void calc_combat_xp_upgrade();
	void add_combat_xp(unsigned int combat_xp);

	// survival 
	unsigned int getSurvivalLvl() { return survival_lvl; }
	unsigned int getSurvivalXp() { return survival_xp; }
	unsigned int getSurvivalXpUpgrade() { return survival_xp_upgrade; }
	void calc_survival_xp_upgrade();
	void add_survival_xp(unsigned int survival_xp);

private:
	int gender = Undefined;
	std::string name = "";
	Weapon* weapon = nullptr; // now used weapon
	unsigned int consumption = 0; // how much food the character eats

	// combat
	unsigned int combat_lvl = 0; // combat level
	unsigned int combat_xp = 0; // combat xp for now level
	unsigned int combat_xp_upgrade = 0; // combat xp to next level

	// survival
	unsigned int survival_lvl = 0; // survival level
	unsigned int survival_xp = 0; // survival xp to next level
	unsigned int survival_xp_upgrade = 0; // survival xp to next level
};

class Zombie : public Entity
{
public:
	Zombie(unsigned int health, float speed, unsigned int strength): Entity(health, speed, strength) {};
	~Zombie() override;
private:
};

class Weapon
{
public:
	Weapon(std::string name, unsigned int max_distance, const char* file_tex_simple, const char* file_tex_fire) : name(name), max_distance(max_distance),
		file_tex_simple(file_tex_simple), file_tex_fire(file_tex_fire) {};
	virtual ~Weapon() = 0;

	std::string getName() { return name; }
	unsigned int getMaxDistance() { return max_distance; }
	const char* getFileTexSimple() { return file_tex_simple; }
	const char* getFileTexFire() { return file_tex_fire; }
protected:
	std::string name;
	unsigned int max_distance = 0; // max distance at which the weapon can be fired
	const char* file_tex_simple = nullptr; // name of file which contain weapon image without shot flash
	const char* file_tex_fire = nullptr; // name of file which contain weapon image with shot flash
};

class Weapon_SingleShot : public Weapon
{
public:
	Weapon_SingleShot(std::string name, unsigned int max_distance, const char* file_tex_simple, const char* file_tex_fire, unsigned int shot_damage, 
		unsigned int shot_accuracy, float shot_timeout): Weapon(name, max_distance, file_tex_simple, file_tex_fire), shot_damage(shot_damage), 
		shot_accuracy(shot_accuracy), shot_timeout(shot_timeout) {};
	~Weapon_SingleShot() override {};

	unsigned int getShotDamage() { return shot_damage; }
	unsigned int getShotAccuracy() { return shot_accuracy; }
	float getShotTimeout() { return shot_timeout; }
private:
	unsigned int shot_damage = 0;
	unsigned int shot_accuracy = 0;
	float shot_timeout = 0.0f;
};

class Weapon_BurstFire : public Weapon
{
public:
	Weapon_BurstFire(std::string name, unsigned int max_distance, const char* file_tex_simple, const char* file_tex_fire, unsigned int shot_damage, 
		unsigned int shot_accuracy, float shot_timeout, unsigned int burst_length, float burst_timeout): Weapon(name, max_distance, file_tex_simple, 
		file_tex_fire), shot_damage(shot_damage), shot_accuracy(shot_accuracy), shot_timeout(shot_timeout), burst_length(burst_length), 
		burst_timeout(burst_timeout) {};
	~Weapon_BurstFire() override {};

	unsigned int getShotDamage() { return shot_damage; }
	unsigned int getShotAccuracy() { return shot_accuracy; }
	float getShotTimeout() { return shot_timeout; }
	unsigned int getBurstLength() { return burst_length; }
	float getBurstTimeout() { return burst_timeout; }
private:
	unsigned int shot_damage = 0; // one shot damage
	unsigned int shot_accuracy = 0; // one shot accuracy 
	float shot_timeout = 0.0f; // time between two shots of one burst
	unsigned int burst_length = 0; // how much bullets a weapon shoots per one burst
	float burst_timeout = 0.0f; // time between two bursts
};

class Weapon_Shotgun : public Weapon
{
public:
	Weapon_Shotgun(std::string name, unsigned int max_distance, const char* file_tex_simple, const char* file_tex_fire, unsigned int pellet_damage, 
		unsigned int pellet_accuracy, unsigned int pellet_count, float shot_timeout): Weapon(name, max_distance, file_tex_simple, file_tex_fire), 
		pellet_damage(pellet_damage), pellet_accuracy(pellet_accuracy), pellet_count(pellet_count), shot_timeout(shot_timeout) {};
	~Weapon_Shotgun() override {};

	unsigned int getPelletDamage() { return pellet_damage; }
	unsigned int getPelletAccuracy() { return pellet_accuracy; }
	unsigned int getPelletCount() { return pellet_count; }
	float getShotTimeot() { return shot_timeout; }
private:
	unsigned int pellet_damage = 0; // one pellet damage
	unsigned int pellet_accuracy = 0; // pullet accuracy
	unsigned int pellet_count = 0; // count of pellets
	float shot_timeout = 0.0f; // time between two shots
};