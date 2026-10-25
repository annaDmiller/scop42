#ifndef SCOP_MAT4_HPP
# define SCOP_MAT4_HPP

#include "Vec3.hpp"

class Mat4
{
    private:
    float data[16];

    public:
    Mat4();
    Mat4(const Mat4& other);
    ~Mat4();

    float& at(int row, int col);
    float at(int row, int col) const;

    Mat4 operator=(const Mat4& other);
    Mat4 operator+(const Mat4& other) const;
    Mat4 operator-(const Mat4& other) const;
    Mat4 operator*(const Mat4& other) const;

    static Mat4 identity();
    static Mat4 translationMat(const Vec3& offset);
    static Mat4 scalingMat(const Vec3& scale);
    static Mat4 rotationXMat(float radians);
    static Mat4 rotationYMat(float radians);
    static Mat4 rotationZMat(float radians);

    Mat4& translate(const Vec3& offset);
    Mat4& rotateX(float radians);
    Mat4& rotateY(float radians);
    Mat4& rotateZ(float radians);
    Mat4& scale(const Vec3& factors);
    Mat4& scale(const float& scale);
    Mat4& scale(const int& scale);

    // OpenGL-style perspective projection (clip-space Z range -1 to +1).
    static Mat4 perspective(float verticalFovRadians,
                            float aspectRatio,
                            float nearPlane,
                            float farPlane);

    // Builds a view matrix from camera position, target, and world-up direction.
    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& worldUp);

    // Applies this matrix to a point (implicit homogeneous w=1).
    // Perspective-divides if the resulting w is nonzero and not 1.
    Vec3 transformPoint(const Vec3& point) const;

    // Applies this matrix to a direction (implicit homogeneous w=0); ignores translation.
    Vec3 transformDirection(const Vec3& direction) const;
};

#endif
