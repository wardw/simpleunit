#include <iostream>
#include <ratio>
#include "gtest/gtest.h"

#include "nful/Units.h"

using namespace std;
using namespace nful;

TEST(UnitTest, ImplicitConversion)
{
	// Implicit conversion - only available when source is an exact multiple of target ratio type
	Unit<int, BaseRatio<4,3>> a(10);
	Unit<int, BaseRatio<1,3>> a2(a);
	//Unit<int, BaseRatio<1,1>> ai3(a); // Should not compile: no matching constructor
	EXPECT_EQ(10, a.value());
	EXPECT_EQ(40, a2.value());

	// // Narrowing of value coefficient - requires explicit cast
	Unit<float, BaseRatio<1,1>> b(10);
	// Unit<int, BaseRatio<1,1>> b2(b);  // Should not compile: no matching constructor

	// If target is floating-point, conversions are always possilbe
	Unit<int, BaseRatio<4,3>> c(10);
	Unit<float, BaseRatio<1,1>> c2(c);
	EXPECT_EQ(10, c.value());
	EXPECT_FLOAT_EQ(40.f/3, c2.value());

	Unit<float, BaseRatio<4,3>> d(10);
	Unit<float, BaseRatio<1,1>> d2(d);
	EXPECT_EQ(10, c.value());
	EXPECT_FLOAT_EQ(40.f/3, c2.value());
}

