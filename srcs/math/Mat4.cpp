#include "Mat4.hpp"

#include <cmath>
#include <stdexcept>

namespace
{
    const float PI = 3.14159265358979323846f;
    const float EPSILON = 1e-8f;
}

Mat4::Mat4()
{
    for (int ind = 0; ind < 16; ind++)
        data[ind] = 0.0f;
}

Mat4::Mat4(const Mat4& other)
{
    for (int ind = 0; ind < 16; ind++)
        data[ind] = other.data[ind];
}

Mat4::~Mat4()
{ }

Mat4 Mat4::identity()
{
    Mat4 result;

    result.at(0, 0) = 1.0f;
    result.at(1, 1) = 1.0f;
    result.at(2, 2) = 1.0f;
    result.at(3, 3) = 1.0f;

    return result;
}

float& Mat4::at(int row, int col)
{
    return data[col * 4 + row];
}

float Mat4::at(int row, int col) const
{
    return data[col * 4 + row];
}

Mat4 Mat4::translationMat(const Vec3& offset)
{
    Mat4 result = identity();

    result.at(0, 3) = offset.getX();
    result.at(1, 3) = offset.getY();
    result.at(2, 3) = offset.getZ();

    return result;
}

Mat4 Mat4::scalingMat(const Vec3& scale)
{
    Mat4 result = identity();

    result.at(0, 0) = scale.getX();
    result.at(1, 1) = scale.getY();
    result.at(2, 2) = scale.getZ();

    return result;
}

Mat4 Mat4::rotationXMat(float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = identity();

    result.at(1, 1) = c;
    result.at(1, 2) = -s;
    result.at(2, 1) = s;
    result.at(2, 2) = c;

    return result;
}

Mat4 Mat4::rotationYMat(float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = identity();

    result.at(0, 0) = c;
    result.at(0, 2) = s;
    result.at(2, 0) = -s;
    result.at(2, 2) = c;

    return result;
}

Mat4 Mat4::rotationZMat(float radians)
{
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = identity();

    result.at(0, 0) = c;
    result.at(0, 1) = -s;
    result.at(1, 0) = s;
    result.at(1, 1) = c;

    return result;
}

Mat4 Mat4::perspective(float verticalFovRadians,
                       float aspectRatio,
                       float nearPlane,
                       float farPlane)
{
    if (aspectRatio <= 0.0f)
        throw std::invalid_argument("Mat4::perspective: aspect ratio must be positive");
    if (nearPlane <= 0.0f || farPlane <= nearPlane)
        throw std::invalid_argument("Mat4::perspective: invalid near/far planes");
    if (verticalFovRadians <= 0.0f || verticalFovRadians >= PI)
        throw std::invalid_argument("Mat4::perspective: FOV must be between 0 and pi radians");

    const float tanHalfFov = std::tan(verticalFovRadians / 2.0f);

    Mat4 result;
    result.at(0, 0) = 1.0f / (aspectRatio * tanHalfFov);
    result.at(1, 1) = 1.0f / tanHalfFov;
    result.at(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.at(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    result.at(3, 2) = -1.0f;
    return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& worldUp)
{
    const Vec3 forward = (target - eye).normalized();
    if (forward.length() == 0)
        throw std::invalid_argument("Mat4::lookAt: eye and target must differ");

    Vec3 side = forward.cross(worldUp).normalized();
    if (side.length() == 0)
        throw std::invalid_argument("Mat4::lookAt: forward and worldUp must not be parallel");

    const Vec3 up = side.cross(forward).normalized();

    Mat4 result = identity();

    result.at(0, 0) = side.getX();
    result.at(0, 1) = side.getY();
    result.at(0, 2) = side.getZ();
    result.at(0, 3) = -side.dot(eye);

    result.at(1, 0) = up.getX();
    result.at(1, 1) = up.getY();
    result.at(1, 2) = up.getZ();
    result.at(1, 3) = -up.dot(eye);

    result.at(2, 0) = -forward.getX();
    result.at(2, 1) = -forward.getY();
    result.at(2, 2) = -forward.getZ();
    result.at(2, 3) = forward.dot(eye);

    return result;
}

Mat4 Mat4::operator=(const Mat4& other)
{
    return Mat4(other);
}

Mat4 Mat4::operator+(const Mat4& other) const
{
    Mat4 result;

    for (int ind = 0; ind < 16; ind++)
        result.data[ind] = data[ind] + other.data[ind];

    return result;
}

Mat4 Mat4::operator-(const Mat4& other) const
{
    Mat4 result;

    for (int ind = 0; ind < 16; ind++)
        result.data[ind] = data[ind] - other.data[ind];

    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const
{
    Mat4 result;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            float sum = 0.0f;

            for (int calc_ind = 0; calc_ind < 4; calc_ind++)
                sum += at(row, calc_ind) * other.at(calc_ind, col);

            result.at(row, col) = sum;
        }
    }
    return result;
}

Mat4& Mat4::translate(const Vec3& offset)
{
    *this = *this * translationMat(offset);
    return *this;
}

Mat4& Mat4::rotateX(float radians)
{
    *this = *this * rotationXMat(radians);
    return *this;
}

Mat4& Mat4::rotateY(float radians)
{
    *this = *this * rotationYMat(radians);
    return *this;
}

Mat4& Mat4::rotateZ(float radians)
{
    *this = *this * rotationZMat(radians);
    return *this;
}

Mat4& Mat4::scale(const Vec3& factors)
{
    *this = *this * scalingMat(factors);
    return *this;
}

Mat4& Mat4::scale(const float& scale)
{
    *this = *this * scalingMat(Vec3(scale, scale, scale));
    return *this;
}

Mat4& Mat4::scale(const int& scale)
{
    const float fScale = static_cast<float>(scale);

    *this = *this * scalingMat(Vec3(fScale, fScale, fScale));

    return *this;
}

Vec3 Mat4::transformPoint(const Vec3& point) const
{
    const float x = at(0, 0) * point.getX() + at(0, 1) * point.getY()
                  + at(0, 2) * point.getZ() + at(0, 3);
    const float y = at(1, 0) * point.getX() + at(1, 1) * point.getY()
                  + at(1, 2) * point.getZ() + at(1, 3);
    const float z = at(2, 0) * point.getX() + at(2, 1) * point.getY()
                  + at(2, 2) * point.getZ() + at(2, 3);
    const float w = at(3, 0) * point.getX() + at(3, 1) * point.getY()
                  + at(3, 2) * point.getZ() + at(3, 3);

    if (std::fabs(w) > EPSILON && std::fabs(w - 1.0f) > EPSILON)
        return Vec3(x / w, y / w, z / w);

    return Vec3(x, y, z);
}

Vec3 Mat4::transformDirection(const Vec3& direction) const
{
    return Vec3(
        at(0, 0) * direction.getX() + at(0, 1) * direction.getY() + at(0, 2) * direction.getZ(),
        at(1, 0) * direction.getX() + at(1, 1) * direction.getY() + at(1, 2) * direction.getZ(),
        at(2, 0) * direction.getX() + at(2, 1) * direction.getY() + at(2, 2) * direction.getZ()
    );
}
