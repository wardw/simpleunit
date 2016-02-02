#include <iostream>
#include <chrono>
#include <ratio>

namespace sunit {

namespace
{
	template <typename X, typename Y>
	using AddType = decltype(std::declval<X>() + std::declval<Y>());

	template <typename X, typename Y>
	using MulType = decltype(std::declval<X>() * std::declval<Y>());

	template <typename D1, typename D2>
	using DivType = decltype(std::declval<D1>() / std::declval<D2>());
}

template <int D1, int D2=0, int D3=0>
struct Dim {
	static constexpr int d1 = D1;
	static constexpr int d2 = D2;
	static constexpr int d3 = D3;
};

template <int A1, int A2, int A3, int B1, int B2, int B3>
Dim<A1+B1, A2+B2, A3+B3> operator*(Dim<A1, A2, A3> lhs,
	                               Dim<B1, B2, B3> rhs) {
	return Dim<A1+B1, A2+B2, A3+B3>();
}

template <int A1, int A2, int A3, int B1, int B2, int B3>
using DimMultiply = Dim<A1+B1, A2+B2, A3+B3>;

template <int A1, int l1, int A3, int B1, int B2, int B3>
Dim<A1-B1, l1-B2, A3-B3> operator/(Dim<A1, l1, A3> lhs,
	                               Dim<B1, B2, B3> rhs) {
	return Dim<A1-B1, l1-B2, A3-B3>();
}

template <int A1, int A2, int A3>
Dim<A1, A2, A3> operator+(Dim<A1, A2, A3> lhs,
	                      Dim<A1, A2, A3> rhs) {
	return Dim<A1, A2, A3>();
}

template <typename Dim = Dim<1>,
          typename R1 = std::ratio<1>, typename R2 = std::ratio<1>, typename R3 = std::ratio<1>>
struct BaseUnit
{
	using dim = Dim;
	using r1 = R1;
	using r2 = R2;
	using r3 = R3;
};


template <typename B1, typename B2>
using IsMultiple = std::integral_constant<bool, (B1::r1::num * B2::r1::den) % (B2::r1::num * B1::r1::den) == 0 &&
                                                (B1::r2::num * B2::r2::den) % (B2::r2::num * B1::r2::den) == 0 &&
                                                (B1::r3::num * B2::r3::den) % (B2::r3::num * B1::r3::den) == 0 >;

template <typename T, typename B>
class Unit;

constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1) {
  return exp < 1 ? result : ipow(base*base, exp/2, (exp % 2) ? result*base : result);
}

constexpr int64_t iabs(int64_t i) {
	return i >= 0 ? i : -i;
}

// Zero-dimensions must contribute unity.
// This is indirectly covered by iexp returning 1 for exp < 0, but making this an explicit check
constexpr int64_t dividend(int64_t a, int exp) {
	if (exp == 0) return 1;
	else          return ipow(a, iabs(exp));
}

// Take positive powers of the quotient's inverse, rather than negative powers
constexpr int64_t power_flip(int64_t a, int64_t b, int exp) {
	if (exp == 0) return 1;
	else          return ipow(exp > 0 ? a : b, iabs(exp));
}

// Multiply inverse of the divisor (rather than divide)
template <typename R1, typename R, int exp>
using ConversionRatio = std::ratio_multiply<std::ratio<power_flip(R1::num, R1::den, exp), power_flip(R1::den, R1::num, exp)>,
                                            std::ratio<power_flip(R::den, R::num, exp), power_flip(R::num, R::den, exp)>>;

template <typename ToUnit, typename X, typename B1, typename D = typename B1::dim>
ToUnit dimension_cast(const Unit<X,B1>& unit)
{
	using Y = typename ToUnit::rep;
	using B = typename ToUnit::base;

	using conversion = std::ratio_multiply<ConversionRatio<typename B1::r1, typename B::r1, D::d1>,
	                   std::ratio_multiply<ConversionRatio<typename B1::r2, typename B::r2, D::d2>,
	                                       ConversionRatio<typename B1::r3, typename B::r3, D::d3>>>;

	return ToUnit(static_cast<Y>(unit.value()) * conversion::num / conversion::den);
}

template <typename ToUnit, typename X, typename B1>
ToUnit unit_cast(const Unit<X,B1>& unit)
{
	// todo: static_assert()
	// A unit cast only casts between units of equal dimensions.
	// To avoid this check, use dimension_cast.

	using B = typename ToUnit::base;
	return dimension_cast<ToUnit,X,B1,AddType<typename B1::dim,typename B::dim>>(unit);
}

template <typename T, typename B = BaseUnit<>>
class Unit
{
public:
	using rep = T;
	using base = B;

	Unit(const T& val) : value_(val) {}

	// The purpose of the following two construtors are to exclude the case for integral T but floating-point X
	// and ensure no loss of information in the integral to integral constructor
	// This convention is adpoted from std::chrono::duration
	template < typename X, typename B1,
		typename std::enable_if_t<
		    std::is_integral<T>::value &&
		    std::is_integral<X>::value &&
			IsMultiple<B1,B>::value, int > = 0 >
	Unit(const Unit<X,B1>& rhs) {
		value_ = unit_cast<Unit<T,B>>(rhs).value();
	}

