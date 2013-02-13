/*
Rectangle2f.inl
Written by Matthew Fisher

Inline file for a 2-dimensional, floating point rectangle
*/

#pragma once

Rectangle2f::Rectangle2f()
{

}

Rectangle2f::Rectangle2f(const Vec2f &_Min, const Vec2f &_Max)
{
    Min = _Min;
    Max = _Max;
}

Rectangle2f::Rectangle2f(const Rectangle2f &R)
{
    Min = R.Min;
    Max = R.Max;
}

Rectangle2f::Rectangle2f(const RECT &Rect)
{
    Min.x = float(Rect.left);
    Max.x = float(Rect.right);
    Min.y = float(Rect.top);
    Max.y = float(Rect.bottom);
}

__forceinline RECT Rectangle2f::ToRect() const
{
    RECT Result;
    Result.left = LONG(Min.x);
    Result.right = LONG(Max.x);
    Result.top = LONG(Min.y);
    Result.bottom = LONG(Max.y);
    return Result;
}

__forceinline void Rectangle2f::ExpandBoundingBox(const Vec2f &V)
{
    Min = Vec2f::Minimize(Min, V);
    Max = Vec2f::Maximize(Max, V);
}

_forceinline Rectangle2i Rectangle2f::RoundToRectangle2i() const
{
    return Rectangle2i(Min.RoundToVec2i(), Max.RoundToVec2i());
}

__forceinline Rectangle2f Rectangle2f::ConstructFromCenterVariance(const Vec2f &Center, const Vec2f &Variance)
{
    return Rectangle2f(Center - Variance, Center + Variance);
}

__forceinline Rectangle2f Rectangle2f::ConstructFromTwoPoints(const Vec2f &P1, const Vec2f &P2)
{
    return Rectangle2f(Vec2f::Minimize(P1, P2), Vec2f::Maximize(P1, P2));
}

Rectangle2f& Rectangle2f::operator = (const Rectangle2f &R)
{
    Min = R.Min;
    Max = R.Max;
    return *this;
}

__forceinline Vec2f Rectangle2f::Center() const
{
    return (Max + Min) * 0.5f;
}

__forceinline Vec2f Rectangle2f::Variance() const
{
    return (Max - Min) * 0.5f;
}

__forceinline Vec2f Rectangle2f::Dimensions() const
{
    return (Max - Min);
}

__forceinline float Rectangle2f::Width() const
{
    return (Max.x - Min.x);
}

__forceinline float Rectangle2f::Height() const
{
    return (Max.y - Min.y);
}

__forceinline void Rectangle2f::Expand(float Value)
{
    Min -= Vec2f(Value, Value);
    Max += Vec2f(Value, Value);
}

__forceinline bool Rectangle2f::ContainsPt(const Vec2f &Pt) const
{
    return (Pt.x >= Min.x && Pt.x <= Max.x &&
            Pt.y >= Min.y && Pt.y <= Max.y);
}
