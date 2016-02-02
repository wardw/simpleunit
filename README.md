
### Simpleunit - C++ units library

A simple C++ units library with compile-time unit checking and automatic arithmetic conversions between quantities and dimensions. The method makes use of Barton & Nackman's method for dimensional analysis [1] and a method for representing quantities similar in style to the approach of `std::chrono` for representing durations of time. The result is a `Unit` type consistent in semantics to `std::chrono::duration` but extended to arbitrary dimensions.

Under the `Unit` abstraction, the class has a single type `T` to represent the unit's value. An object of type `Unit` should therefore compile equivalently to using `T` directly, i.e. the object will be of the same size.

The library is header only, with a single header `simpleunit/Unit.h`. If you want to build the tests there is a CMakeLists.txt for building the tests with CMake and Google Test.

#### Example

	#include "simpleunit/Unit.h"
	using namespace sunit::si;

	Meters height(5);
	Centimeters width(200);

	// Prints: flowrate = 757.576 cm^2/s
	cout << "flowrate = " << width * height / Seconds(132) << endl;

The resultant type of the expression is consistent with the dimensions of the operands and at a scale common to all operands. In this case, the type's scale is reported as "cm^2/s". For integer types, conversions between unit magnitudes are only possible to units of a finer gradation, such that no information is lost. Floating-point types are not limited by this restriction yet follow the same behaviour for consistency. If you want to represent a value at a different scale, a convenience member function is provided

	auto flowrate = .. // as before

	// Prints: flowrate = 115.227 in^2/s
	cout << "flowrate = " << flowrate.as<Inches2_Second>() << endl;

or alternatively by a `unit_cast`

    auto f2 = sunit::unit_cast<Inches2_Second>(flowrate);

Since a unit's dimensionality is encoded within the type, all operations are checked at compile time for errors between incompatible dimensions. Arithmetic operations are defined only for those operations that are valid and consistent with the algebra. Attempting

	auto foo = height + Seconds(2);  // Compile error: invalid operands 'Meters' and 'Seconds'

is a compile error, whereas the following would succeed

	auto foo = height / Seconds(2);  // Ok: returns unit of Meters_Second

A `unit_cast` follows the same constraints in checking for dimensional consistency. If you do want to completely cast a unit to another unrelated unit, you can do so explicitly with a `dimension_cast` of the same interface.

### The `Unit` type

Units like `Centimeters` and `Meters_Second` are simple type aliases for a general `Unit` type, and so give convenient names to those units most used in practice. Some common SI units are defined in `sunit::si` (todo!), but it is straightforward to alias new types as necessary.  For example, `Meters_Second` is

	using Meters_Second = Unit<float, Velocity<meter, second>>;

where `Velocity` is itself an alias for a `BaseUnit` type that captures the notion of Length/Time.  All `BaseUnit`s like `Velocity` are templated with the types necessary to fully represent the unit's scale, one scale for each physical dimension. A `Unit` is thus composed of a value of type `T` that represents some scalar coefficient under a base of type `BaseUnit`.

#### Unit scale

In the example above, the types `meter` and `second` are aliases for types of `std::ratio`, a rational compile-time constant representing the unit's magnitude, presumably to be understood with respect to some (implicit) reference. This might typically be SI or MKS (meters, kilograms, seconds).

Using `std::ratio` has the distinct advantage that values are represented as rational numbers and are known at compile time, with any operations between `ratio`s always reduced to their lowest terms. All unit conversions within the library make use of compile-time ratio arithmetic and so reduce the risk of runtime overflow, for example in intermediate calculations. The use of `std::ratio` is likewise analogous to the behaviour in `std::chrono::duration` for representing magnitudes of time.

One awkward design point, there is a difference between what you might define as an `inch` scale

	using inch = std::ratio<1,39>;

and a floating-point quantity of length, expressed as `Unit`s of `Inches`

	using Inches = Unit<float, Length<inch>>;
    ..
	Inches gap = 4;

But in general any naming convention could be used. The Standard Library also provides type aliases for [common SI prefixes][b] like `std::kilo`, `std::mega`, etc.

### The `BaseUnit` type

Types like `Length` and `Velocity` are type aliases for a `BaseUnit` type that captures the notion of dimensionality and scale in a general way. By example, the fundamental units `Length` and `Time` are aliases for

	template <typename r1> using Length = BaseUnit<Dim<1,0>, r1>;
	template <typename r1> using Time   = BaseUnit<Dim<0,1>, r1>;

and derived units like `Velocity` work in a similar way

	template <typename r1, typename r2> using Velocity = BaseUnit<Dim<1,-1>, r1, r2>;

This starts to get quite syntactically noisy, but in practice the intention is that aliases would be defined for all common units in use. This approach makes separate aliases for dimension and scale very composable. One restriction is that for any dimension, only a single scale can be represented - it is not possible to represent an area using the heterogeneous length units 'feet.meters'. You must choose either meters^2 or feet^2, and is a clear limitation of this approach. ([Boost.Units][c] has you covered).

### Summary

A `Unit` looks like

	sunit::Unit<T, BaseUnit>

with a `BaseUnit` comprising of a dimension `Dim` and scales `r1 .. r7`

	sunit::BaseUnit<Dim, r1, r2, r3, .., r7>

A `Dim` represents each dimension by its exponent

    sunit::Dim<d1,d2,d3,d4,d5,d6,d7>

where each dimension `d1 .. d7` must match the scale `r1 .. r7`.

### Acknowledgements

The concept of a `Dim` representing dimensional exponents is adopted from Barton & Nackman [1].

### Limitations

One limitation is the very un-special handling of units under multiplication and division. For example, rules for integer division follow identically to plain integers and no special handling is taken by the unit type

	auto val = Unit<int, Length<meter>>(1) / 2;  // result: val == 0

It might be more useful if (similar to addition) the result accommodated a conversion of the unit type and so made use of the type's internal rational arithmetic to resolve to some finer scale where necessary. A floating-point value representation neatly avoids the problem, although in this instance an opportunity is missed since we essentially account for the exponent in storage twice (both in the floating-point value, and in the unit). Regardless, in both cases no effort is made to harmonize the two.

For this project, behaviour is simply deferred to the type `T` and the normal rules for `T` apply. For common integer and floating-point types this will be immediately recognizable. Comments and suggestions welcome!


### Todo

+ `constexpr`. Make Unit a literal class
+ Fill out a basic set of SI unit aliases & unit strings
+ User defined literal operator
+ Simplify printing of generic units that have no `ostream` overload
+ Extend to 7 dimensions when happy
+ Check assembly output
+ overload common_type?

### References

[1]: Scientific and Engineering C++. John J. Barton & Lee R. Nackman

[a]: http://en.cppreference.com/w/cpp/chrono/duration
[b]: http://en.cppreference.com/w/cpp/numeric/ratio/ratio
[c]: http://www.boost.org/doc/libs/1_58_0/doc/html/boost_units.html
