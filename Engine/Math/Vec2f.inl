/*
Vec2f.inl
Written by Matthew Fisher

Inline file for a 2-dimensional vector of floats
*/

#pragma once

Vec2f::Vec2f()
{

}

Vec2f::Vec2f(float _x, float _y)
{
    x = _x;
    y = _y;
}

Vec2f::Vec2f(const Vec2f &V)
{
    x = V.x;
    y = V.y;
}

Vec2f::Vec2f(const Vec2i &V)
{
    x = float(V.x);
    y = float(V.y);
}

__forceinline Vec2f& Vec2f::operator = (const Vec2f &V)
{
    x = V.x;
    y = V.y;
    return *this;
}

__forceinline Vec2f& Vec2f::operator *= (float Right)
{
    x *= Right;
    y *= Right;
    return *this;
}

__forceinline Vec2f& Vec2f::operator /= (float Right)
{
    x /= Right;
    y /= Right;
    return *this;
}

__forceinline Vec2f& Vec2f::operator += (const Vec2f &Right)
{
    x += Right.x;
    y += Right.y;
    return *this;
}

__forceinline Vec2f& Vec2f::operator -= (const Vec2f &Right)
{
    x -= Right.x;
    y -= Right.y;
    return *this;
}


__forceinline Vec2f Vec2f::StdRandomVector()
{
    return Vec2f(float(rand()) / RAND_MAX * 2.0f - 1.0f,
                float(rand()) / RAND_MAX * 2.0f - 1.0f);
}

__forceinline Vec2f Vec2f::StdRandomNormal()
{
    return Normalize(StdRandomVector());
}

__forceinline float Vec2f::Length() const
{
    return sqrtf(x * x + y * y);
}

__forceinline float Vec2f::LengthSq() const
{
    return x * x + y * y;
}

__forceinline Vec2i Vec2f::RoundToVec2i() const
{
    return Vec2i(Math::Round(x), Math::Round(y));
}

__forceinline Vec2f Vec2f::Normalize(const Vec2f &V)
{
    float Len = V.Length();
    if(Len == 0.0f)
    {
        return V;
    }
    else
    {
        float Factor = 1.0f / Len;
        return Vec2f(V.x * Factor, V.y * Factor);
    }
}

__forceinline Vec2f Vec2f::Maximize(const Vec2f &Left, const Vec2f &Right)
{
    Vec2f Result = Right;
    if(Left.x > Right.x) Result.x = Left.x;
    if(Left.y > Right.y) Result.y = Left.y;
    return Result;
}

__forceinline Vec2f Vec2f::Minimize(const Vec2f &Left, const Vec2f &Right)
{
    Vec2f Result = Right;
    if(Left.x < Right.x) Result.x = Left.x;
    if(Left.y < Right.y) Result.y = Left.y;
    return Result;
}

__forceinline Vec2f Vec2f::Lerp(const Vec2f &Left, const Vec2f &Right, float s)
{
    return (Left + s * (Right - Left));
}

__forceinline Vec2f Vec2f::ConstructFromPolar(float Radius, float Theta)
{
    return Vec2f(Radius * cosf(Theta), Radius * sinf(Theta));
}

__forceinline float Vec2f::Dot(const Vec2f &Left, const Vec2f &Right)
{
    return (Left.x * Right.x + Left.y * Right.y);
}

__forceinline float Vec2f::Cross(const Vec2f &Left, const Vec2f &Right)
{
    return (Left.x * Right.y - Left.y * Right.x);
}

__forceinline Vec2f Vec2f::Reflect(const Vec2f &Input, const Vec2f &ReflectAbout)
{
    Vec2f ReflectAboutNormalized = Vec2f::Normalize(ReflectAbout);
    Vec2f InputNormalized = Vec2f::Normalize(Input);
    return (2.0f * Vec2f::Dot(InputNormalized, ReflectAboutNormalized) * ReflectAboutNormalized - InputNormalized);
}

__forceinline float Vec2f::AngleBetween(const Vec2f &Left, const Vec2f &Right)
{
    float LeftLength = Left.Length();
    float RightLength = Right.Length();
    if(LeftLength > 0.0f && RightLength > 0.0f)
    {
        return acosf(Utility::Bound(Vec2f::Dot(Left, Right) / LeftLength / RightLength, -1.0f, 1.0f));
    }
    else
    {
        return 0.0f;
    }
}

__forceinline float Vec2f::Dist(const Vec2f &Left, const Vec2f &Right)
{
    return (Right - Left).Length();
}

__forceinline float Vec2f::DistSq(const Vec2f &Left, const Vec2f &Right)
{
    return (Right - Left).LengthSq();
}

__forceinline Vec2f operator * (const Vec2f &Left, float Right)
{
    Vec2f Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    return Return;
}

__forceinline Vec2f operator * (float Right, const Vec2f &Left)
{
    Vec2f Return;
    Return.x = Left.x * Right;
    Return.y = Left.y * Right;
    return Return;
}

__forceinline Vec2f operator / (const Vec2f &Left, float Right)
{
    Vec2f Return;
    Return.x = Left.x / Right;
    Return.y = Left.y / Right;
    return Return;
}

__forceinline Vec2f operator + (const Vec2f &Left, const Vec2f &Right)
{
    Vec2f Return;
    Return.x = Left.x + Right.x;
    Return.y = Left.y + Right.y;
    return Return;
}

__forceinline Vec2f operator - (const Vec2f &Left, const Vec2f &Right)
{
    Vec2f Return;
    Return.x = Left.x - Right.x;
    Return.y = Left.y - Right.y;
    return Return;
}

__forceinline Vec2f operator - (const Vec2f &V)
{
    return Vec2f(-V.x, -V.y);
}
