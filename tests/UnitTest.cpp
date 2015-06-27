#include <iostream>
#include <ratio>
#include "gtest/gtest.h"
#include "hummingbird/solve/Unit.h"

using namespace std;
using namespace hummingbird;

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
	Meter b(a);
	EXPECT_FLOAT_EQ(0.007, a.asVal<Meter>());
	cout << "Original: " << a.as() << endl;
	cout << "As Meter: " << a.as<Meter>() << endl;
	cout << "a + b = " << a + b << endl;
	cout << "a + b + c = " << a + b + Centimeter(127) << endl;
	cout << "a + b + c = " << (a + b + Centimeter(127)).as<Centimeter>() << endl;
}

/* For binary operators, currently care about three things. Does it return

+ the right answer (coefficient and base)
+ in the right dimension
+ of the right type

These three axes tested independently

*/

TEST(UnitTest, AddCoeff)
{
	Quantity<int, ratio<4,3>> a1(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(80, (a1+a2).value());
	EXPECT_EQ(1, decltype(a1+a2)::unit::num);
	EXPECT_EQ(3, decltype(a1+a2)::unit::den);

	Quantity<int, ratio<2,3>> b1(10);
	Quantity<int, ratio<4,3>> b2(40);
	EXPECT_EQ(90, (b1+b2).value());
	EXPECT_EQ(2, decltype(b1+b2)::unit::num);
	EXPECT_EQ(3, decltype(b1+b2)::unit::den);

	Quantity<int, ratio<7,8>> c1(10);
	Quantity<int, ratio<8,9>> c2(40);
	EXPECT_EQ(3190, (c1+c2).value());
	EXPECT_EQ(1, decltype(c1+c2)::unit::num);
	EXPECT_EQ(72, decltype(c1+c2)::unit::den);
}

TEST(UnitTest, AddDim)
{
	Quantity<int, ratio<4,3>> a1(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(1, int(decltype(a1+a2)::dim::d1));
	EXPECT_EQ(0, int(decltype(a1+a2)::dim::d2));
	EXPECT_EQ(0, int(decltype(a1+a2)::dim::d3));

	Quantity<int, ratio<4,3>, Dimension<0,1,0>> b1(10);
	Quantity<int, ratio<1,3>, Dimension<0,1,0>> b2(40);
	EXPECT_EQ(0, int(decltype(b1+b2)::dim::d1));
	EXPECT_EQ(1, int(decltype(b1+b2)::dim::d2));
	EXPECT_EQ(0, int(decltype(b1+b2)::dim::d3));

	// Dimension mismatch's should not compile
	Quantity<int, ratio<1,1>, Dimension<1,1,0>> c1(10);
	Quantity<int, ratio<1,1>, Dimension<0,1,0>> c2(40);
	//c1 + c2;
}

TEST(UnitTest, AddType)
{
	Quantity<int, ratio<7,8>> c1(10);
	Quantity<int, ratio<8,9>> c2(40);
	EXPECT_EQ(3190, (c1+c2).value());
	auto result = (c1+c2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Quantity<int, ratio<7,8>> d1(10);
	Quantity<float, ratio<8,9>> d2(40);
	EXPECT_FLOAT_EQ(3190.f, (d1+d2).value());
	auto val2 = (d1+d2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
}

TEST(UnitTest, MultiplyCoeff)
{
	Quantity<int, ratio<4,3>> a1(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(1600, (a1*a2).value());
	EXPECT_EQ(1, decltype(a1*a2)::unit::num);
	EXPECT_EQ(3, decltype(a1*a2)::unit::den);

	Quantity<int, ratio<2,3>> c1(10);
	Quantity<int, ratio<4,3>> c2(40);
	EXPECT_EQ(800, (c1*c2).value());
	EXPECT_EQ(2, decltype(c1*c2)::unit::num);
	EXPECT_EQ(3, decltype(c1*c2)::unit::den);

	Quantity<int, ratio<7,8>> d1(10);
	Quantity<int, ratio<8,9>> d2(40);
	EXPECT_EQ(1612800, (d1*d2).value());
	EXPECT_EQ(1, decltype(d1*d2)::unit::num);
	EXPECT_EQ(72, decltype(d1*d2)::unit::den);
}

TEST(UnitTest, MultiplyDim)
{
	Quantity<int, ratio<4,3>> a1(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(2, int(decltype(a1*a2)::dim::d1));
	EXPECT_EQ(0, int(decltype(a1*a2)::dim::d2));
	EXPECT_EQ(0, int(decltype(a1*a2)::dim::d3));

	Quantity<int, ratio<4,3>, Dimension<0,1,0>> b1(10);
	Quantity<int, ratio<1,3>, Dimension<0,1,0>> b2(40);
	EXPECT_EQ(0, int(decltype(b1*b2)::dim::d1));
	EXPECT_EQ(2, int(decltype(b1*b2)::dim::d2));
	EXPECT_EQ(0, int(decltype(b1*b2)::dim::d3));

	Quantity<int, ratio<2,3>, Dimension<1,0,0>> c1(10);
	Quantity<int, ratio<4,3>, Dimension<0,1,0>> c2(40);
	EXPECT_EQ(1, int(decltype(c1*c2)::dim::d1));
	EXPECT_EQ(1, int(decltype(c1*c2)::dim::d2));
	EXPECT_EQ(0, int(decltype(c1*c2)::dim::d3));

	Quantity<int, ratio<7,8>, Dimension<1,-1,1>> d1(10);
	Quantity<int, ratio<8,9>, Dimension<0, 1,2>> d2(40);
	EXPECT_EQ(1, int(decltype(d1*d2)::dim::d1));
	EXPECT_EQ(0, int(decltype(d1*d2)::dim::d2));
	EXPECT_EQ(3, int(decltype(d1*d2)::dim::d3));
}

TEST(UnitTest, MultiplyType)
{
	Quantity<int, ratio<7,8>> d1(10);
	Quantity<int, ratio<8,9>> d2(40);
	EXPECT_EQ(1612800, (d1*d2).value());
	auto result = (d1*d2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Quantity<int, ratio<7,8>> e1(10);
	Quantity<float, ratio<8,9>> e2(40);
	EXPECT_FLOAT_EQ(1612800.f, (e1*e2).value());
	auto val2 = (e1*e2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
}

TEST(UnitTest, DivCoeff)
{
	Quantity<int, ratio<4,3>> a1(10);
	Quantity<int, ratio<1,3>> a2(40);
	EXPECT_EQ(1, decltype(a1/a2)::unit::num);
	EXPECT_EQ(3, decltype(a1/a2)::unit::den);
	EXPECT_EQ(1, (a1/a2).value());

	Quantity<float, ratio<2,3>> c1(10);
	Quantity<float, ratio<4,3>> c2(40);
	EXPECT_FLOAT_EQ(0.125f, (c1/c2).value());
	EXPECT_EQ(2, decltype(c1/c2)::unit::num);
	EXPECT_EQ(3, decltype(c1/c2)::unit::den);

	Quantity<float, ratio<7,8>> d1(10);
	Quantity<float, ratio<8,9>> d2(40);
	EXPECT_EQ(0.24609375, (d1/d2).value());
	EXPECT_EQ(1, decltype(d1/d2)::unit::num);
	EXPECT_EQ(72, decltype(d1/d2)::unit::den);
}

TEST(UnitTest, DivDim)
{
	// Todo: handle zero dimensions returing fundamental type
	Quantity<int, ratio<4,3>, Dimension<1,0,0>> a1(10);
	Quantity<int, ratio<1,3>, Dimension<1,0,0>> a2(40);
	EXPECT_EQ(0, int(decltype(a1/a2)::dim::d1));
	EXPECT_EQ(0, int(decltype(a1/a2)::dim::d2));
	EXPECT_EQ(0, int(decltype(a1/a2)::dim::d3));

	Quantity<float, ratio<2,3>, Dimension<1,0,0>> b1(10);
	Quantity<float, ratio<4,3>, Dimension<0,1,0>> b2(40);
	EXPECT_EQ( 1, int(decltype(b1/b2)::dim::d1));
	EXPECT_EQ(-1, int(decltype(b1/b2)::dim::d2));
	EXPECT_EQ( 0, int(decltype(b1/b2)::dim::d3));

	Quantity<float, ratio<7,8>, Dimension<1,-1,1>> d1(10);
	Quantity<float, ratio<8,9>, Dimension<0, 1,2>> d2(40);
	EXPECT_EQ( 1, int(decltype(d1/d2)::dim::d1));
	EXPECT_EQ(-2, int(decltype(d1/d2)::dim::d2));
	EXPECT_EQ(-1, int(decltype(d1/d2)::dim::d3));
}

TEST(UnitTest, DivType)
{
	// Todo: As it stands:

	// + When the bases are different, result is a quantity with zero dimension
	// (and a residual common base that's meaningless)

	Quantity<int, ratio<7,8>> d1(10);
	Quantity<int, ratio<8,9>> d2(40);
	EXPECT_EQ(0, (d1/d2).value());
	auto val = (d1/d2).value();
	EXPECT_EQ(1, std::is_integral<decltype(val)>::value);

	Quantity<int, ratio<7,8>> e1(10);
	Quantity<float, ratio<8,9>> e2(40);
	EXPECT_FLOAT_EQ(0.24609375f, (e1/e2).value());
	EXPECT_FLOAT_EQ(4.0634923f, (e2/e1).value());
	auto val2 = (e1/e2).value();
	auto val3 = (e2/e1).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(val3)>::value);

	// + When bases are equal, result is a fundamental type (what we want)

	Quantity<int, ratio<4,3>> a1(7);
	Quantity<int, ratio<4,3>> a2(2);
	EXPECT_EQ(3, a1/a2);
	auto val4 = a1/a2;
	EXPECT_EQ(1, std::is_integral<decltype(val4)>::value);

	Quantity<int, ratio<4,3>> b1(7);
	Quantity<float, ratio<4,3>> b2(2);
	EXPECT_FLOAT_EQ(3.5f, b1/b2);
	EXPECT_FLOAT_EQ(0.2857142f, b2/b1);
	auto val5 = b1/b2;
	auto val6 = b2/b1;
	EXPECT_EQ(1, std::is_floating_point<decltype(val5)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(val6)>::value);
}

TEST(UnitTest, ScalarMultiplyCoeff)
{
	Quantity<int, ratio<4,3>> a(10);
	EXPECT_EQ(20, (a*2).value());
	EXPECT_EQ(4, decltype(a*2)::unit::num);
	EXPECT_EQ(3, decltype(a*2)::unit::den);
	EXPECT_EQ(20, (2*a).value());
	EXPECT_EQ(4, decltype(2*a)::unit::num);
	EXPECT_EQ(3, decltype(2*a)::unit::den);
}

TEST(UnitTest, ScalarMultiplyDim)
{
	Quantity<int, ratio<4,3>> a(10);
	EXPECT_EQ(1, int(decltype(a*2)::dim::d1));
	EXPECT_EQ(0, int(decltype(a*2)::dim::d2));
	EXPECT_EQ(0, int(decltype(a*2)::dim::d3));
	EXPECT_EQ(1, int(decltype(2*a)::dim::d1));
	EXPECT_EQ(0, int(decltype(2*a)::dim::d2));
	EXPECT_EQ(0, int(decltype(2*a)::dim::d3));

	Quantity<int, ratio<4,3>, Dimension<0,1,0>> b(10);
	EXPECT_EQ(0, int(decltype(b*2)::dim::d1));
	EXPECT_EQ(1, int(decltype(b*2)::dim::d2));
	EXPECT_EQ(0, int(decltype(b*2)::dim::d3));
	EXPECT_EQ(0, int(decltype(2*b)::dim::d1));
	EXPECT_EQ(1, int(decltype(2*b)::dim::d2));
	EXPECT_EQ(0, int(decltype(2*b)::dim::d3));
}

TEST(UnitTest, ScalarMultiplyType)
{
	Quantity<int, ratio<4,3>> a(10);
	EXPECT_EQ(20, (a*2).value());
	EXPECT_EQ(20, (2*a).value());
	auto left = (2*a).value();
	auto right = (a*2).value();
	EXPECT_EQ(1, std::is_integral<decltype(left)>::value);
	EXPECT_EQ(1, std::is_integral<decltype(right)>::value);

	Quantity<int, ratio<4,3>> b(10);
	EXPECT_FLOAT_EQ(20.f, (b*2.f).value());
	EXPECT_FLOAT_EQ(20.f, (2.f*b).value());
	auto left2 = (2.f*b).value();
	auto right2 = (b*2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left2)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right2)>::value);

	Quantity<float, ratio<4,3>> c(10.25f);
	EXPECT_FLOAT_EQ(20.5f, (c*2).value());
	EXPECT_FLOAT_EQ(20.5f, (2*c).value());
	auto left3 = (2*c).value();
	auto right3 = (c*2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left3)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right3)>::value);

	Quantity<float, ratio<4,3>> d(10.25f);
	EXPECT_FLOAT_EQ(20.5f, (d*2).value());
	EXPECT_FLOAT_EQ(20.5f, (2*d).value());
	auto left4 = (2.f*d).value();
	auto right4 = (d*2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left4)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right4)>::value);
}

TEST(UnitTest, ScalarDivCoeff)
{
	Quantity<int, ratio<4,3>> a1(7);
	EXPECT_EQ(3, (a1/2).value());  // integer division
	EXPECT_EQ(4, decltype(a1/2)::unit::num);
	EXPECT_EQ(3, decltype(a1/2)::unit::den);
}

TEST(UnitTest, ScalarDivDim)
{
	Quantity<int, ratio<4,3>> a(10);
	EXPECT_EQ(1, int(decltype(a/2)::dim::d1));
	EXPECT_EQ(0, int(decltype(a/2)::dim::d2));
	EXPECT_EQ(0, int(decltype(a/2)::dim::d3));

	Quantity<int, ratio<4,3>, Dimension<0,1,0>> b(10);
	EXPECT_EQ(0, int(decltype(b/2)::dim::d1));
	EXPECT_EQ(1, int(decltype(b/2)::dim::d2));
	EXPECT_EQ(0, int(decltype(b/2)::dim::d3));
}

TEST(UnitTest, ScalarDivType)
{
	Quantity<int, ratio<4,3>> a(7);
	EXPECT_EQ(3, (a/2).value());  // integer division
	auto result = (a/2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Quantity<int, ratio<4,3>> b(7);
	EXPECT_FLOAT_EQ(3.5f, (b/2.f).value());
	auto result2 = (b/2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result2)>::value);

	Quantity<float, ratio<4,3>> c(7);
	EXPECT_FLOAT_EQ(3.5f, (c/2.f).value());
	auto result3 = (c/2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result3)>::value);

	Quantity<float, ratio<4,3>> d(7);
	EXPECT_FLOAT_EQ(3.5f, (d/2.f).value());
	auto result4 = (d/2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result4)>::value);
}
