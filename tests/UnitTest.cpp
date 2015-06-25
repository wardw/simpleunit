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

	//cout << height * length;

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
	// Implicit conversion - only available when source is an exact multiple of target ratio type
	Quantity<int, ratio<4,3>> a(10);
	Quantity<int, ratio<1,3>> a2(a);
	//Quantity<int, ratio<1,1>> ai3(a); // error
	EXPECT_EQ(10, a.value());
	EXPECT_EQ(40, a2.value());
	cout << "a: " << a << "  a2: " << a2 << endl;

	// // Narrowing of value coefficient - requires explicit cast
	Quantity<float, ratio<1,1>> b(10);
	//Quantity<int, ratio<1,1>> b2(b);
	//cout << "b: " << b << "  b2: " << b2 << endl;

	// If target is floating-point, conversions are always possilbe
	Quantity<int, ratio<4,3>> c(10);
	Quantity<float, ratio<1,1>> c2(c);
	EXPECT_EQ(10, c.value());
	EXPECT_FLOAT_EQ(40.f/3, c2.value());
	cout << "c: " << c << "  c2: " << c2 << endl;

	Quantity<float, ratio<4,3>> d(10);
	Quantity<float, ratio<1,1>> d2(d);
	EXPECT_EQ(10, c.value());
	EXPECT_FLOAT_EQ(40.f/3, c2.value());
	cout << "d: " << d << "  d2: " << d2 << endl;
}

TEST(UnitTest, CompoundAssignment)
{
	// Compound assignment with implicit conversion

	Quantity<int, ratio<4,3>> a(10);
	Quantity<int, ratio<1,3>> a2(40);
	a2 += a;
	// a += a2; // Should not compile: no viable overloaded '+='
	EXPECT_EQ(80, a2.value());

	Quantity<int, ratio<4,3>> c(10);
	Quantity<float, ratio<1,1>> c2(40.f/3);
	c2 += c;
	// c += c2; // Should not compile: no viable overloaded '+='
	EXPECT_FLOAT_EQ(80.f/3, c2.value());

	Quantity<float, ratio<4,3>> d(10);
	Quantity<float, ratio<1,1>> d2(40.f/3);
	d2 += d;
	// d += d2; // Should not compile: no viable overloaded '+='
	EXPECT_FLOAT_EQ(80.f/3, d2.value());
}

TEST(UnitTest, CompoundAssignmentOthers)
{
	Quantity<int, ratio<4,3>> a(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(80, (a2 += a).value());

	Quantity<int, ratio<4,3>> b(10);
	Quantity<int, ratio<1,3>> b2(20);
	EXPECT_EQ(-20, (b2 -= b).value());

	Quantity<int, ratio<4,3>> c(7);
	EXPECT_EQ(14, (c *= 2).value());

	Quantity<int, ratio<4,3>> d(7);
	EXPECT_EQ(3, (d /= 2).value());
	EXPECT_EQ(1, (d /= 2.f).value());

	Quantity<float, ratio<4,3>> e(7);
	EXPECT_FLOAT_EQ(3.5f, (e /= 2).value());
	EXPECT_FLOAT_EQ(1.75f, (e /= 2.f).value());
}

TEST(UnitTest, BinaryAdd)
{
	Quantity<int, ratio<4,3>> a(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(80, (a + a2).value());
	EXPECT_EQ(1, decltype(a+a2)::unit::num);
	EXPECT_EQ(3, decltype(a+a2)::unit::den);

	Quantity<int, ratio<2,3>> b(10);
	Quantity<int, ratio<4,3>> b2(40);
	EXPECT_EQ(90, (b + b2).value());
	EXPECT_EQ(2, decltype(b+b2)::unit::num);
	EXPECT_EQ(3, decltype(b+b2)::unit::den);

	Quantity<int, ratio<7,8>> c(10);
	Quantity<int, ratio<8,9>> c2(40);
	EXPECT_EQ(3190, (c + c2).value());
	EXPECT_EQ(1, decltype(c+c2)::unit::num);
	EXPECT_EQ(72, decltype(c+c2)::unit::den);
	auto value = (c+c2).value();
	EXPECT_EQ(1, std::is_integral<decltype(value)>::value);

	Quantity<int, ratio<7,8>> d(10);
	Quantity<float, ratio<8,9>> d2(40);
	EXPECT_EQ(3190, (d + d2).value());
	auto value2 = (d + d2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(value2)>::value);
}

TEST(UnitTest, BinaryMultiply)
{
	Quantity<int, ratio<4,3>> a(10);
	EXPECT_EQ(20, (a * 2).value());
	EXPECT_EQ(20, (2 * a).value());

	Quantity<float, ratio<4,3>> b(10.25f);
	EXPECT_EQ(20.5f, (b * 2).value());
	EXPECT_EQ(20.5f, (2 * b).value());
}

TEST(UnitTest, BinaryDivide)
{
	Quantity<int, ratio<4,3>> a1(7);
	EXPECT_EQ(3, (a1 / 2).value());  // integer division

	Quantity<int, ratio<4,3>> a2(7);
	EXPECT_EQ(3.5f, (a2 / 2.f).value());

	Quantity<float, ratio<4,3>> a3(7.f);
	EXPECT_EQ(3.5f, (a3 / 2).value());

	Quantity<float, ratio<4,3>> a4(7.f);
	EXPECT_EQ(3.5f, (a4 / 2.f).value());

	Quantity<int, ratio<4,3>> b(7);
	Quantity<int, ratio<4,3>> c(2);
	EXPECT_EQ(3, (b / c));

	Quantity<int, ratio<4,3>> b1(7);
	Quantity<float, ratio<4,3>> c1(2);
	EXPECT_EQ(3.5f, (b1 / c1));

	Quantity<float, ratio<4,3>> b2(7);
	Quantity<int, ratio<4,3>> c2(2);
	EXPECT_EQ(3.5f, (b2 / c2));

	Quantity<float, ratio<4,3>> b3(7);
	Quantity<float, ratio<4,3>> c3(2);
	EXPECT_EQ(3.5f, (b3 / c3));

	// Any division under different bases is a compile error
	Quantity<float, ratio<4,3>> d(7);
	Quantity<int, ratio<1,3>> e(2);
	// d / e;
}

TEST(UnitTest, QuantityCast)
{
	Quantity<int, ratio<4,3>> a(7);
	auto a1 = quantity_cast<Quantity<int, ratio<3,2>>>(a);
	EXPECT_EQ(6, a1.value());
	EXPECT_EQ(3, decltype(a1)::unit::num);
	EXPECT_EQ(2, decltype(a1)::unit::den);

	Quantity<float, ratio<4,3>> b(7);
	auto b1 = quantity_cast<Quantity<int, ratio<3,2>>>(b);
	EXPECT_EQ(6, b1.value());

	Quantity<int, ratio<4,3>> c(7);
	auto c1 = quantity_cast<Quantity<float, ratio<3,2>>>(c);
	EXPECT_FLOAT_EQ(56.f/9, c1.value());

	Quantity<float, ratio<4,3>> d(7);
	auto d1 = quantity_cast<Quantity<float, ratio<3,2>>>(d);
	EXPECT_FLOAT_EQ(56.f/9, d1.value());
}

TEST(UnitTest, MemberCast)
{
	Millimeter a(7);
	auto val = a.as<Meter>();
	EXPECT_EQ(9, a.asVal<Meter>());
	cout << "As Meter: " << a.as<Meter>();
}
