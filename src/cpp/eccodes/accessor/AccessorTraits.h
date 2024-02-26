#pragma once

#include <type_traits>
#include <string>
#include <vector>

// Trait definitions for accessor templates 

struct grib_expression;

namespace eccodes::accessor {

using StringArray = std::vector<std::string>;

template <typename Test, typename... List>
using isOneOf = std::disjunction<std::is_same<Test, List>...>;

template<typename T>
using isAllowedSimpleType = isOneOf<T, long, float, double>;

template<typename T>
using isAllowedBufferType = isOneOf<T, std::string, std::vector<long>, std::vector<float>, std::vector<double>,
                                       std::vector<StringArray>, std::vector<unsigned char>>;

template<typename T>
using isAllowedUnpackElementType = isOneOf<T, float, double>;

template<typename T>
struct isAllowedPackType : std::integral_constant<bool,
                                    isAllowedSimpleType<T>::value ||
                                    isAllowedBufferType<T>::value ||
                                    std::is_same<T, grib_expression>::value
                                    > {};

template<typename T>
struct isAllowedUnpackType : std::integral_constant<bool,
                                    isAllowedSimpleType<T>::value ||
                                    isAllowedBufferType<T>::value
                                    > {};

} // namespace eccodes::accessor
