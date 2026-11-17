#include <Vec2.hpp>

Vec2::Vec2() : x(0.0f), y(0.0f)
{ }

Vec2::Vec2(float xValue, float yValue)
    : x(xValue), y(yValue)
{ }

Vec2::Vec2(const Vec2& other)
    : x(other.x), y(other.y)
{ }

Vec2::~Vec2()
{ }

float Vec2::getX() const
{
    return x;
}

float Vec2::getY() const
{
    return y;
}

bool Vec2::isValidVector() const
{
    return (isFinite(x) && isFinite(y));
}
