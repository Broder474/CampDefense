#include "pch.h"
#include "CppUnitTest.h"
#include "../CampDefense/World.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WorldTests
{
	TEST_CLASS(CharacterTests)
	{
	public:
		TEST_METHOD(Constructor)
		{
			Character character1(100, 2.5f, 20, "Broder", nullptr, 3, 2, 3300, 5, 15500);
			Assert::IsTrue(character1.getCombatLvl() == 3 && character1.getCombatXp() == 300 && character1.getCombatXpUpgrade() == 4000 && 
				character1.getSurvivalLvl() == 7 && character1.getSurvivalXp() == 2500 && character1.getSurvivalXpUpgrade() == 8000 && 
				character1.getConsumption() == 3 && character1.getName() == "Broder" && character1.getHealth() == 100 && character1.getSpeed() == 2.5f && 
				character1.getStrength() == 20 && character1.getWeapon() == nullptr);
		}
	};
	TEST_CLASS(Weapon_SingleShotTests)
	{
	public:
		TEST_METHOD(Constructor)
		{
			Weapon_SingleShot weapon1("Glock 19", 15, "glock 19 S.png", "glock 19 F.png", 25, 60, 1.2f);
			Assert::IsTrue(weapon1.getName() == "Glock 19" && weapon1.getMaxDistance() == 15 && weapon1.getShotDamage() == 25 && weapon1.getShotAccuracy() ==
				60 && weapon1.getShotTimeout() == 1.2f && weapon1.getFileTexSimple() == "glock 19 S.png" && weapon1.getFileTexFire() == "glock 19 F.png");
		}
	};
	TEST_CLASS(Weapon_BurstFireTests)
	{
	public:
		TEST_METHOD(Constructor)
		{
			Weapon_BurstFire weapon1("M4", 35, "M4 S.png", "M4 F.png", 35, 70, 0.3f, 3, 1.2f);
		Assert::IsTrue(weapon1.getName() == "M4" && weapon1.getMaxDistance() == 35 && weapon1.getShotDamage() == 35 && weapon1.getShotAccuracy() == 70 &&
			weapon1.getShotTimeout() == 0.3f && weapon1.getBurstLength() == 3 && weapon1.getBurstTimeout() == 1.2f && weapon1.getFileTexSimple() == 
			"M4 S.png" && weapon1.getFileTexFire() == "M4 F.png");
		}
	};
	TEST_CLASS(Weapon_ShotgunTests)
	{
	public:
		TEST_METHOD(Constructor)
		{
			Weapon_Shotgun weapon1("MP-133", 15, "MP-133 S.png", "MP-133 F.png", 8, 60, 10, 0.9f);
			Assert::IsTrue(weapon1.getName() == "MP-133" && weapon1.getMaxDistance() == 15 && weapon1.getPelletDamage() == 8 && weapon1.getPelletAccuracy()
				== 60 && weapon1.getPelletCount() == 10 && weapon1.getShotTimeot() == 0.9f && weapon1.getFileTexSimple() == "MP-133 S.png" &&
				weapon1.getFileTexFire() == "MP-133 F.png");
		}
	};
}
