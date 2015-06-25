#include <iostream>
#include <ratio>
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

TEST(UnitTest, Units)
{
	ratio<1,10> r;

	ratio<1,10>::type r1;

	cout << decltype(r1)::num << "/" << r1.den << endl;

	Quantity<float, ratio<1,100>> width(6);
	Centimeter height(2);
	Meter length(5);

	decltype(height)::unit u;

	cout << decltype(u)::num << "/" << u.den << endl;

	cout << height * length;

	ratio_add<ratio<2,5>,ratio<1,3>> r2;
	cout << decltype(r2)::num << "/" << r2.den << endl;

	Quantity<float, ratio<2,5>> a(4);
	Quantity<float, ratio<1,3>> b(2.5);
	cout << "a + b = " << a + b << endl;

	BaseUnit<ratio<2,5>, ratio<1,3>>::type r3;
	cout << "this: " << decltype(r3)::num << "/" << r3.den << endl;

	cout << height + length << endl;

	cout << 20 % 40 << " and " << 41 % 20 << endl;

	cout << "IsMultiple " << IsMultiple<ratio<1,3>, ratio<1,6>>::value << endl;
	//cout << "IsMultipleTest " << IsMultipleTest<ratio<1,23>, ratio<1,22>>::value << endl;

	cout << "std::is_integral<T>::value " << std::is_integral<int>::value << endl;


}

TEST(UnitTest, ImplicitConversion)
{
	Quantity<int, ratio<1,1>> a1(1);
	Quantity<int, ratio<1,3>> a2(1);
	//Quantity<int, ratio<1,1>> a3fail(a2);
	Quantity<int, ratio<1,6>> a3(a2);

	Quantity<int, ratio<1,3>> a4(a1);
	cout << "a4 " << a4 << endl;
	a3 += a4;
	cout << "a3 " << a4 << endl;


}
