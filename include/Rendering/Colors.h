#ifndef COLORS_H
#define COLORS_H

#include "Color.h"

// collection of popular colors
namespace Colors {
inline constexpr Color Black      {0, 0, 0};
inline constexpr Color White      {255, 255, 255};
inline constexpr Color Red        {0, 0, 255};
inline constexpr Color Green      {0, 255, 0};
inline constexpr Color Blue       {255, 0, 0};
inline constexpr Color Purple     {255, 0, 200};
inline constexpr Color Orange     {0, 100, 255};
inline constexpr Color Gray       {127, 127, 127};
inline constexpr Color Transparent {0, 0, 0, 0};
}

#endif // COLORS_H
