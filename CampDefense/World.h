#include <string>
#include <list>
#include <memory>
#include <SDL.h>
#include "Resources.h"

class Entity;
class Weapon;
class World;
class Character;

class World
{
public:
	enum search_type { Nearest, Farthest };
	enum generate_type { Empty, Random };

	World(Resources* resources, Settings* settings, Uint64 time, const Uint8* keys, unsigned int day, unsigned int hour, int generate_type);
	std::map<int, std::shared_ptr<Entity>>entities;

	// setters and getters
	void setDay(int day) { this->day = day; }
	int getDay() { return day; }
	void setHour(int hour) { this->hour = hour; }
	int getHour() { return hour; }
	void setMinutes(int minutes) { this->minutes = minutes; }
	int getMinutes() { return minutes; }
	Uint64 getTicks() { return ticks; }
	Uint64 getTime() { return time; }
	Uint64* getPtrTime() { return &time; }
	void setTime(Uint64 time) { this->time = time; }
	void setFood(int food) { this->food = food; }
	int getFood() { return food; }
	void setResources(int resources) { this->resources = resources; }
	int getResources() { return resources; }
	void setSelectedId(int id) { selected_id = id; }
	int getSelectedId() { return selected_id; }
	int getStatus() { return status; }
	void setStatus(int status) { this->status = status; }
	void setCharactersExist(bool status) { characters_exist = status; }
	void setZombieSpawnTimeout(int timeout) { this->zombie_spawn_timeout = timeout; }
	int getZombieSpawnTimeout() { return zombie_spawn_timeout; }
	void setZombieSpawnTimer(Uint64 timer) { this->zombie_spawn_timer = timer; }
	Uint64 getZombieSpawnTimer() { return zombie_spawn_timer; }

	void addHours(int hours);
	void addMinutes(int minutes);
	bool addFood(int food);
	bool addResources(int resources);
	void addZombieSpawnTimer(Uint64 time) { zombie_spawn_timer += time; }
	void addRandomCharacter();

	void update();
	int findFreeId();
	int findEntity(int basic_entity_id, const type_info& type_search_entity, int search_type);
	void searchFood();
	void searchResources();
	void feedCharacter(Character& character);
	void removeCharacter(std::string name);
	void spawnZombies();
	void updateZombies();
	void updateCharacters();

	bool areZombiesExist();
	bool areCharactersExist() { return characters_exist; }

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
	const Uint8* keys = nullptr;
	int day = 0;
	int hour = 0;
	Uint64 time = 0;
	Uint64 ticks = 0;
	enum {Peace, Fight};
	int status = Peace;
	int minutes = 0; // used only during fight to automatically add time
	int fight_start = 22; // zombies start attack at 22
	int fight_end = 6; // zombies end attack at 6
	int zombie_spawn_timeout; // in milliseconds
	Uint64 zombie_spawn_timer;
	float melee_distance = 45.0f;
	bool characters_exist = false;
	int selected_id = -1; // selected character id for manual control 

	int food = 0;
	int resources = 0;
};

class Entity
{
public:
	Entity(Resources* res, Settings* settings, Uint64* time, unsigned int max_health, unsigned int health, float speed_per_tick, unsigned int strength, 
		std::string tex_name, int melee_timeout);
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
	int getAngle() { return angle; }
	void setAngle(int angle) { this->angle = angle; }
	void setTarget(int id) { this->target_id = id; }
	int getTarget() { return target_id; }
	void hit(int damage);
	std::string getTextureName() { return tex_name; }

	Uint64* getTime() { return time; }
	void setTime(Uint64* time) { this->time = time; }
	int getMeleeTimeout() { return melee_timeout; }
	Uint64 getMeleeTimer() { return melee_timer; }
	void addMeleeTimer(int time) { melee_timer += time; }
	void setMeleeTimer(Uint64 time) { melee_timer = time; }

	// animation functions
	Uint64 getHitTimer() { return hit_timer; }
	void addHitTimer(Uint64 time) { hit_timer = time; }
	SDL_Point getHitPos() { return hit_pos; }
	bool isHit() { return is_hit; }
	void setHit(bool hit) { is_hit = hit; }
protected:
	Resources* res = nullptr;
	Settings* settings = nullptr;
	unsigned int max_health = 0;
	unsigned int health = 0;
	int angle = 0;
	float speed_per_tick = 0.0f; // how much cells the entity can pass per one tact
	unsigned int strength = 0; // melee damage
	SDL_Texture* texture = nullptr;
	std::string tex_name = "";
	SDL_FRect dstrect{ 0 };
	int target_id = -1; // target for attack
	int melee_timeout = 0;
	Uint64 melee_timer = 0;
	Uint64* time = nullptr;

