#ifndef INMACH_MATTRO_TYPES_HPP_INCLUDED
#define INMACH_MATTRO_TYPES_HPP_INCLUDED

#include <chrono>

namespace inmach {
namespace mattro {

typedef std::chrono::steady_clock clock_type;
typedef std::chrono::time_point<clock_type> timestamp_type;

} // namespace mattro
} // namespace inmach

#endif	// INMACH_MATTRO_TYPES_HPP_INCLUDED