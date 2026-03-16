#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <array>
#include <cmath>
#include <iostream>

struct Transform
{
    Transform* parent = nullptr;

    struct Vec3 { float x = 0, y = 0, z = 0; };
    Vec3 position;
    Vec3 rotation; // en grados
    Vec3 scale{ 1,1,1 };

    // Matriz 4x4 simplificada
    struct Mat4
    {
        std::array<float, 16> m;

        Mat4() { for (int i = 0; i < 16; i++) m[i] = (i % 5 == 0 ? 1 : 0); } // identidad

        void print() const
        {
            for (int i = 0; i < 4; i++)
                std::cout << m[i * 4 + 0] << " " << m[i * 4 + 1] << " " << m[i * 4 + 2] << " " << m[i * 4 + 3] << "\n";
            std::cout << "\n";
        }

        Mat4 operator*(const Mat4& other) const
        {
            Mat4 result;
            for (int row = 0; row < 4; row++)
                for (int col = 0; col < 4; col++)
                {
                    result.m[row * 4 + col] = 0;
                    for (int k = 0; k < 4; k++)
                        result.m[row * 4 + col] += m[row * 4 + k] * other.m[k * 4 + col];
                }
            return result;
        }
    };

    static Mat4 translate(const Vec3& v)
    {
        Mat4 t;
        t.m[12] = v.x; t.m[13] = v.y; t.m[14] = v.z;
        return t;
    }

    static Mat4 scale_matrix(const Vec3& v)
    {
        Mat4 s;
        s.m[0] = v.x; s.m[5] = v.y; s.m[10] = v.z;
        return s;
    }

    static Mat4 rotateY(float angle)
    {
        Mat4 r;
        float rad = angle * 3.14159265f / 180.0f;
        float c = cos(rad), s = sin(rad);
        r.m[0] = c; r.m[2] = s;
        r.m[5] = 1;
        r.m[8] = -s; r.m[10] = c;
        r.m[15] = 1;
        return r;
    }

    Mat4 get_transform_matrix() const
    {
        Mat4 T = translate(position);
        Mat4 R = rotateY(rotation.y);
        Mat4 S = scale_matrix(scale);

        Mat4 local = T * R * S;

        if (parent)
            return parent->get_transform_matrix() * local;
        return local;
    }
};

#endif