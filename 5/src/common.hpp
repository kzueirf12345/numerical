#pragma once

#include <concepts>
#include <functional>

template <std::floating_point T>
using BaseRngT = std::function<T()>;