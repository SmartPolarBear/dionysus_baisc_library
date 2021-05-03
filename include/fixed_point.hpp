#pragma once

namespace kbl
{

/// \brief A P.Q fixed point implementation
/// \details The following table summarizes how fixed-point arithmetic operations can be implemented in C.
/// 		In the table, x and y are fixed-point numbers, n is an integer, fixed-point numbers are in signed p.q format where p + q = 31, and f is 1 << q:
///         Convert n to fixed point:	n * f
///         Convert x to integer (rounding toward zero):	x / f
///         Convert x to integer (rounding to nearest):	(x + f / 2) / f if x >= 0,
///         											(x - f / 2) / f if x <= 0.
///         Add x and y:	x + y
///         Subtract y from x:	x - y
///         Add x and n:	x + n * f
///         Subtract n from x:	x - n * f
///         Multiply x by y:	((int64_t) x) * y / f
///         Multiply x by n:	x * n
///         Divide x by y:	((int64_t) x) * f / y
///         Divide x by n:	x / n

template<uint32_t P, uint32_t Q>
requires (P + Q < 32)
class fixed_point
{
 public:
	enum class [[clang::enum_extensibility(closed)]] roundings
	{
		ROUND_TO_NEAREST,
		ROUND_TOWARDS_0
	};

 public:

	explicit fixed_point(uint32_t n)
		: data_{ n * FACT }
	{

	}

	fixed_point add(const fixed_point& another)
	{
		return data_ + another.data_;
	}

	fixed_point subtract(const fixed_point& another)
	{
		return data_ - another.data_;
	}

	fixed_point add(int32_t n)
	{
		return data_ + n * FACT;
	}

	fixed_point subtract(int32_t n)
	{
		return data_ - n * FACT;
	}

	[[nodiscard]] int32_t to_int(roundings rounding = roundings::ROUND_TO_NEAREST)
	{
		if (rounding == roundings::ROUND_TO_NEAREST)
		{

		}
		else
		{

		}
	}
 private:
	static constexpr int32_t FACT = 1 << Q;

	int32_t data_{ 0 };
};

}