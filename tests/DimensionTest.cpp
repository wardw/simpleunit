#include <iostream>
#include "gtest/gtest.h"
#include "hummingbird/solve/Dimension.h"

using namespace std;
using namespace hummingbird;

TEST(UnitsTest, Start)
{
	Dimension<float,1,0,0> two(2.f);
	cout << "1: " << two * two * 1.25f << endl;

	Length l1(2);
	Length l2(4.5f);
	Mass m1(4.5f);
	cout << "2: " << l1 + l2 << endl;
	//cout << "2: " << l1 + l2 + m1 << endl;
}
