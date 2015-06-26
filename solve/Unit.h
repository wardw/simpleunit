#include <iostream>
#include <chrono>

// Experimental no frills units library inspired on Barton & Nackman's Dimensional Analysis (Ch 16.5)
// for dimension handling and std::chrono library / std::ratio for units of measurement.

// A very minimal subset of features originally as required for problems in classical mechanics

namespace
{
	// Todo: I could use std::common_type
	// + which is already defined for built-ins (X,Y)
	// + and overloading for Dimension would be to replace the existing Dimension operators (?)

	template <typename X, typename Y>
	using AddType = decltype(std::declval<X>() + std::declval<Y>());

	template <typename X, typename Y>
	using MulType = decltype(std::declval<X>() * std::declval<Y>());

	template <typename D1, typename D2>
	using DivType = decltype(std::declval<D1>() / std::declval<D2>());
}


namespace hummingbird {

template <int m, int l, int t>
struct Dimension {
	static constexpr int d1 = m;
	static constexpr int d2 = l;
	static constexpr int d3 = t;
};

template <int m1, int l1, int t1, int m2, int l2, int t2>
Dimension<m1+m2, l1+l2, t1+t2> operator*(Dimension<m1, l1, t1> lhs,
	                                     Dimension<m2, l2, t2> rhs) {
	return Dimension<m1+m2, l1+l2, t1+t2>();
}

template <int m1, int l1, int t1, int m2, int l2, int t2>
Dimension<m1-m2, l1-l2, t1-t2> operator/(Dimension<m1, l1, t1> lhs,
	                                     Dimension<m2, l2, t2> rhs) {
	return Dimension<m1-m2, l1-l2, t1-t2>();
}

template <typename T, typename D>
class Unit
{
public:
	explicit Unit(const T& val) : value_(val) {}

	T& value() { return value_; }
	const T& value() const { return value_; }

	Unit& operator+=(const Unit& rhs) { value_ += rhs; return *this; }
	Unit& operator-=(const Unit& rhs) { value_ -= rhs; return *this; }
	template <typename S>
	Unit& operator*=(const S& rhs) { value_ *= rhs; return *this; }
	template <typename S>
	Unit& operator/=(const S& rhs) { value_ /= rhs; return *this; }

private:
	T value_;

	friend std::ostream& operator<<(std::ostream& os, const Unit& quantity) {
		return os << quantity.value_ << " units";
	}
};



template <typename X, typename Y, typename D>
Unit<AddType<X,Y>, D> operator+(const Unit<X,D>& lhs,
	                            const Unit<Y,D>& rhs) {
	return Unit<AddType<X,Y>, D>(lhs.value() + rhs.value());
}

template <typename X, typename Y, typename D>
Unit<AddType<X,Y>, D> operator-(const Unit<X,D>& lhs,
	                            const Unit<Y,D>& rhs) {
	return Unit<AddType<X,Y>, D>(lhs.value() - rhs.value());
}

template <typename T, typename D, typename S>
Unit<T,D> operator*(Unit<T,D> copy, const S& rhs) {
	return copy *= rhs;
}

template <typename T, typename D, typename S>
Unit<T,D> operator*(const S& lhs, Unit<T, D> copy) {
	return copy *= lhs;
}

template <typename T, typename D, typename S>
Unit<T,D> operator/(Unit<T,D> copy, const S& rhs) {
	return copy /= rhs;
}

template <typename X, typename Y, typename D1, typename D2>
Unit<MulType<X,Y>, MulType<D1,D2>> operator*(const Unit<X,D1>& lhs,
	                                         const Unit<Y,D2>& rhs) {
	return Unit<MulType<X,Y>, MulType<D1,D2>>(lhs.value() * rhs.value());
}

template <typename X, typename Y, typename D1, typename D2>
Unit<MulType<X,Y>, DivType<D1,D2>> operator/(const Unit<X,D1>& lhs,
	                                         const Unit<Y,D2>& rhs) {
	return Unit<MulType<X,Y>, DivType<D1,D2>>(lhs.value() / rhs.value());
}

using Mass = Dimension<1,0,0>;
using Length = Dimension<0,1,0>;
using Time = Dimension<0,0,1>;
using Velocity = Dimension<0,1,-1>;

Unit<float, Mass> kilogram(1.0);
Unit<float, Mass> gram(0.001);

template <typename T>
std::ostream& operator<<(std::ostream& os, const Unit<T, Mass>& unit)
{ return os << unit.value() << " L"; }

template <typename T>
std::ostream& operator<<(std::ostream& os, const Unit<T, Velocity>& unit)
{ return os << unit.value() << " L/T"; }


// Start of Quantity

template <typename R1, typename R2>
using IsMultiple = std::integral_constant<bool, (R1::num*R2::den) % (R2::num*R1::den) == 0>;

template <typename T, typename Ratio, typename Dimension>
class Quantity;

template <typename ToQuantity, typename X, typename R1, typename D>
ToQuantity quantity_cast(const Quantity<X,R1,D>& quantity)
{
	using R = typename ToQuantity::unit;
	using Y = typename ToQuantity::rep;
	return ToQuantity(static_cast<Y>(quantity.value()) * R1::num*R::den / (R1::den*R::num));
}

template <typename T, typename Ratio = std::ratio<1>, typename Dim = Dimension<1,0,0>>
class Quantity
{
public:
	using rep = T;
	using unit = Ratio;
	using dim = Dim;

	Quantity(const T& val) : value_(val) {}

