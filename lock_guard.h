#pragma once

#include "thread_annotations.hpp"

#include <mutex>

namespace ktl
{
namespace mutex
{
template<typename T>
using lock_guard = std::lock_guard<T>;
}
}