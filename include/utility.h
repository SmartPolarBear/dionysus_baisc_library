#pragma once

#include <algorithm>

namespace kbl
{

template<typename T>
struct default_deleter
{
	void operator()(T data)
	{
		delete data;
	}
};

template<typename T>
using less = std::less<T>;

template<typename T>
using greater = std::greater<T>;

}