#include "Tranform.hpp"
#include <cmath>

Transform::Mat4 Transform::translate(const Vec3& v)
{
    Mat4 t;
    t.m[12] = v.x; t.m[13] = v.y; t.m[14] = v.z;
    return t;
}

Transform::Mat4 Transform::scale_matrix(const Vec3& v)
{
    Mat4 s;
    s.m[0] = v.x; s.m[5] = v.y; s.m[10] = v.z;
    return s;
}

Transform::Mat4 Transform::rotateY(float angle)
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

Transform::Mat4 Transform::get_transform_matrix() const
{
    Mat4 T = translate(position);
    Mat4 R = rotateY(rotation.y);
    Mat4 S = scale_matrix(scale);

    Mat4 local = T * R * S;

    if (parent)
        return parent->get_transform_matrix() * local;

    return local;
}