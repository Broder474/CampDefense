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
	enum search_type { Nearest, Farthest };

	World(Resources* resources, Settings* settings, unsigned int day, unsigned int hour);
	std::map<int, std::shared_ptr<Entity>>entities;
	int findFreeId();
	int findEntity(int basic_entity_id, const type_info& type_search_entity, int search_type);
	void addHours(int hours);
	void addMinutes(int minutes);
	int getDay() { return day; }
	int getHour() { return hour; }
	int getMinutes() { return minutes; }
	Uint64 getTicks() { return ticks; }
	void update();

	int getFood() { return food; }
	bool addFood(int food);
	int getResources() { return resources; }
	bool addResources(int resources);
	void searchFood();
	void searchResources();

	void addCharacter();
	void removeCharacter(std::string name);
	void spawnZombies();
	void updateZombies();

	struct Trigger
	{
		bool characters_list_changed = false; // trigger to update characters outside

		bool day = false;
		bool time = false;
		bool resources = false;
		bool food = false;
	}trigger;

private:
	Resources* res = nullptr;
	Settings* settings = nullptr;
	int day = 0;
	int hour = 0;
	Uint64 ticks = 0;
	enum {Peace, Fight};
	int status = Peace;
	int minutes = 0; // used only during fight to automatically add time
	int fight_start = 22;
	int fight_end = 6;
	int zombie_spawn_timeout; // in milliseconds
	Uint64 zombie_spawn_timer;

	int food = 0;
	int resources = 0;
};

class Entity
{
public:
	Entity(Resources* res, Settings* settings, unsigned int max_health, unsigned int health, float speed_per_tick, unsigned int strength, 
		SDL_Texture* texture, int melee_timeout);
	virtual ~Entity() = 0;
	unsigned int getHealth() { return health; }
	unsigned int getMaxHealth() { return max_health; }
	virtual void addHealth(int health) = 0;
	float getSpeedPerTick() { return speed_per_tick; }
	unsigned int getStrength() { return strength; }
	void setHealth(unsigned int health) { health > max_health ? this->health = max_health : this->health = health; }
	void setX(float x);
	void setY(float y);
	void addX(float x);
	void addY(float y);
	SDL_Texture* getTexture() { return texture; }
	void setDestRect(SDL_FRect dstrect) { this->dstrect = dstrect; }
	SDL_FRect getDestRect() { return dstrect; }
	float getX() { return dstrect.x; }
	float getY() { return dstrect.y; }
	float getAngle() { return angle; }
	void setTarget(int id) { this->target_id = id; }
	int getTarget() { return target_id; }
	int getMeleeTimeout() { return melee_timeout; }
	Uint64 getMeleeTimer() { return melee_timer; }
	void addMeleeTimer(int time) { melee_timer += time; }
	void setMeleeTimer(Uint64 time) { melee_timer = time; }
protected:
	Resources* res = nullptr;
	Settings* settings = nullptr;
	unsigned int max_health = 0;
	unsigned int health = 0;
	int angle = 0;
	float speed_per_tick = 0.0f; // how much cells the entity can pass per one tact
	unsigned int strength = 0; // melee damage
	SDL_Texture* texture = nullptr;
	SDL_FRect dstrect{ 0 };
	int target_id = -1; // target for attack
	int melee_timeout = 0;
	Uint64 melee_timer = 0;
};

class Character : public Entity
{
public:
	Character(Resources* res, Settings* settings, int gender, unsigned int max_health, unsigned int health, float speed, unsigned int strength, 
		SDL_Texture* texture, std::string name, std::string weapon_name,  unsigned int consumption, unsigned int max_satiety,
		unsigned int satiety, unsigned int combat_lvl, unsigned int combat_xp, unsigned int survival_lvl, unsigned int survival_xp);
	~Character() override;

	enum Sex { Male, Female, Undefined };

	std::string getName() { return name; }
	void setName(std::string name) { this->name = name; trigger.name = true; }
	void addHealth(int health) override;
	unsigned int getMaxSatiety() { return max_satiety; }
	unsigned int getSatiety() { return satiety; };
	int addSatiety(int satiety); // return count of used satiety
	bool setSatiety(unsigned int satiety); // return true if success, false - fail
	unsigned int getConsumption() { return consumption; }
	void setWeapon(std::string weapon_name);
	Weapon* getWeapon() { return weapon; }
	int getGender() { return gender; }
	int getWeaponAngle() { return weapon_angle; }
	void setWeaponAngle(int angle);

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

	struct Trigger
	{
		bool name = false;
		bool max_health = false;
		bool health = false;
		bool max_satiety = false;
		bool satiety = false;
		bool consumption = false;
		bool speed_per_second = false;
		bool strength = false;
		bool weapon = false;
		bool dead = false;
		
		bool combat_lvl = false;
		bool combat_xp = false;
		
		bool survival_lvl = false;
		bool survival_xp = false;
	} trigger;

protected:
	int gender = Undefined;
	std::string name = "";
	Weapon* weapon = nullptr; // now used weapon
	int weapon_angle = 0;
	unsigned int consumption = 0; // how much food the character eats
	unsigned int max_satiety = 0;
	unsigned int satiety = 0; // if satiety will be 0 the character will die

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
	Zombie(Resources* res, Settings* settings, unsigned int health, float speed, unsigned int strength, SDL_Texture* texture);
	~Zombie() override;

	void addHealth(int health) override;
private:
};

class Weapon
{
public:
	Weapon(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
	virtual ~Weapon() = 0;

	std::string getName() { return name; }
	std::string getHiddenName() { return hidden_name; }
	unsigned int getMaxDistance() { return max_distance; }
	SDL_Texture* getTextureSimple() { return tex_simple; }
	SDL_Texture* getTextureFire() { return tex_fire; }
	SDL_Rect getDestRect() { return dstrect; }
protected:
	std::string hidden_name = "";
	std::string name = "";
	unsigned int max_distance = 0; // max distance at which the weapon can be fired
	SDL_Texture* tex_simple = nullptr; // texture without shot flash
	SDL_Texture* tex_fire = nullptr; // texture with shot flash
	SDL_Rect dstrect{ 0 };
};

class Weapon_SingleShot : public Weapon
{
public:
	Weapon_SingleShot(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
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
	Weapon_BurstFire(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
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
	Weapon_Shotgun(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
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

class Building
{
public:
	unsigned int getHealth() { return health; }
	unsigned int getArmor() { return armor; }
	int getHeight() { return height; }
protected:
	unsigned int health = 0;
	unsigned int armor = 0;
	int height = 0;
	SDL_Texture* texture = nullptr;
};

class Turret : public Building
{
public:
	Weapon* getWeapon() { return weapon; }
protected:
	Weapon* weapon = nullptr;
};

class Fence : public Building
{

};

class Barbed_Wire : public Building
{
	float slowdown = 0.0f;
	int damage_per_tick = 0;
};