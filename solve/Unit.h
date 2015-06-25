#include <iostream>


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
class Dimension {};

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
{ return os << unit.value() << " kg"; }

template <typename T>
std::ostream& operator<<(std::ostream& os, const Unit<T, Velocity>& unit)
{ return os << unit.value() << " m/s"; }

template <typename R1, typename R2>
using IsMultiple = std::integral_constant<bool, (R1::num*R2::den) % (R2::num*R1::den) == 0>;

template <typename T, typename Ratio = std::ratio<1>>
class Quantity
{
public:
	using rep = T;
	using unit = Ratio;

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

	Quantity& operator+=(const Quantity& rhs) { value_ += rhs.value(); return *this; }
	Quantity& operator-=(const Quantity& rhs) { value_ -= rhs.value(); return *this; }
	template <typename X>
	Quantity& operator*=(const X& x) { value_ *= x; return *this; }
	template <typename X>
	Quantity& operator/=(const X& x) { value_ /= x; return *this; }

	friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
		return os << q.value() << " (" << Ratio::num << "/" << Ratio::den << ")";
	}

private:
	T value_;
};

// Todo: make use of common_type specialisation for chrono::duration
template <typename R1, typename R2>
using BaseUnit = std::ratio<1, R1::den*R2::den>;

template <typename X, typename Y, typename R1, typename R2>
Quantity<AddType<X,Y>, BaseUnit<R1,R2>> operator+(const Quantity<X,R1>& lhs, const Quantity<Y,R2>& rhs)
{
	return Quantity<AddType<X,Y>, BaseUnit<R1,R2>>(R1::num * R2::den * lhs.value() +
												   R2::num * R1::den * rhs.value());
}

template <typename X, typename Y, typename R>
Quantity<MulType<X,Y>, R> operator*(const Quantity<X,R>& lhs, const Y& y)
{
	return Quantity<MulType<X,Y>, R>(lhs.value() * y);
}

template <typename X, typename Y, typename R>
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




// Later

// template <typename X, typename Y, typename R1, typename R2>
// Quantity<MulType<X,Y>, std::ratio_multiply<R1,R2>> operator*(const Quantity<X,R1>& lhs, const Quantity<Y,R2>& rhs)
// {
// 	return Quantity<MulType<X,Y>, std::ratio_multiply<R1,R2>>(lhs.value() * rhs.value());
// }


// Helper types

using Meter = Quantity<float, std::ratio<1,1>>;
using Centimeter = Quantity<int, std::ratio<1,100>>;
using Millimeter = Quantity<int, std::milli>;

using Meter_d = Unit<Meter, Length>;



}
