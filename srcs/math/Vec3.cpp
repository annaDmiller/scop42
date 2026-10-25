#include "Vec3.hpp"

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f)
{ }

Vec3::Vec3(float xValue, float yValue, float zValue)
    : x(xValue), y(yValue), z(zValue)
{ }

Vec3::Vec3(const Vec3& other)
    : x(other.x), y(other.y), z(other.z)
{ }

Vec3::~Vec3()
{ }

Vec3 Vec3::operator=(const Vec3& other)
{
    return Vec3(other.x, other.y, other.z);
}

Vec3 Vec3::operator+(const Vec3& other) const
{
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(float scalar) const
{
    return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::operator/(float scalar) const
{
    if (scalar == 0.0f)
        return Vec3();

    return Vec3(x / scalar, y / scalar, z / scalar);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3& Vec3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

float Vec3::dot(const Vec3& other) const
{
    return (x * other.x + y * other.y + z * other.z);
}

Vec3 Vec3::cross(const Vec3& other) const
{
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vec3::length() const
{
    float lengthSquared = dot(*this);
    return std::sqrt(lengthSquared);
}

Vec3 Vec3::normalized() const
{
    const float magnitude = length();

    if (magnitude == 0.0f)
        return Vec3();

    return *this / magnitude;
}

float Vec3::getX() const
{
    return x;
}

float Vec3::getY() const
{
    return y;
}

float Vec3::getZ() const
{
    return z;
}

float Vec3::dot(const Vec3& a, const Vec3& b)
{
    return a.dot(b);
}

Vec3 Vec3::cross(const Vec3& a, const Vec3& b)
{
    return a.cross(b);
}

Vec3 operator*(float scalar, const Vec3& vector)
{
    return vector * scalar;
}
