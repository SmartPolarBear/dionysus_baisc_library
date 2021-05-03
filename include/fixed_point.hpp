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
	fixed_point() = default;
	~fixed_point() = default;
	fixed_point(const fixed_point&) = default;
	fixed_point(fixed_point&&) noexcept = default;
	fixed_point& operator=(const fixed_point&) = default;

	explicit fixed_point(float n)
		: data_{ int32_t(n * FACT) }
	{
	}

	explicit fixed_point(int32_t n)
		: data_{ n * FACT }
	{
	}

	fixed_point add(const fixed_point& another) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ + another.data_;
		return ret;
	}

	fixed_point subtract(const fixed_point& another) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ - another.data_;
		return ret;
	}

	fixed_point add(int32_t n) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ + n * FACT;
		return ret;
	}

	fixed_point subtract(int32_t n) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ - n * FACT;
		return ret;
	}

	fixed_point multiply(const fixed_point& another) const
	{
		fixed_point ret{ 0 };
		ret.data_ = ((int64_t)data_) * another.data_ / FACT;
		return ret;
	}

	fixed_point divide(const fixed_point& another) const
	{
		fixed_point ret{ 0 };
		ret.data_ = ((int64_t)data_) * FACT / another.data_;
		return ret;
	}

	fixed_point multiply(int32_t n) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ * n;
		return ret;
	}

	fixed_point divide(int32_t n) const
	{
		fixed_point ret{ 0 };
		ret.data_ = data_ / n;
		return ret;
	}

	fixed_point operator+(const fixed_point& another) const
	{
		return this->add(another);
	}

	fixed_point operator+(int32_t another) const
	{
		return this->add(another);
	}

	fixed_point operator-(const fixed_point& another) const
	{
		return this->subtract(another);

	}

	fixed_point operator-(int32_t another) const
	{
		return this->subtract(another);

	}

	fixed_point operator*(const fixed_point& another) const
	{
		return this->multiply(another);
	}

	fixed_point operator*(int32_t another) const
	{
		return this->multiply(another);
	}

	fixed_point operator/(const fixed_point& another) const
	{
		return this->divide(another);
	}

	fixed_point operator/(int32_t another) const
	{
		return this->divide(another);
	}

	[[nodiscard]] int32_t to_int(roundings rounding = roundings::ROUND_TO_NEAREST) const
	{
		if (rounding == roundings::ROUND_TO_NEAREST)
		{
			return data_ / FACT;
		}
		else
		{
			if (data_ > 0)
			{
				return (data_ + FACT / 2) / FACT;
			}
			else
			{
				return (data_ - FACT / 2) / FACT;
			}
		}
	}

	[[nodiscard]] float to_float() const
	{
		return data_ / float(FACT);
	}

	auto operator<=>(const fixed_point& another) const = default;
 private:
	static constexpr int32_t FACT = 1 << Q;

	int32_t data_{ 0 };
};

using fixed_point_17dot14 = fixed_point<17, 14>;

}