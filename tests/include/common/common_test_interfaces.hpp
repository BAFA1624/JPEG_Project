#pragma once

#include <concepts>

namespace PNG
{

template <typename T>
concept TestInput = std::is_integral<T>::value;

} // namespace PNG