	// Notice no case covers integral T but floating-point X
	template < typename X, typename R1,
		typename std::enable_if_t<
		    std::is_integral<T>::value &&
		    std::is_integral<X>::value &&
			IsMultiple<R1,Ratio>::value, int > = 0 >
	Quantity(const Quantity<X,R1>& rhs) {
		// New value is target quantity / this ratio, which enable_if guarantees to divide evenly
		value_ = rhs.value() * R1::num * Ratio::den / (R1::den * Ratio::num);
	}

	// The seemingly redundant `std::is_floating_point<X>::value` seems required to
	// make this overload conditionally dependent on X (otherwise a fail instantiating Quantity<T>)
	template < typename X, typename R1,
		typename std::enable_if_t<
		    (std::is_floating_point<T>::value && std::is_floating_point<X>::value) ||
		    (std::is_floating_point<T>::value && !std::is_floating_point<X>::value), int> = 0 >
	Quantity(const Quantity<X,R1>& rhs) {
		// New value is target quantity / this ratio, which enable_if guarantees to divide evenly
		value_ = rhs.value() * static_cast<T>(R1::num * Ratio::den) / static_cast<T>(R1::den * Ratio::num);
	}

	T& value() { return value_; }
	const T& value() const { return value_; }

	template <typename Q = Quantity<T,Ratio>>
	Q as() const { return quantity_cast<Q>(*this); }

	template <typename Q = Quantity<T,Ratio>>
	T asVal() const { return quantity_cast<Q>(*this).value(); }

	Quantity& operator+=(const Quantity& rhs) { value_ += rhs.value(); return *this; }
	Quantity& operator-=(const Quantity& rhs) { value_ -= rhs.value(); return *this; }
	template <typename X>
	Quantity& operator*=(const X& x) { value_ *= x; return *this; }
	template <typename X>
	Quantity& operator/=(const X& x) { value_ /= x; return *this; }

	friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
		return os << q.value() << " (" << Ratio::num << "/" << Ratio::den << ") [" << Dim::d1 << "," << Dim::d2 << "," << Dim::d3 << "]";
	}

private:
	T value_;
};

template <typename R1, typename R2>
using BaseUnit = std::ratio<1, R1::den*R2::den>;

template <typename X, typename Y, typename R1, typename R2>
using CommonDuration = typename std::common_type<std::chrono::duration<X,R1>, std::chrono::duration<Y,R2>>::type;

template <typename Duration, typename D>
using CommonQuantity = Quantity<typename Duration::rep, typename Duration::period, D>;

template <typename X, typename Y, typename R1, typename R2, typename D,
          typename ToQuantity = CommonQuantity<CommonDuration<X,Y,R1,R2>,D>>
ToQuantity operator+(const Quantity<X,R1,D>& lhs, const Quantity<Y,R2,D>& rhs)
{
	using R = typename ToQuantity::unit;
	return ToQuantity(quantity_cast<Quantity<X,R>>(lhs).value() + quantity_cast<Quantity<Y,R>>(rhs).value());
}

template <typename X, typename Y, typename R1, typename R2, typename D1, typename D2,
          typename ToQuantity = CommonQuantity<CommonDuration<X,Y,R1,R2>,MulType<D1,D2>>>
ToQuantity operator*(const Quantity<X,R1,D1>& lhs, const Quantity<Y,R2,D2>& rhs)
{
	using R = typename ToQuantity::unit;
	return ToQuantity(quantity_cast<Quantity<X,R>>(lhs).value() * quantity_cast<Quantity<Y,R>>(rhs).value());
}

template <typename X, typename Y, typename R,
          typename = std::enable_if_t<std::is_arithmetic<Y>::value>>
Quantity<MulType<X,Y>, R> operator*(const Quantity<X,R>& lhs, const Y& y)
{
	return Quantity<MulType<X,Y>, R>(lhs.value() * y);
}

template <typename X, typename Y, typename R,
          typename = std::enable_if_t<std::is_arithmetic<Y>::value>>
Quantity<MulType<X,Y>, R> operator*(const Y& y, const Quantity<X,R>& rhs)
{
	return Quantity<MulType<X,Y>, R>(rhs.value() * y);
}

template <typename X, typename Y, typename R>
Quantity<MulType<X,Y>, R> operator/(const Quantity<X,R>& lhs, const Y& y)
{
	return Quantity<MulType<X,Y>, R>(lhs.value() / y);
}

template <typename X, typename Y, typename R>
MulType<X,Y> operator/(const Quantity<X,R>& lhs, const Quantity<Y,R>& rhs)
{
	return MulType<X,Y>(lhs.value() / rhs.value());
}




// Helper types

using Meter = Quantity<float, std::ratio<1,1>>;
using Meter2 = Quantity<float, std::ratio<1,1>, Dimension<2,0,0>>;

using Centimeter = Quantity<float, std::ratio<1,100>>;
using Centimeter2 = Quantity<float, std::ratio<1,100>, Dimension<2,0,0>>;

using Millimeter = Quantity<float, std::milli>;

using Meter_d = Unit<Meter, Length>;

std::ostream& operator<<(std::ostream& os, const Meter& q)
{ return os << q.value() << " m"; }

std::ostream& operator<<(std::ostream& os, const Centimeter& q)
{ return os << q.value() << " cm"; }

std::ostream& operator<<(std::ostream& os, const Millimeter& q)
{ return os << q.value() << " mm"; }


std::ostream& operator<<(std::ostream& os, const Meter2& q)
{ return os << q.value() << " m^2"; }

std::ostream& operator<<(std::ostream& os, const Centimeter2& q)
{ return os << q.value() << " cm^2"; }

}
