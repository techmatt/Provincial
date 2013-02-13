/*
Rectangle2i.inl
Written by Matthew Fisher

Inline file for a 2-dimensional, floating point rectangle
*/

#pragma once

Rectangle2i::Rectangle2i()
{

}

Rectangle2i::Rectangle2i(const Vec2i &_Min, const Vec2i &_Max)
{
    Min = _Min;
    Max = _Max;
}

Rectangle2i::Rectangle2i(int MinX, int MinY, int MaxX, int MaxY)
{
    Min.x = MinX;
    Min.y = MinY;
    Max.x = MaxX;
    Max.y = MaxY;
}

Rectangle2i::Rectangle2i(const Rectangle2i &R)
{
    Min = R.Min;
    Max = R.Max;
}

Rectangle2i::Rectangle2i(const RECT &Rect)
{
    Min.x = int(Rect.left);
    Max.x = int(Rect.right);
    Min.y = int(Rect.top);
    Max.y = int(Rect.bottom);
}

__forceinline RECT Rectangle2i::ToRect() const
{
    RECT Result;
    Result.left = LONG(Min.x);
    Result.right = LONG(Max.x);
    Result.top = LONG(Min.y);
    Result.bottom = LONG(Max.y);
    return Result;
}

__forceinline Rectangle2i Rectangle2i::ConstructFromCenterVariance(const Vec2i &Center, const Vec2i &Variance)
{
    return Rectangle2i(Center - Variance, Center + Variance);
}

__forceinline Rectangle2i Rectangle2i::ConstructFromTwoPoints(const Vec2i &P1, const Vec2i &P2)
{
    return Rectangle2i(Vec2i::Minimize(P1, P2), Vec2i::Maximize(P1, P2));
}

__forceinline Rectangle2i Rectangle2i::ConstructFromMinDimensions(const Vec2i &_Min, const Vec2i &Dimensions)
{
    return Rectangle2i(_Min, _Min + Dimensions);
}

Rectangle2i& Rectangle2i::operator = (const Rectangle2i &R)
{
    Min = R.Min;
    Max = R.Max;
    return *this;
}

__forceinline Vec2i Rectangle2i::Center() const
{
    return (Max + Min) / 2;
}

__forceinline Vec2i Rectangle2i::Variance() const
{
    return (Max - Min) / 2;
}

__forceinline Vec2i Rectangle2i::Dimensions() const
{
    return (Max - Min);
}

__forceinline UINT Rectangle2i::Width() const
{
    return (Max.x - Min.x);
}

__forceinline UINT Rectangle2i::Height() const
{
    return (Max.y - Min.y);
}

__forceinline void Rectangle2i::Expand(int Value)
{
    Min -= Vec2i(Value, Value);
    Max += Vec2i(Value, Value);
}

__forceinline void Rectangle2i::Expand(const Vec2i &Pt)
{
    Min = Vec2i::Minimize(Min, Pt);
    Max = Vec2i::Maximize(Max, Pt);
}

__forceinline bool Rectangle2i::ContainsPt(const Vec2i &Pt) const
{
    return (Pt.x >= Min.x && Pt.x <= Max.x &&
            Pt.y >= Min.y && Pt.y <= Max.y);
}

__forceinline bool operator == (const Rectangle2i &Left, const Rectangle2i &Right)
{
    return (Left.Min.x == Right.Min.x &&
            Left.Min.y == Right.Min.y &&
            Left.Max.x == Right.Max.x &&
            Left.Max.y == Right.Max.y);
}

__forceinline bool operator != (const Rectangle2i &Left, const Rectangle2i &Right)
{
    return (Left.Min.x != Right.Min.x ||
            Left.Min.y != Right.Min.y ||
            Left.Max.x != Right.Max.x ||
            Left.Max.y != Right.Max.y);
}