TEST(UnitTest, CompoundAssignment)
{
	// Compound assignment with implicit conversion

	Unit<int, BaseRatio<4,3>> a(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	a2 += a;
	// a += a2; // Should not compile: no viable overloaded '+='
	EXPECT_EQ(80, a2.value());

	Unit<int, BaseRatio<4,3>> c(10);
	Unit<float, BaseRatio<1,1>> c2(40.f/3);
	c2 += c;
	// c += c2; // Should not compile: no viable overloaded '+='
	EXPECT_FLOAT_EQ(80.f/3, c2.value());

	Unit<float, BaseRatio<4,3>> d(10);
	Unit<float, BaseRatio<1,1>> d2(40.f/3);
	d2 += d;
	// d += d2; // Should not compile: no viable overloaded '+='
	EXPECT_FLOAT_EQ(80.f/3, d2.value());
}

TEST(UnitTest, CompoundAssignmentOthers)
{
	Unit<int, BaseRatio<4,3>> a(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(80, (a2 += a).value());

	Unit<int, BaseRatio<4,3>> b(10);
	Unit<int, BaseRatio<1,3>> b2(20);
	EXPECT_EQ(-20, (b2 -= b).value());

	Unit<int, BaseRatio<4,3>> c(7);
	EXPECT_EQ(14, (c *= 2).value());

	Unit<int, BaseRatio<4,3>> d(7);
	EXPECT_EQ(3, (d /= 2).value());
	EXPECT_EQ(1, (d /= 2.f).value());

	Unit<float, BaseRatio<4,3>> e(7);
	EXPECT_FLOAT_EQ(3.5f, (e /= 2).value());
	EXPECT_FLOAT_EQ(1.75f, (e /= 2.f).value());
}

TEST(UnitTest, UnitCastCoeff)
{
	// Dimensions of 1
	Unit<float, BaseUnit<Dim<1,1,1>, ratio<4,3>, ratio<4,3>, ratio<4,3>>> a(7);
	auto a1 = unit_cast<Unit<float, BaseUnit<Dim<1,1,1>, ratio<3,2>, ratio<1,2>, ratio<1,3>>>>(a);
	EXPECT_FLOAT_EQ(66.3704, a1.value());
	EXPECT_EQ(3, decltype(a1)::base::r1::num);
	EXPECT_EQ(2, decltype(a1)::base::r1::den);
	EXPECT_EQ(1, decltype(a1)::base::r2::num);
	EXPECT_EQ(2, decltype(a1)::base::r2::den);
	EXPECT_EQ(1, decltype(a1)::base::r3::num);
	EXPECT_EQ(3, decltype(a1)::base::r3::den);

	Unit<float, BaseUnit<Dim<1,1>, ratio<1>, ratio<1>>> b(7);
	auto b1 = unit_cast<Unit<float, BaseUnit<Dim<1,1>, ratio<1,39>, ratio<60,1>>>>(b);
	EXPECT_FLOAT_EQ(4.55, b1.value());
	EXPECT_EQ( 1, decltype(b1)::base::r1::num);
	EXPECT_EQ(39, decltype(b1)::base::r1::den);
	EXPECT_EQ(60, decltype(b1)::base::r2::num);
	EXPECT_EQ( 1, decltype(b1)::base::r2::den);

	Unit<float, BaseUnit<Dim<1,1,1>, ratio<4,3>, ratio<4,3>>> c(7);
	auto c1 = unit_cast<Unit<float, BaseUnit<Dim<1,1,1>, ratio<3,2>, ratio<1,2>, ratio<1,3>>>>(c);
	EXPECT_FLOAT_EQ(49.77777, c1.value());

	// Dimensions of > 1
	Unit<float, BaseUnit<Dim<2>, ratio<1,100>>> d(10);
	auto d1 = unit_cast<Unit<float, BaseUnit<Dim<2>, ratio<1,39>>>>(d);
	EXPECT_FLOAT_EQ(1.521, d1.value());

	Unit<float, BaseUnit<Dim<2,1,3>, ratio<4,3>, ratio<4,3>, ratio<4,3>>> e(7);
	auto e1 = unit_cast<Unit<float, BaseUnit<Dim<2,1,3>, ratio<3,2>, ratio<1,2>, ratio<1,3>>>>(e);
	EXPECT_FLOAT_EQ(943.93414, e1.value());
	EXPECT_EQ(3, decltype(e1)::base::r1::num);
	EXPECT_EQ(2, decltype(e1)::base::r1::den);
	EXPECT_EQ(1, decltype(e1)::base::r2::num);
	EXPECT_EQ(2, decltype(e1)::base::r2::den);
	EXPECT_EQ(1, decltype(e1)::base::r3::num);
	EXPECT_EQ(3, decltype(e1)::base::r3::den);

	// Dimensions of -1
	Unit<float, BaseUnit<Dim<1,-1>, ratio<4,3>, ratio<4,3>>> f(7);
	auto f1 = unit_cast<Unit<float, BaseUnit<Dim<1,-1>, ratio<3,2>, ratio<1,2>>>>(f);
	EXPECT_FLOAT_EQ(7.f/3, f1.value());
	EXPECT_EQ(3, decltype(f1)::base::r1::num);
	EXPECT_EQ(2, decltype(f1)::base::r1::den);
	EXPECT_EQ(1, decltype(f1)::base::r2::num);
	EXPECT_EQ(2, decltype(f1)::base::r2::den);

	// Dimensions of < -1
	Unit<float, BaseUnit<Dim<2,-3>, ratio<4,3>, ratio<4,3>>> g(7);
	auto g1 = unit_cast<Unit<float, BaseUnit<Dim<2,-3>, ratio<3,2>, ratio<1,2>>>>(g);
	EXPECT_FLOAT_EQ(0.29166666, g1.value());
	EXPECT_EQ(3, decltype(g1)::base::r1::num);
	EXPECT_EQ(2, decltype(g1)::base::r1::den);
	EXPECT_EQ(1, decltype(g1)::base::r2::num);
	EXPECT_EQ(2, decltype(g1)::base::r2::den);

	// Check that any non-unity ratios with zero-dimension do not interfer
	Unit<float, BaseUnit<Dim<1>, ratio<4,3>, ratio<4,3>, ratio<4,3>>> h(7);
	auto h1 = unit_cast<Unit<float, BaseUnit<Dim<1>, ratio<3,2>, ratio<7,8>, ratio<8,9>>>>(h);
	EXPECT_FLOAT_EQ(56.f/9, h1.value());
	EXPECT_EQ(3, decltype(h1)::base::r1::num);
	EXPECT_EQ(2, decltype(h1)::base::r1::den);
}

TEST(UnitTest, UnitCastDim)
{
	Unit<float, BaseUnit<Dim<1,1,1>, ratio<4,3>, ratio<4,3>, ratio<4,3>>> a(7);
	auto a1 = unit_cast<Unit<float, BaseUnit<Dim<1,1,1>, ratio<3,2>, ratio<1,2>, ratio<1,3>>>>(a);
	EXPECT_EQ(1, int(decltype(a1)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(a1)::base::dim::d2));
	EXPECT_EQ(1, int(decltype(a1)::base::dim::d3));

	Unit<float, BaseUnit<Dim<2,-3>, ratio<1>, ratio<1>>> b(7);
	auto b1 = unit_cast<Unit<float, BaseUnit<Dim<2,-3>, ratio<1,39>, ratio<60,1>>>>(b);
	EXPECT_EQ( 2, int(decltype(b1)::base::dim::d1));
	EXPECT_EQ(-3, int(decltype(b1)::base::dim::d2));
	EXPECT_EQ( 0, int(decltype(b1)::base::dim::d3));

	// A unit cast only casts between units of equal dimensions.
	Unit<float, BaseUnit<Dim<1,0,1>, ratio<4,3>, ratio<4,3>, ratio<4,3>>> c(7);
	// Should not compile: invalid operands to binary expression ('nful::Dim<1, 0, 1>' and 'nful::Dim<1, 1, 1>')
	//auto c1 = unit_cast<Unit<float, BaseUnit<Dim<1,1,1>, ratio<3,2>, ratio<1,2>, ratio<1,3>>>>(c);
}

TEST(UnitTest, UnitCastType)
{
	Unit<int, BaseRatio<4,3>> a(7);
	auto val = unit_cast<Unit<int, BaseRatio<3,2>>>(a).value();
	EXPECT_EQ(6, val);
	EXPECT_EQ(1, std::is_integral<decltype(val)>::value);

	Unit<float, BaseRatio<4,3>> b(7);
	auto val2 = unit_cast<Unit<int, BaseRatio<3,2>>>(b).value();
	EXPECT_EQ(6, val2);
	EXPECT_EQ(1, std::is_integral<decltype(val2)>::value);

	Unit<int, BaseRatio<4,3>> c(7);
	auto val3 = unit_cast<Unit<float, BaseRatio<3,2>>>(c).value();
	EXPECT_FLOAT_EQ(56.f/9, val3);
	EXPECT_EQ(1, std::is_floating_point<decltype(val3)>::value);

	Unit<float, BaseRatio<4,3>> d(7);
	auto val4 = unit_cast<Unit<float, BaseRatio<3,2>>>(d).value();
	EXPECT_FLOAT_EQ(56.f/9, val4);
	EXPECT_EQ(1, std::is_floating_point<decltype(val4)>::value);
}

TEST(UnitTest, MemberCast)
{
	using namespace si;

	Millimeters a(7);
	EXPECT_FLOAT_EQ(7, a.asVal());
	EXPECT_FLOAT_EQ(7, a.as().value());
	EXPECT_FLOAT_EQ(0.007, a.asVal<Meters>());
	EXPECT_FLOAT_EQ(0.007, a.as<Meters>().value());
}

/* For binary operators, currently care about three things. Does it return

+ the right answer (coefficient and base)
+ in the right dimension
+ of the right type

These three axes tested independently

*/

TEST(UnitTest, AddCoeff)
{
	Unit<int, Length<ratio<4,3>>> a1(10);
	Unit<int, Length<ratio<1,3>>> a2(40);
	EXPECT_EQ(80, (a1+a2).value());
	EXPECT_EQ(1, decltype(a1+a2)::base::r1::num);
	EXPECT_EQ(3, decltype(a1+a2)::base::r1::den);

	Unit<int, Length<ratio<2,3>>> b1(10);
	Unit<int, Length<ratio<4,3>>> b2(40);
	EXPECT_EQ(90, (b1+b2).value());
	EXPECT_EQ(2, decltype(b1+b2)::base::r1::num);
	EXPECT_EQ(3, decltype(b1+b2)::base::r1::den);

	Unit<int, Length<ratio<7,8>>> c1(10);
	Unit<int, Length<ratio<8,9>>> c2(40);
	EXPECT_EQ(3190, (c1+c2).value());
	EXPECT_EQ(1, decltype(c1+c2)::base::r1::num);
	EXPECT_EQ(72, decltype(c1+c2)::base::r1::den);
}

TEST(UnitTest, AddDim)
{
	Unit<int, BaseRatio<4,3>> a1(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(1, int(decltype(a1+a2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(a1+a2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(a1+a2)::base::dim::d3));

	Unit<int, BaseUnit<Dim<0,1,0>,ratio<4,3>>> b1(10);
	Unit<int, BaseUnit<Dim<0,1,0>,ratio<1,3>>> b2(40);
	EXPECT_EQ(0, int(decltype(b1+b2)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(b1+b2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(b1+b2)::base::dim::d3));

	// Dim mismatch's should not compile
	Unit<int, BaseUnit<Dim<1,1,0>>> c1(10);
	Unit<int, BaseUnit<Dim<0,1,0>>> c2(40);
	//c1 + c2;
}

TEST(UnitTest, AddType)
{
	Unit<int, BaseRatio<7,8>> c1(10);
	Unit<int, BaseRatio<8,9>> c2(40);
	EXPECT_EQ(3190, (c1+c2).value());
	auto result = (c1+c2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Unit<int, BaseRatio<7,8>> d1(10);
	Unit<float, BaseRatio<8,9>> d2(40);
	EXPECT_FLOAT_EQ(3190.f, (d1+d2).value());
	auto val2 = (d1+d2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
}

TEST(UnitTest, MultiplyCoeff)
{
	Unit<int, BaseRatio<4,3>> a1(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(1600, (a1*a2).value());
	EXPECT_EQ(1, decltype(a1*a2)::base::r1::num);
	EXPECT_EQ(3, decltype(a1*a2)::base::r1::den);

	Unit<int, BaseRatio<2,3>> c1(10);
	Unit<int, BaseRatio<4,3>> c2(40);
	EXPECT_EQ(800, (c1*c2).value());
	EXPECT_EQ(2, decltype(c1*c2)::base::r1::num);
	EXPECT_EQ(3, decltype(c1*c2)::base::r1::den);

	Unit<int, BaseRatio<7,8>> d1(10);
	Unit<int, BaseRatio<8,9>> d2(40);
	EXPECT_EQ(1612800, (d1*d2).value());
	EXPECT_EQ(1, decltype(d1*d2)::base::r1::num);
	EXPECT_EQ(72, decltype(d1*d2)::base::r1::den);
}

TEST(UnitTest, MultiplyDim)
{
	Unit<int, BaseRatio<4,3>> a1(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(2, int(decltype(a1*a2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(a1*a2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(a1*a2)::base::dim::d3));

	Unit<int, BaseUnit<Dim<0,1,0>, ratio<4,3>>> b1(10);
	Unit<int, BaseUnit<Dim<0,1,0>, ratio<1,3>>> b2(40);
	EXPECT_EQ(0, int(decltype(b1*b2)::base::dim::d1));
	EXPECT_EQ(2, int(decltype(b1*b2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(b1*b2)::base::dim::d3));

	Unit<int, BaseUnit<Dim<1,0,0>, ratio<2,3>>> c1(10);
	Unit<int, BaseUnit<Dim<0,1,0>, ratio<4,3>>> c2(40);
	EXPECT_EQ(1, int(decltype(c1*c2)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(c1*c2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(c1*c2)::base::dim::d3));

	Unit<int, BaseUnit<Dim<1,-1,1>, ratio<7,8>>> d1(10);
	Unit<int, BaseUnit<Dim<0, 1,2>, ratio<8,9>>> d2(40);
	EXPECT_EQ(1, int(decltype(d1*d2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(d1*d2)::base::dim::d2));
	EXPECT_EQ(3, int(decltype(d1*d2)::base::dim::d3));
}

TEST(UnitTest, MultiplyType)
{
	Unit<int, BaseRatio<7,8>> d1(10);
	Unit<int, BaseRatio<8,9>> d2(40);
	EXPECT_EQ(1612800, (d1*d2).value());
	auto result = (d1*d2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Unit<int, BaseRatio<7,8>> e1(10);
	Unit<float, BaseRatio<8,9>> e2(40);
	EXPECT_FLOAT_EQ(1612800.f, (e1*e2).value());
	auto val2 = (e1*e2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
}

TEST(UnitTest, DivCoeff)
{
	Unit<int, BaseRatio<4,3>> a1(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(1, decltype(a1/a2)::base::r1::num);
	EXPECT_EQ(3, decltype(a1/a2)::base::r1::den);
	EXPECT_EQ(1, (a1/a2).value());

	Unit<float, BaseRatio<2,3>> c1(10);
	Unit<float, BaseRatio<4,3>> c2(40);
	EXPECT_FLOAT_EQ(0.125f, (c1/c2).value());
	EXPECT_EQ(2, decltype(c1/c2)::base::r1::num);
	EXPECT_EQ(3, decltype(c1/c2)::base::r1::den);

	Unit<float, BaseRatio<7,8>> d1(10);
	Unit<float, BaseRatio<8,9>> d2(40);
	EXPECT_EQ(0.24609375, (d1/d2).value());
	EXPECT_EQ(1, decltype(d1/d2)::base::r1::num);
	EXPECT_EQ(72, decltype(d1/d2)::base::r1::den);
}

TEST(UnitTest, DivDim)
{
	// Todo: handle zero dimensions returing fundamental type
	Unit<int, BaseRatio<4,3>> a1(10);
	Unit<int, BaseRatio<1,3>> a2(40);
	EXPECT_EQ(0, int(decltype(a1/a2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(a1/a2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(a1/a2)::base::dim::d3));

	Unit<float, BaseUnit<Dim<1,0,0>, ratio<2,3>>> b1(10);
	Unit<float, BaseUnit<Dim<0,1,0>, ratio<4,3>>> b2(40);
	EXPECT_EQ( 1, int(decltype(b1/b2)::base::dim::d1));
	EXPECT_EQ(-1, int(decltype(b1/b2)::base::dim::d2));
	EXPECT_EQ( 0, int(decltype(b1/b2)::base::dim::d3));

	Unit<float, BaseUnit<Dim<1,-1,1>, ratio<7,8>>> d1(10);
	Unit<float, BaseUnit<Dim<0, 1,2>, ratio<8,9>>> d2(40);
	EXPECT_EQ( 1, int(decltype(d1/d2)::base::dim::d1));
	EXPECT_EQ(-2, int(decltype(d1/d2)::base::dim::d2));
	EXPECT_EQ(-1, int(decltype(d1/d2)::base::dim::d3));
}

TEST(UnitTest, DivType)
{
	// Todo: As it stands:

	// + When the bases are different, result is a unit with zero dimension
	// (and a residual common base that's meaningless)

	Unit<int, BaseRatio<7,8>> d1(10);
	Unit<int, BaseRatio<8,9>> d2(40);
	EXPECT_EQ(0, (d1/d2).value());
	auto val = (d1/d2).value();
	EXPECT_EQ(1, std::is_integral<decltype(val)>::value);

	Unit<int, BaseRatio<7,8>> e1(10);
	Unit<float, BaseRatio<8,9>> e2(40);
	EXPECT_FLOAT_EQ(0.24609375f, (e1/e2).value());
	EXPECT_FLOAT_EQ(4.0634923f, (e2/e1).value());
	auto val2 = (e1/e2).value();
	auto val3 = (e2/e1).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(val2)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(val3)>::value);

	// + When bases are equal, result is a fundamental type (what we want)
	// (Todo: either way theses should really be consistent,
	// - add support above, or remove this operator/ overload below)

	Unit<int, BaseRatio<4,3>> a1(7);
	Unit<int, BaseRatio<4,3>> a2(2);
	EXPECT_EQ(3, a1/a2);
	auto val4 = a1/a2;
	EXPECT_EQ(1, std::is_integral<decltype(val4)>::value);

	Unit<int, BaseRatio<4,3>> b1(7);
	Unit<float, BaseRatio<4,3>> b2(2);
	EXPECT_FLOAT_EQ(3.5f, b1/b2);
	EXPECT_FLOAT_EQ(0.2857142f, b2/b1);
	auto val5 = b1/b2;
	auto val6 = b2/b1;
	EXPECT_EQ(1, std::is_floating_point<decltype(val5)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(val6)>::value);
}

TEST(UnitTest, ScalarMultiplyCoeff)
{
	Unit<int, BaseRatio<4,3>> a(10);
	EXPECT_EQ(20, (a*2).value());
	EXPECT_EQ(4, decltype(a*2)::base::r1::num);
	EXPECT_EQ(3, decltype(a*2)::base::r1::den);
	EXPECT_EQ(20, (2*a).value());
	EXPECT_EQ(4, decltype(2*a)::base::r1::num);
	EXPECT_EQ(3, decltype(2*a)::base::r1::den);
}

TEST(UnitTest, ScalarMultiplyDim)
{
	Unit<int, BaseRatio<4,3>> a(10);
	EXPECT_EQ(1, int(decltype(a*2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(a*2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(a*2)::base::dim::d3));
	EXPECT_EQ(1, int(decltype(2*a)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(2*a)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(2*a)::base::dim::d3));

	Unit<int, BaseUnit<Dim<0,1,0>, ratio<4,3>>> b(10);
	EXPECT_EQ(0, int(decltype(b*2)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(b*2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(b*2)::base::dim::d3));
	EXPECT_EQ(0, int(decltype(2*b)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(2*b)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(2*b)::base::dim::d3));
}

TEST(UnitTest, ScalarMultiplyType)
{
	Unit<int, BaseRatio<4,3>> a(10);
	EXPECT_EQ(20, (a*2).value());
	EXPECT_EQ(20, (2*a).value());
	auto left = (2*a).value();
	auto right = (a*2).value();
	EXPECT_EQ(1, std::is_integral<decltype(left)>::value);
	EXPECT_EQ(1, std::is_integral<decltype(right)>::value);

	Unit<int, BaseRatio<4,3>> b(10);
	EXPECT_FLOAT_EQ(20.f, (b*2.f).value());
	EXPECT_FLOAT_EQ(20.f, (2.f*b).value());
	auto left2 = (2.f*b).value();
	auto right2 = (b*2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left2)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right2)>::value);

	Unit<float, BaseRatio<4,3>> c(10.25f);
	EXPECT_FLOAT_EQ(20.5f, (c*2).value());
	EXPECT_FLOAT_EQ(20.5f, (2*c).value());
	auto left3 = (2*c).value();
	auto right3 = (c*2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left3)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right3)>::value);

	Unit<float, BaseRatio<4,3>> d(10.25f);
	EXPECT_FLOAT_EQ(20.5f, (d*2).value());
	EXPECT_FLOAT_EQ(20.5f, (2*d).value());
	auto left4 = (2.f*d).value();
	auto right4 = (d*2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(left4)>::value);
	EXPECT_EQ(1, std::is_floating_point<decltype(right4)>::value);
}

TEST(UnitTest, ScalarDivCoeff)
{
	Unit<int, BaseRatio<4,3>> a1(7);
	EXPECT_EQ(3, (a1/2).value());  // integer division
	EXPECT_EQ(4, decltype(a1/2)::base::r1::num);
	EXPECT_EQ(3, decltype(a1/2)::base::r1::den);
}

TEST(UnitTest, ScalarDivDim)
{
	Unit<int, BaseRatio<4,3>> a(10);
	EXPECT_EQ(1, int(decltype(a/2)::base::dim::d1));
	EXPECT_EQ(0, int(decltype(a/2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(a/2)::base::dim::d3));

	Unit<int, BaseUnit<Dim<0,1,0>, ratio<4,3>>> b(10);
	EXPECT_EQ(0, int(decltype(b/2)::base::dim::d1));
	EXPECT_EQ(1, int(decltype(b/2)::base::dim::d2));
	EXPECT_EQ(0, int(decltype(b/2)::base::dim::d3));
}

TEST(UnitTest, ScalarDivType)
{
	Unit<int, BaseRatio<4,3>> a(7);
	EXPECT_EQ(3, (a/2).value());  // integer division
	auto result = (a/2).value();
	EXPECT_EQ(1, std::is_integral<decltype(result)>::value);

	Unit<int, BaseRatio<4,3>> b(7);
	EXPECT_FLOAT_EQ(3.5f, (b/2.f).value());
	auto result2 = (b/2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result2)>::value);

	Unit<float, BaseRatio<4,3>> c(7);
	EXPECT_FLOAT_EQ(3.5f, (c/2.f).value());
	auto result3 = (c/2).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result3)>::value);

	Unit<float, BaseRatio<4,3>> d(7);
	EXPECT_FLOAT_EQ(3.5f, (d/2.f).value());
	auto result4 = (d/2.f).value();
	EXPECT_EQ(1, std::is_floating_point<decltype(result4)>::value);
}

// TEST(UnitTest, OutputUnits)
// {
// 	Millimeter a(7);
// 	Meter b(a);
// 	EXPECT_FLOAT_EQ(0.007, a.asVal<Meter>());
// 	cout << "Original: " << a.as() << endl;
// 	cout << "As Meter: " << a.as<Meter>() << endl;
// 	cout << "a + b = " << a + b << endl;
// 	cout << "a + b + c = " << a + b + Centimeter(127) << endl;
// 	cout << "a + b + c = " << (a + b + Centimeter(127)).as<Centimeter>() << endl;
// }


TEST(UnitTest, Examples)
{
	using namespace si;

	m_s vel(4);
	cout << "Velociy: " << vel << endl;
	cout << "Velociy: " << vel.as<in_hr>() << endl;

}
