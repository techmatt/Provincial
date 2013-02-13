/*
Vec3i.inl
Written by Matthew Fisher

Inline file for a 2-dimensional vector of ints
*/

#pragma once

Vec3i::Vec3i()
{

}

Vec3i::Vec3i(int _x, int _y, int _z)
{
    x = _x;
    y = _y;
    z = _z;
}

Vec3i::Vec3i(UINT _x, UINT _y, UINT _z)
{
    x = int(_x);
    y = int(_y);
    z = int(_z);
}

Vec3i::Vec3i(RGBColor c)
{
    x = c.r;
    y = c.g;
    z = c.b;
}

__forceinline Vec3i& Vec3i::operator *= (int Right)
{
    x *= Right;
    y *= Right;
    z *= Right;
    return *this;
}

__forceinline Vec3i& Vec3i::operator /= (int Right)
{
    x /= Right;
    y /= Right;
    z /= Right;
    return *this;
}

__forceinline Vec3i& Vec3i::operator += (const Vec3i &Right)
{
    x += Right.x;
    y += Right.y;
    z += Right.z;
    return *this;
}

__forceinline Vec3i& Vec3i::operator -= (const Vec3i &Right)
{
    x -= Right.x;
    y -= Right.y;
    z -= Right.z;
    return *this;
}

__forceinline float Vec3i::Length() const
{
    return sqrtf(float(x) * float(x) + float(y) * float(y) + float(z) * float(z));
}

__forceinline float Vec3i::LengthSq() const
{
    return float(float(x) * float(x) + float(y) * float(y) + float(z) * float(z));
}

__forceinline int Vec3i::MaxElement() const
{
    return Math::Max(x, y, z);
}

__forceinline Vec3i Vec3i::Maximize(const Vec3i &Left, const Vec3i &Right)
{
    Vec3i Result = Right;
    if(Left.x > Right.x) Result.x = Left.x;
    if(Left.y > Right.y) Result.y = Left.y;
    if(Left.z > Right.z) Result.z = Left.z;
    return Result;
}

__forceinline Vec3i Vec3i::Minimize(const Vec3i &Left, const Vec3i &Right)
{
    Vec3i Result = Right;
    if(Left.x < Right.x) Result.x = Left.x;
    if(Left.y < Right.y) Result.y = Left.y;
    if(Left.z < Right.z) Result.z = Left.z;
    return Result;
}

__forceinline float Vec3i::Dist(const Vec3i &Left, const Vec3i &Right)
{
    float XDiff = float(Right.x - Left.x);
    float YDiff = float(Right.y - Left.y);
    float ZDiff = float(Right.z - Left.z);
    return sqrtf(XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff);
}

__forceinline float Vec3i::DistSq(const Vec3i &Left, const Vec3i &Right)
{
    float XDiff = float(Right.x - Left.x);
    float YDiff = float(Right.y - Left.y);
    float ZDiff = float(Right.z - Left.z);
    return (XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff);
}

__forceinline Vec3i operator * (const Vec3i &Left, int Right)
{
    Vec3i Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    Return.z = Left.z * Right;
    return Return;
}

__forceinline Vec3i operator * (int Right, const Vec3i &Left)
{
    Vec3i Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    Return.z = Left.z * Right;
    return Return;
}

__forceinline Vec3i operator / (const Vec3i &Left, int Right)
{
    Vec3i Return;
    Return.x = Left.x / Right;
    Return.y = Left.y / Right;
    Return.z = Left.z / Right;
    return Return;
}

__forceinline Vec3i operator + (const Vec3i &Left, const Vec3i &Right)
{
    Vec3i Return;
    Return.x = Left.x + Right.x;
    Return.y = Left.y + Right.y;
    Return.z = Left.z + Right.z;
    return Return;
}

__forceinline Vec3i operator - (const Vec3i &Left, const Vec3i &Right)
{
    Vec3i Return;
    Return.x = Left.x - Right.x;
    Return.y = Left.y - Right.y;
    Return.z = Left.z - Right.z;
    return Return;
}

__forceinline bool operator == (const Vec3i &Left, const Vec3i &Right)
{
    return (Left.x == Right.x && Left.y == Right.y && Left.z == Right.z);
}

__forceinline bool operator != (const Vec3i &Left, const Vec3i &Right)
{
    return (Left.x != Right.x || Left.y != Right.y || Left.z != Right.z);
}
