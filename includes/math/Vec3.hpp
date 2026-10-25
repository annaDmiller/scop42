#ifndef SCOP_VEC3_HPP
# define SCOP_VEC3_HPP

#include <cmath>

class Vec3
{
    private:
    float x;
    float y;
    float z;

    public:
    Vec3();
    Vec3(float xValue, float yValue, float zValue);
    Vec3(const Vec3& other);
    ~Vec3();

    Vec3 operator=(const Vec3& other);
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const;

    Vec3& operator+=(const Vec3& other);
    Vec3& operator-=(const Vec3& other);
    Vec3& operator*=(float scalar);

    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;

    float length() const;

    // Returns a unit vector. A zero-length vector returns (0, 0, 0).
    Vec3 normalized() const;

    float getX() const;
    float getY() const;
    float getZ() const;

    static float dot(const Vec3& a, const Vec3& b);
    static Vec3 cross(const Vec3& a, const Vec3& b);
};

Vec3 operator*(float scalar, const Vec3& vector);

#endif
