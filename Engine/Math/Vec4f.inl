/*
Vec4f.inl
Written by Matthew Fisher

Inline file for a 4-dimensional vector of floats
*/

#pragma once

Vec4f::Vec4f()
{

}

Vec4f::Vec4f(float _x, float _y, float _z, float _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

Vec4f::Vec4f(const Vec3f &V, float _w)
{
    x = V.x;
    y = V.y;
    z = V.z;
    w = _w;
}

Vec4f::Vec4f(const Vec2f &V0, const Vec2f &V1)
{
    x = V0.x;
    y = V0.y;
    z = V1.x;
    w = V1.y;
}

Vec4f::Vec4f(const Vec4f &V)
{
    x = V.x;
    y = V.y;
    z = V.z;
    w = V.w;
}

Vec4f::Vec4f(const RGBColor &c)
{
    x = c.r / 255.0f;
    y = c.g / 255.0f;
    z = c.b / 255.0f;
    w = c.a / 255.0f;
}

Vec4f& Vec4f::operator = (const Vec4f &V)
{
    x = V.x;
    y = V.y;
    z = V.z;
    w = V.w;
    return *this;
}

float Vec4f::Length() const
{
    return sqrtf(x * x + y * y + z * z + w * w);
}

float Vec4f::LengthSq() const
{
    return x * x + y * y + z * z + w * w;
}

__forceinline Vec4f Vec4f::Normalize(const Vec4f &V)
{
    float Len = V.Length();
    if(Len == 0.0f)
    {
        return V;
    }
    else
    {
        float Factor = 1.0f / Len;
        return Vec4f(V.x * Factor, V.y * Factor, V.z * Factor, V.w * Factor);
    }
}


float Vec4f::Dot(const Vec4f &Left, const Vec4f &Right)
{
    return (Left.x * Right.x + Left.y * Right.y + Left.z * Right.z + Left.w * Right.w);
}

Vec4f Vec4f::Lerp(const Vec4f &Left, const Vec4f &Right, float s)
{
    return (Left + s * (Right - Left));
}

Vec4f Vec4f::Maximize(const Vec4f &Left, const Vec4f &Right)
{
    Vec4f Result = Right;
    if(Left.x > Right.x) Result.x = Left.x;
    if(Left.y > Right.y) Result.y = Left.y;
    if(Left.z > Right.z) Result.z = Left.z;
    if(Left.w > Right.w) Result.w = Left.w;
    return Result;
}

Vec4f Vec4f::Minimize(const Vec4f &Left, const Vec4f &Right)
{
    Vec4f Result = Right;
    if(Left.x < Right.x) Result.x = Left.x;
    if(Left.y < Right.y) Result.y = Left.y;
    if(Left.z < Right.z) Result.z = Left.z;
    if(Left.w < Right.w) Result.w = Left.w;
    return Result;
}

Vec4f Vec4f::Abs(const Vec4f &V)
{
    Vec4f Result = V;
    if(Result.x < 0.0f)
    {
        Result.x = -Result.x;
    }
    if(Result.y < 0.0f)
    {
        Result.y = -Result.y;
    }
    if(Result.z < 0.0f)
    {
        Result.z = -Result.z;
    }
    if(Result.w < 0.0f)
    {
        Result.w = -Result.w;
    }
    return Result;
}

Vec4f operator * (const Vec4f &Left, float Right)
{
    Vec4f Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    Return.z = Left.z * Right;
    Return.w = Left.w * Right;
    return Return;
}

Vec4f operator * (float Right, const Vec4f &Left)
{
    Vec4f Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    Return.z = Left.z * Right;
    Return.w = Left.w * Right;
    return Return;
}

Vec4f operator / (const Vec4f &Left, float Right)
{
    Vec4f Return;
    Return.x = Left.x / Right;
    Return.y = Left.y / Right;
    Return.z = Left.z / Right;
    Return.w = Left.w / Right;
    return Return;
}

Vec4f operator + (const Vec4f &Left, const Vec4f &Right)
{
    Vec4f Return;
    Return.x = Left.x + Right.x;
    Return.y = Left.y + Right.y;
    Return.z = Left.z + Right.z;
    Return.w = Left.w + Right.w;
    return Return;
}

Vec4f operator - (const Vec4f &Left, const Vec4f &Right)
{
    Vec4f Return;
    Return.x = Left.x - Right.x;
    Return.y = Left.y - Right.y;
    Return.z = Left.z - Right.z;
    Return.w = Left.w - Right.w;
    return Return;
}

Vec4f& Vec4f::operator *= (float Right)
{
    x *= Right;
    y *= Right;
    z *= Right;
    w *= Right;
    return *this;
}

Vec4f& Vec4f::operator /= (float Right)
{
    x /= Right;
    y /= Right;
    z /= Right;
    w /= Right;
    return *this;
}

Vec4f& Vec4f::operator += (const Vec4f &Right)
{
    x += Right.x;
    y += Right.y;
    z += Right.z;
    w += Right.w;
    return *this;
}

Vec4f& Vec4f::operator -= (const Vec4f &Right)
{
    x -= Right.x;
    y -= Right.y;
    z -= Right.z;
    w -= Right.w;
    return *this;
}

__forceinline Vec4f operator - (const Vec4f &V)
{
    Vec4f Result;
    Result.x = -V.x;
    Result.y = -V.y;
    Result.z = -V.z;
    Result.w = -V.w;
    return Result;
}