	// The seemingly redundant test on `is_floating_point<X>` is required to make this
	// overload conditionally dependent on X (although there's probably a better way)
	template < typename X, typename B1,
		typename std::enable_if_t<
		    (std::is_floating_point<T>::value && std::is_floating_point<X>::value) ||
		    (std::is_floating_point<T>::value && !std::is_floating_point<X>::value), int> = 0 >
	Unit(const Unit<X,B1>& rhs) {
		value_ = unit_cast<Unit<T,B>>(rhs).value();
	}

	T& value() { return value_; }
	const T& value() const { return value_; }

	template <typename Q = Unit<T,B>>
	Q as() const { return unit_cast<Q>(*this); }

	template <typename Q = Unit<T,B>>
	T asVal() const { return unit_cast<Q>(*this).value(); }

	Unit& operator+=(const Unit& rhs) { value_ += rhs.value(); return *this; }
	Unit& operator-=(const Unit& rhs) { value_ -= rhs.value(); return *this; }
	template <typename X>
	Unit& operator*=(const X& x) { value_ *= x; return *this; }
	template <typename X>
	Unit& operator/=(const X& x) { value_ /= x; return *this; }

	friend std::ostream& operator<<(std::ostream& os, const Unit& q)
	{
		return os << q.value()
		       << " (" << base::r1::num << "/" << base::r1::den
		       << ", " << base::r2::num << "/" << base::r2::den
		       << ", " << base::r3::num << "/" << base::r3::den
		       << ")"
		       << " [" << base::dim::d1 << "," << base::dim::d2 << "," << base::dim::d3 << "]";
	}

private:
	T value_;
};

template <typename R1, typename R2>
using CommonRatio = typename std::common_type<std::chrono::duration<int,R1>, std::chrono::duration<int,R2>>::type::period;

template <typename D, typename B1, typename B2>
using CommonBase = BaseUnit<D, CommonRatio<typename B1::r1 , typename B2::r1>,
                               CommonRatio<typename B1::r2 , typename B2::r2>,
                               CommonRatio<typename B1::r3 , typename B2::r3>>;

// Unit + - * / Unit

template <typename X, typename Y, typename B1, typename B2,
          typename ToUnit = Unit< AddType<X,Y>, CommonBase<AddType<typename B1::dim,typename B2::dim>,B1,B2>> >
ToUnit operator+(const Unit<X,B1>& lhs, const Unit<Y,B2>& rhs)
{
	using B = typename ToUnit::base;
	return ToUnit(unit_cast<Unit<X,B>>(lhs).value() + unit_cast<Unit<Y,B>>(rhs).value());
}

template <typename X, typename Y, typename B1, typename B2,
          typename ToUnit = Unit< AddType<X,Y>, CommonBase<AddType<typename B1::dim,typename B2::dim>,B1,B2>> >
ToUnit operator-(const Unit<X,B1>& lhs, const Unit<Y,B2>& rhs)
{
	using B = typename ToUnit::base;
	return ToUnit(unit_cast<Unit<X,B>>(lhs).value() - unit_cast<Unit<Y,B>>(rhs).value());
}

template <typename X, typename Y, typename B1, typename B2,
          typename ToUnit = Unit< AddType<X,Y>, CommonBase<MulType<typename B1::dim,typename B2::dim>,B1,B2>> >
ToUnit operator*(const Unit<X,B1>& lhs, const Unit<Y,B2>& rhs)
{
	using B = typename ToUnit::base;
	return ToUnit(dimension_cast<Unit<X,B>>(lhs).value() * dimension_cast<Unit<Y,B>>(rhs).value());
}

template <typename X, typename Y, typename B1, typename B2,
          typename ToUnit = Unit< AddType<X,Y>, CommonBase<DivType<typename B1::dim,typename B2::dim>,B1,B2>> >
ToUnit operator/(const Unit<X,B1>& lhs, const Unit<Y,B2>& rhs)
{
	using B = typename ToUnit::base;
	return ToUnit(dimension_cast<Unit<X,B>>(lhs).value() / dimension_cast<Unit<Y,B>>(rhs).value());
}

// Todo: see `TEST(UnitTest, DivType)`
template <typename X, typename Y, typename B>
MulType<X,Y> operator/(const Unit<X,B>& lhs, const Unit<Y,B>& rhs)
{
	return MulType<X,Y>(lhs.value() / rhs.value());
}


// Scalar * * / Unit

template <typename X, typename Y, typename B,
          typename = std::enable_if_t<std::is_arithmetic<Y>::value>>
Unit<MulType<X,Y>,B> operator*(const Unit<X,B>& lhs, const Y& y)
{
	return Unit<MulType<X,Y>,B>(lhs.value() * y);
}

template <typename X, typename Y, typename B,
          typename = std::enable_if_t<std::is_arithmetic<Y>::value>>