	// animation variables
	Uint64 hit_timer = 0;
	SDL_Point hit_pos{ 0 };
	bool is_hit = false; // needed for animation of damage receiving
};

class Character : public Entity
{
public:
	Character(Resources* res, Settings* settings, Uint64* time, int gender, unsigned int max_health, unsigned int health, float speed, unsigned int strength,
		std::string tex_name, std::string name, std::string weapon_name,  unsigned int consumption, unsigned int max_satiety,
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

	// animation functions and variables
	Uint64 getShootingTimer() { return shooting_timer; }
	void setShootingTimer(Uint64 time) { shooting_timer = time; }
	void addShootingTimer(Uint64 time) { shooting_timer += time; }
	int getShootingTimeout() { return shooting_timeout; }
	bool isShooting() { return is_shooting; }
	void setShooting(bool shooting) { is_shooting = shooting; }

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

	// animation variables
	Uint64 shooting_timer = 0;
	int shooting_timeout = 50;
	bool is_shooting = false; // needed for changing weapon texture
};

class Zombie : public Entity
{
public:
	Zombie(Resources* res, Settings* settings, Uint64* time, unsigned int max_health, unsigned int health, float speed, unsigned int strength, std::string tex_name);
	~Zombie() override;

	void addHealth(int health) override;

	struct Trigger
	{
		bool dead = false;
	}trigger;
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
	unsigned int getShotTimeout() { return shot_timeout; }
	Uint64 getShotTimer() { return shot_timer; }
	void setShotTimer(Uint64 time) { shot_timer = time; }
	void addShotTimer(Uint64 time) { shot_timer += time; }
private:
	unsigned int shot_damage = 0;
	unsigned int shot_accuracy = 0;
	unsigned int shot_timeout = 0;
	Uint64 shot_timer = 0;
};

class Weapon_BurstFire : public Weapon
{
public:
	Weapon_BurstFire(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
	~Weapon_BurstFire() override {};

	unsigned int getShotDamage() { return shot_damage; }
	unsigned int getShotAccuracy() { return shot_accuracy; }
	unsigned int getShotTimeout() { return shot_timeout; }
	unsigned int getBurstLength() { return burst_length; }
	unsigned int getBurstTimeout() { return burst_timeout; }
	Uint64 getShotTimer() { return shot_timer; }
	void setShotTimer(Uint64 time) { shot_timer = time; }
	void addShotTimer(Uint64 time) { shot_timer += time; }
	Uint64 getBurstTimer() { return burst_timer; }
	void setBurstTimer(Uint64 time) { burst_timer = time; }
	void addBurstTimer(Uint64 time) { burst_timer += time; }
	int getBurstStep() { return burst_step; }
	void addBurstStep(int step) { burst_step += step; }
	void setBurstStep(int step) { burst_step = step; }
private:
	unsigned int shot_damage = 0; // one shot damage
	unsigned int shot_accuracy = 0; // one shot accuracy 
	unsigned int shot_timeout = 0; // time between two shots of one burst
	unsigned int burst_length = 0; // how much bullets a weapon shoots per one burst
	unsigned int burst_timeout = 0; // time between two bursts
	Uint64 shot_timer = 0;
	Uint64 burst_timer = 0;
	int burst_step = 0;
};

class Weapon_Shotgun : public Weapon
{
public:
	Weapon_Shotgun(std::string hidden_name, json weapons_data, std::map<std::string, TextureData>& weapons_textures);
	~Weapon_Shotgun() override {};

	unsigned int getPelletDamage() { return pellet_damage; }
	unsigned int getPelletAccuracy() { return pellet_accuracy; }
	unsigned int getPelletCount() { return pellet_count; }
	unsigned int getShotTimeout() { return shot_timeout; }
	Uint64 getShotTimer() { return shot_timer; }
	void setShotTimer(Uint64 time) { shot_timer = time; }
	void addShotTimer(Uint64 time) { shot_timer += time; }
private:
	unsigned int pellet_damage = 0; // one pellet damage
	unsigned int pellet_accuracy = 0; // pullet accuracy
	unsigned int pellet_count = 0; // count of pellets
	unsigned int shot_timeout = 0; // time between two shots
	Uint64 shot_timer = 0;
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
public:
protected:
	float slowdown = 0.0f;
	int damage_per_tick = 0;
};