#include <iostream>
#include "gtest/gtest.h"
#include "hummingbird/solve/Unit.h"

using namespace std;
using namespace hummingbird;

TEST(UnitTest, Start)
{
	Unit<float, Mass> m1(1.5);
	Unit<float, Mass> m2(1.4);
	Unit<float, Length> l1(1.1);
	Unit<float, Time> t1(1.9);
	auto vel = l1 / t1;
	Unit<float, DivType<Length,Time>> vel2 = l1 / t1;
	auto vel3 = l1 / t1;

	cout << "vel: " << vel + vel3 << endl;

	Dimension<1,0,0> d1;
	Dimension<0,1,0> d2;
	d1 * d2;

	MulType<Dimension<1,0,0>, Dimension<0,1,0>> d3;
	MulType<Mass, Length> ML;

	Unit<float, MulType<Dimension<1,0,0>, Dimension<0,1,0>>> u1(2);
	Unit<float, MulType<Mass, Length>> u2(2);
}
