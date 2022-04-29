#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

// 返回角度 degrees 对应的弧度值
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// 返回一个 [0, 1) 之间的double
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

// 返回 [min, max)之间的double 
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}   

// 将 x 限制在 min 和 max 之间
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "../Render/ray.h"
#include "vec3.h"