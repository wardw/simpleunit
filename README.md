
### Nful - No frills units library

A C++ units library building on Barton & Nackman's Dimensional Analysis [1] and the STL [`std::chrono`][a] library. The library combines similar semantics to `std::chrono::duration` for representing quantities of time with an extension to multiple dimensions.  Scale and dimensionality are included within the type, and so the library allows for both static compile time checking of arithmetic operations between consistent dimensions and for numeric conversions between equivalent units of varying scales.

Under the type abstraction, the `Unit` class has a single value type `T` representing the unit's value, and so should handle equivalently to using `T` directly. (todo: comparison check)

The library is header only, with a single header `nful/Unit.h`.  There is a CMakeLists.txt to build the tests with CMake (that depends on Google Test).

### Example usage

	#include "nful/Unit.h"
	using namespace nful::si;

	Meters height(5);
	Centimeters width(200);

	cout << "flowrate = " << width * height / Seconds(132) << endl;

Prints: `flowrate = 757.576 cm^2/s`.

The resulting type of the arithmetic expression one consistent with the dimensions of the expression and at a scale that is common to the operands. In this case, the type is reported as 'cm^2/s' -- and is selected over 'm^2/s' in order that the most information is retained. This is essential for integer types, where conversions are only possible to units of a finer gradation (and so no information is lost) and preferred for floating-point types in order that the greatest precision is retained. This behaviour is also similar to `std::chrono::duration`.

To represent a value under a different base, a convenience type conversion is provided

	auto flowrate = .. // as before
	cout << "flowrate = " << flowrate.as<Inches2_Second>() << endl;

Prints: `flowrate = 115.227 in^2/s`

or alternatively by `unit_cast`

    auto f2 = nful::unit_cast<Inches2_Second>(flowrate);

All conversions are checked statically at compile time, with each unit's dimensionality expressed as part of the type. Arithmetic operations on units are only defined for those operations that are valid and consistent with the algebra. Attempting

	auto foo = height + Seconds(2);  // Compile error: invalid operands 'Meters' and 'Seconds'

is a compile error, whereas the following succeeds

	auto foo = height / Seconds(2);  // Ok: returns unit of Meters_Second

### The `Unit` type

Units like `Centimeters` and `Meters_Second` are just type aliases for a general `Unit` type, and so give convenient names to those units used in practice. Some common SI units are defined in `nful::si` (todo!), but it is straightforward to alias new types as necessary.  For example, `Meters_Second` is aliased as

	using Meters_Second = Unit<float, Velocity<meter, second>>;

where `Velocity` is itself an alias for a `BaseUnit` type that captures the notion of Length/Time.  All `BaseUnit`s like `Velocity` are templated with the types necessary to fully represent the unit's scale, one scale for each physical dimension. A `Unit` is thus composed of a value of type `T`, representing some scalar coefficient under a base of type `BaseUnit`.

#### Unit scale

The types `meter` and `second` are aliases for types of `std::ratio`, and so represent an expression of unit scale, presumably with respect to some (implicit) reference. This might typically be MKS (meters, kilograms, seconds) or SI.

Using `std::ratio` has the distinct advantage that values are represented as rational numbers and are known at compile time, with any operations between `ratio`s always reduced to their lowest terms. All unit conversions within the library make use of compile-time ratio arithmetic and so reduce the risk of runtime overflow, for example in intermediate calculations. The use of `std::ratio` is likewise analogous to the behaviour in `std::chrono::duration` for representing magnitudes of time.

One awkward design point, there is a difference between what you might define as an `inch` scale

	using inch = std::ratio<1,39>;

and a floating-point quantity of length, expressed as `Unit`s of `Inches`

	using Inches = Unit<float, Length<inch>>;
    ..
	Inches gap = 4;

But in general any naming convention could be used. Naming units like `Meters2_Second` might get quite awkward quite quickly, so perhaps a better convention could be used, perhaps project specific.  (An alternative short form might be just to use the conventions for labelling units, such as `m2_s`.) The Standard Library also provides type aliases for [common SI prefixes][b] like `std::kilo`, `std::mega`, etc.

### The `BaseUnit` type

Types like `Length` and `Velocity` are type aliases for a `BaseUnit` type that captures the notion of dimensionality and scale in a general way. By example, the fundamental units `Length` and `Time` are aliases for

	template <typename r1> using Length = BaseUnit<Dim<1,0>, r1>;
	template <typename r1> using Time   = BaseUnit<Dim<0,1>, r1>;

and derived units like `Velocity` work in a similar way

	template <typename r1, typename r2> using Velocity = BaseUnit<Dim<1,-1>, r1, r2>;

This starts to get quite noisy syntactically, but the motivation is that in practice aliases are defined for the typical units in use. This approach makes separate aliases for dimension and scale very composable. One restriction is that for any dimension, only a single scale can be represented - it is not possible to represent an area using the heterogeneous length units 'feet.meters'. You must choose either meters^2 or feet^2, and is a clear limitation of this approach.

### Summary

In full, a `Unit` looks like

	nful::Unit<T, BaseUnit>

with a `BaseUnit` composing a dimension `Dim` and scales `r1 .. r7`

	nful::BaseUnit<Dim, r1, r2, r3, .., r7>

A `Dim` represents each dimension by its exponent

    nful::Dim<d1,d2,d3,d4,d5,d6,d7>

### Acknowledgements

The concept of a `Dim` representing dimensional exponents is adopted from Barton & Nackman [1] in their chapter on Dimensional Analysis.

As an aside, Walter Brown, a long standing ISO C++ committee member, explained at cppcon14 that he has ambitions for a future units library for the C++ Standard Library, but that he needs one more core language feature to do a units library right.. Still, I needed something reasonably straightforward and this suited my purposes. (Walter Brown is also the principal author of `std::ratio`).


### Todo

+ constexpr. Make Unit a literal class
+ Fill out a basic set of SI unit aliases & unit strings
+ User defined literal operator
+ Simplify printing of generic units that have no ostream overload
+ Extend to 7 dimensions when happy
+ Check assembly output
+ overload common_type?

So still incomplete and expect breaking changes!


[1]: Scientific and Engineering C++. John J. Barton & Lee R. Nackman

[a]: http://en.cppreference.com/w/cpp/chrono/duration
[b]: http://en.cppreference.com/w/cpp/numeric/ratio/ratio
