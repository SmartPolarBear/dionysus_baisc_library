#include <cstdint>

#include <gtest/gtest.h>

#include "fixed_point.hpp"

using namespace kbl;

TEST(FixPointTest, Basic)
{
	fixed_point_17dot14 f1{ -10 }, f2{ 10 };

	ASSERT_EQ(f1.to_int(), -10);
	ASSERT_EQ(f2.to_int(), 10);

	auto f3 = f1 + f2;

	ASSERT_EQ(f3.to_int(), 0);

	f1 = f1 / 3;
	ASSERT_EQ(f1.to_int(), -3);
	ASSERT_LE(f1.to_float() - (-10.0 / 3), 1e-3);

	f2 = f2 / 3;
	ASSERT_EQ(f2.to_int(), 3);
	ASSERT_LE(f2.to_float() - (10.0 / 3), 1e-3);

	f3 = f1 + f2;
	ASSERT_EQ(f3.to_int(), 0);
}

TEST(FixPointTest, Floating)
{
	fixed_point_17dot14 f1{ 10.25f }, f2{ 0.25f };
	ASSERT_LE(f1.to_float() - 10.25, 1e-3);
	ASSERT_LE(f2.to_float() - 0.25, 1e-3);

	auto f3 = f1 - f2;
	ASSERT_EQ(f3.to_int(), 10);

	auto f4 = f3 * f2;
	ASSERT_EQ(f4.to_int(), int32_t(10 * 0.25));
}