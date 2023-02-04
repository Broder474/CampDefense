#include <string>
#include <vector>
#include <memory>

class Entity;
class Weapon;
class World;

class World
{
public:
	std::vector<std::unique_ptr<Entity>> entities;
	int day, hour;
};

class Entity
{
public:
	Entity() {};
	virtual ~Entity() = 0;
protected:
	float speed; // how much cells the entity can pass per one second
	int health;
	int strength; // melee damage
};

class Character : public Entity
{
public:
	Character() {};
	~Character() override;
private:
	std::string name;
	Weapon* weapon; // now used weapon
	int consumption; // how much food the character eats
	int combat_xp; // combat experience
	int combat_lvl; // combat level
	int survival_xp; // survival experience
	int survival_lvl; // survival level
};

class Zombie : public Entity
{
public:
	Zombie() {};
	~Zombie() override;
private:
};

class Weapon
{
public:
	virtual ~Weapon() = 0;
protected:
	std::string name;
	int max_distance; // max distance at which the weapon can be fired
	char* texture_simple; // name of file which contain weapon image without shot flash
	char* texture_fire; // name of file which contain weapon image with shot flash
};

class Weapon_SingleShot : public Weapon
{
public:
	Weapon_SingleShot() {};
	~Weapon_SingleShot() override {};
private:
	int shot_damage;
	int shot_accuracy;
	float shot_timeout;
};

class Weapon_BurstFire : public Weapon
{
public:
	Weapon_BurstFire() {};
	~Weapon_BurstFire() override {};
private:
	int shot_damage; // one shot damage
	int shot_accuracy; // one shot accuracy 
	float shot_timeout; // time between two shots of one burst
	int burst_length; // how much bullets a weapon shoots per one burst
	float burst_timeout; // time between two bursts
};

class Weapon_Shotgun : public Weapon
{
public:
	Weapon_Shotgun() {};
	~Weapon_Shotgun() override {};
private:
	int pellet_damage; // one pellet damage
	int pellet_accuracy; // pullet accuracy
	int pellet_count; // count of pellets
	float shot_timeout; // time between two shots
};