Unit<MulType<X,Y>,B> operator*(const Y& y, const Unit<X,B>& rhs)
{
	return Unit<MulType<X,Y>,B>(rhs.value() * y);
}

template <typename X, typename Y, typename B,
          typename = std::enable_if_t<std::is_arithmetic<Y>::value>>
Unit<MulType<X,Y>,B> operator/(const Unit<X,B>& lhs, const Y& y)
{
	return Unit<MulType<X,Y>,B>(lhs.value() / y);
}


// Helper types (todo: fill out, put elsewhere)

template <int n, int d> using BaseRatio = BaseUnit<Dim<1>, std::ratio<n,d>>;

// Fundamental dimensions

template <typename r> using Length  = BaseUnit<Dim<1>, r>;
template <typename r> using Length2 = BaseUnit<Dim<2>, r>;
template <typename r> using Length3 = BaseUnit<Dim<2>, r>;
template <typename r> using Time    = BaseUnit<Dim<0,1>, r>;
template <typename r> using Time2   = BaseUnit<Dim<0,2>, r>;
template <typename r> using Mass    = BaseUnit<Dim<0,0,1>, r>;

// Derived dimensions

template <typename r1, typename r2> using Velocity       = BaseUnit<Dim<1,-1>, r1, r2>;
template <typename r1, typename r2> using Acceleration   = BaseUnit<Dim<1,-2>, r1, r2>;
template <typename r1, typename r2> using VolumetricFlux = BaseUnit<Dim<2,-1>, r1, r2>;

template <typename r1, typename r2, typename r3> using Force = BaseUnit<Dim<1,1,-2>, r1, r2, r3>;


namespace si {

	// Useful ratios

	using meter = std::ratio<1>;
	using second = std::ratio<1>;
	using kg = std::ratio<1>;

	using inch = std::ratio<1,39>;
	using minute = std::ratio<60>;
	using hour = std::ratio<3600>;

	// Units (long)

	using Meters = Unit<float, Length<meter>>;
	using Meters2 = Unit<float, Length2<meter>>;
	using Meters3 = Unit<float, Length3<meter>>;
	using Centimeters = Unit<float, Length<std::centi>>;
	using Centimeters2 = Unit<float, Length2<std::centi>>;
	using Centimeters3 = Unit<float, Length3<std::centi>>;
	using Millimeters = Unit<float, Length<std::milli>>;
	using Millimeters2 = Unit<float, Length2<std::milli>>;
	using Millimeters3 = Unit<float, Length3<std::milli>>;

	using Inches = Unit<float, Length<inch>>;

	using Seconds = Unit<float, Time<second>>;
	using Minutes = Unit<float, Time<minute>>;
	using Hours = Unit<float, Time<hour>>;

	using Kilograms = Unit<float, Mass<kg>>;

	// Units (short?)

	using m = Unit<float, Length<meter>>;
	using in = Unit<float, Length<inch>>;

	// Derived units (long)

	using Meters_Second = Unit<float, Velocity<meter, second>>;
	using Meters_Second2 = Unit<float, Acceleration<meter, second>>;
	using Inches_Hour = Unit<float, Velocity<inch, hour>>;
	using KilogramMeters_Second2 = Unit<float, Force<meter, second, kg>>;
	using Meters2_Second = Unit<float, VolumetricFlux<meter, second>>;
	using Inches2_Second = Unit<float, VolumetricFlux<inch, second>>;

	// Derived units (short)

	using m_s = Unit<float, Velocity<meter, second>>;
	using m_s2 = Unit<float, Acceleration<meter, second>>;
	using in_hr = Unit<float, Velocity<inch, hour>>;
	using kgm_s2 = Unit<float, Force<meter, second, kg>>;

	// Constants

	// Todo: Unit a literal type
	//constexpr m_s2 g(9.81);

} // si


// Examples, to fill out

std::ostream& operator<<(std::ostream& os, const si::Meters& q)
{ return os << q.value() << " m"; }
std::ostream& operator<<(std::ostream& os, const si::Centimeters& q)
{ return os << q.value() << " cm"; }
std::ostream& operator<<(std::ostream& os, const si::Millimeters& q)
{ return os << q.value() << " mm"; }

std::ostream& operator<<(std::ostream& os, const si::Meters2_Second& q)
{ return os << q.value() << " m^2/s"; }
std::ostream& operator<<(std::ostream& os, const si::Inches2_Second& q)
{ return os << q.value() << " in^2/s"; }

std::ostream& operator<<(std::ostream& os, const si::Meters2& q)
{ return os << q.value() << " m^2"; }
std::ostream& operator<<(std::ostream& os, const si::Centimeters2& q)
{ return os << q.value() << " cm^2"; }

std::ostream& operator<<(std::ostream& os, const si::m_s& q)
{ return os << q.value() << " m/s"; }

std::ostream& operator<<(std::ostream& os, const si::in_hr& q)
{ return os << q.value() << " in/hr"; }

} // sunit
