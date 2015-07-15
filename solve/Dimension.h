#include <iostream>

namespace hummingbird {

template <typename T, int m, int l, int t>
class Dimension
{
public:
	explicit Dimension(const T& val) : value_(val) {}

	T& value() { return value_; }
	const T& value() const { return value_; }

	Dimension& operator+=(const Dimension& rhs) { value_ += rhs; return *this; }
	Dimension& operator-=(const Dimension& rhs) { value_ -= rhs; return *this; }
	template <typename S>
	Dimension& operator*=(const S& rhs) { value_ *= rhs; return *this; }
	template <typename S>
	Dimension& operator/=(const S& rhs) { value_ /= rhs; return *this; }

private:
	T value_;

	friend std::ostream& operator<<(std::ostream& os, const Dimension& dimension) {
		return os << dimension.value_ << " units";
	}
};

namespace
{
	template <typename X, typename Y>
	using AddType = decltype(std::declval<X>() + std::declval<Y>());

	template <typename X, typename Y>
	using MulType = decltype(std::declval<X>() * std::declval<Y>());
}

template <typename X, typename Y, int m, int l, int t>
Dimension<AddType<X,Y>, m, l, t> operator+(const Dimension<X, m, l, t>& lhs,
	                                       const Dimension<Y, m, l, t>& rhs) {
	return Dimension<AddType<X,Y>, m, l, t>(lhs.value() + rhs.value());
}

template <typename X, typename Y, int m, int l, int t>
Dimension<AddType<X,Y>, m, l, t> operator-(const Dimension<X, m, l, t>& lhs,
	                                       const Dimension<Y, m, l, t>& rhs) {
	return Dimension<AddType<X,Y>, m, l, t>(lhs.value() - rhs.value());
}

template <typename T, int m1, int l1, int t1, typename S>
Dimension<T, m1, l1, t1> operator*(Dimension<T, m1, l1, t1> copy, const S& rhs) {
	return copy *= rhs;
}

template <typename T, int m1, int l1, int t1, typename S>
Dimension<T, m1, l1, t1> operator*(const S& lhs, Dimension<T, m1, l1, t1> copy) {
	return copy *= lhs;
}

template <typename X, typename Y, int m1, int l1, int t1,
                      			  int m2, int l2, int t2>
Dimension<MulType<X,Y>, m1+m2, l1+l2, t1+t2> operator*(const Dimension<X, m1, l1, t1>& lhs,
	                                                   const Dimension<Y, m2, l2, t2>& rhs) {
	return Dimension<MulType<X,Y>, m1+m2, l1+l2, t1+t2>(lhs.value() * rhs.value());
}

template <typename X, typename Y, int m1, int l1, int t1,
                      			  int m2, int l2, int t2>
Dimension<MulType<X,Y>, m1-m2, l1-l2, t1-t2> operator/(const Dimension<X, m1, l1, t1>& lhs,
	                                                   const Dimension<Y, m2, l2, t2>& rhs) {
	return Dimension<MulType<X,Y>, m1-m2, l1-l2, t1-t2>(lhs.value() / rhs.value());
}


using Mass = Dimension<float,1,0,0>;
using Length = Dimension<float,0,1,0>;
using Time = Dimension<float,0,0,1>;


template <typename D>
class Quantity
{

};

Mass kilogram(1.0);
//Quantity<Mass> kilogram(1.0);

}

