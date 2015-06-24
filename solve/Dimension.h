#include <iostream>

namespace
{
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

template <typename T>
template <typename T>
std::ostream& operator<<(std::ostream& os, const Unit<T, Velocity>& mps)
{ return os << mps.value() << " m/s"; }


}
