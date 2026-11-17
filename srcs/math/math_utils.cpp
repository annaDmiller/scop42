#include <math_utils.hpp>

bool isFinite(float value)
{
    return value == value &&
            value <= std::numeric_limits<float>::max() &&
            value >= -std::numeric_limits<float>::max();
}
