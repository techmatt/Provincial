/*
Vec2i.inl
Written by Matthew Fisher

Inline file for a 2-dimensional vector of ints
*/

#pragma once

Vec2i::Vec2i()
{

}

Vec2i::Vec2i(int _x, int _y)
{
    x = _x;
    y = _y;
}

Vec2i::Vec2i(UINT _x, UINT _y)
{
    x = int(_x);
    y = int(_y);
}

Vec2i::Vec2i(int _x, UINT _y)
{
    x = _x;
    y = int(_y);
}

Vec2i::Vec2i(UINT _x, int _y)
{
    x = int(_x);
    y = _y;
}

Vec2i::Vec2i(const Vec2i &V)
{
    x = V.x;
    y = V.y;
}

__forceinline Vec2i& Vec2i::operator = (const Vec2i &V)
{
    x = V.x;
    y = V.y;
    return *this;
}

__forceinline Vec2i& Vec2i::operator *= (int Right)
{
    x *= Right;
    y *= Right;
    return *this;
}

__forceinline Vec2i& Vec2i::operator /= (int Right)
{
    x /= Right;
    y /= Right;
    return *this;
}

__forceinline Vec2i& Vec2i::operator += (const Vec2i &Right)
{
    x += Right.x;
    y += Right.y;
    return *this;
}

__forceinline Vec2i& Vec2i::operator -= (const Vec2i &Right)
{
    x -= Right.x;
    y -= Right.y;
    return *this;
}

__forceinline float Vec2i::Length() const
{
    return sqrtf(float(x) * float(x) + float(y) * float(y));
}

__forceinline float Vec2i::LengthSq() const
{
    return float(float(x) * float(x) + float(y) * float(y));
}

__forceinline int Vec2i::MaxElement() const
{
    return Math::Max(x, y);
}

__forceinline Vec2i Vec2i::Maximize(const Vec2i &Left, const Vec2i &Right)
{
    Vec2i Result = Right;
    if(Left.x > Right.x) Result.x = Left.x;
    if(Left.y > Right.y) Result.y = Left.y;
    return Result;
}

__forceinline Vec2i Vec2i::Minimize(const Vec2i &Left, const Vec2i &Right)
{
    Vec2i Result = Right;
    if(Left.x < Right.x) Result.x = Left.x;
    if(Left.y < Right.y) Result.y = Left.y;
    return Result;
}

__forceinline float Vec2i::Dist(const Vec2i &Left, const Vec2i &Right)
{
    float XDiff = float(Right.x - Left.x);
    float YDiff = float(Right.y - Left.y);
    return sqrtf(XDiff * XDiff + YDiff * YDiff);
}

__forceinline float Vec2i::DistSq(const Vec2i &Left, const Vec2i &Right)
{
    float XDiff = float(Right.x - Left.x);
    float YDiff = float(Right.y - Left.y);
    return (XDiff * XDiff + YDiff * YDiff);
}

__forceinline Vec2i operator * (const Vec2i &Left, int Right)
{
    Vec2i Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    return Return;
}

__forceinline Vec2i operator * (int Right, const Vec2i &Left)
{
    Vec2i Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    return Return;
}

__forceinline Vec2i operator / (const Vec2i &Left, int Right)
{
    Vec2i Return;
    Return.x = Left.x / Right;
    Return.y = Left.y / Right;
    return Return;
}

__forceinline Vec2i operator + (const Vec2i &Left, const Vec2i &Right)
{
    Vec2i Return;
    Return.x = Left.x + Right.x;
    Return.y = Left.y + Right.y;
    return Return;
}

__forceinline Vec2i operator - (const Vec2i &Left, const Vec2i &Right)
{
    Vec2i Return;
    Return.x = Left.x - Right.x;
    Return.y = Left.y - Right.y;
    return Return;
}

__forceinline bool operator == (const Vec2i &Left, const Vec2i &Right)
{
    return (Left.x == Right.x && Left.y == Right.y);
}

__forceinline bool operator != (const Vec2i &Left, const Vec2i &Right)
{
    return (Left.x != Right.x || Left.y != Right.y);
}
