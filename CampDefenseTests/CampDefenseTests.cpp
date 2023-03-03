#include "pch.h"
#include "CppUnitTest.h"
#include "../CampDefense/Functions.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FunctionTests
{
	TEST_CLASS(Functions)
	{
	public:
		TEST_METHOD(FuncFloatToStr)
		{
			float float_pi = 3.141592653589793238462643;
			std::string str_pi = FloatToStr(float_pi, 2);
			Assert::IsTrue(str_pi == "3.14");
		}
		TEST_METHOD(SDL_RectOperatorDivideEqual)
		{
			SDL_Rect rect{ 100, 200, 30, 60 };
			rect /= 1.5f;
			Assert::IsTrue(rect.x == 66 && rect.y == 133 && rect.w == 20 && rect.h == 40);
		}
		TEST_METHOD(FuncCalc2dDistance)
		{
			Assert::IsTrue(FloatToStr(Calc2dDistance(2.0f, 3.0f, -1.0f, 4.0f), 4) == "3.1622");
		}
	};
}
