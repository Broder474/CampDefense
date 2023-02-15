#include "pch.h"
#include "CppUnitTest.h"
#include "../CampDefense/World.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WorldTests
{
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
			Weapon_BurstFire weapon1("M4", (unsigned int)35, "M4 S.png", "M4 F.png", (unsigned int)35, (unsigned int)70, 0.3f, (unsigned int)3, 1.2f);
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
			Weapon_Shotgun weapon1("MP-133", (unsigned int)15, "MP-133 S.png", "MP-133 F.png", (unsigned int)8, (unsigned int)60, (unsigned int)10, 0.9f);
			Assert::IsTrue(weapon1.getName() == "MP-133" && weapon1.getMaxDistance() == 15 && weapon1.getPelletDamage() == 8 && weapon1.getPelletAccuracy()
				== 60 && weapon1.getPelletCount() == 10 && weapon1.getShotTimeot() == 0.9f && weapon1.getFileTexSimple() == "MP-133 S.png" &&
				weapon1.getFileTexFire() == "MP-133 F.png");
		}
	};
	TEST_CLASS(World_Tests)
	{
	public:
		TEST_METHOD(addHours)
		{
			World world(0, 25);
			world.addHours(100);
			Assert::IsTrue(world.getDay() == 5 && world.getHour() == 5);
		}
	};
}
