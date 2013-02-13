/*
Rectangle3f.inl
Written by Matthew Fisher

Inline file for a 3-dimensional, floating point rectangle
*/

#pragma once

Rectangle3f::Rectangle3f()
{

}

Rectangle3f::Rectangle3f(const Vec3f &_Min, const Vec3f &_Max)
{
    Min = _Min;
    Max = _Max;
}

Rectangle3f::Rectangle3f(const Rectangle3f &R)
{
    Min = R.Min;
    Max = R.Max;
}

__forceinline Rectangle3f Rectangle3f::ConstructFromCenterVariance(const Vec3f &Center, const Vec3f &Variance)
{
    return Rectangle3f(Center - Variance, Center + Variance);
}

__forceinline Rectangle3f Rectangle3f::ConstructFromTwoPoints(const Vec3f &P1, const Vec3f &P2)
{
    return Rectangle3f(Vec3f::Minimize(P1, P2), Vec3f::Maximize(P1, P2));
}

__forceinline Rectangle3f Rectangle3f::ConstructFromThreePoints(const Vec3f &P1, const Vec3f &P2, const Vec3f &P3)
{
    return Rectangle3f( Vec3f::Minimize(Vec3f::Minimize(P1, P2), P3), Vec3f::Maximize(Vec3f::Maximize(P1, P2), P3) );
}

__forceinline bool Rectangle3f::Intersects(const Rectangle3f &R0, const Rectangle3f &R1)
{
    Vec3f CenterDiff = R1.Center() - R0.Center();
    Vec3f TotalVariance = R0.Variance() + R1.Variance();
    return (fabsf(CenterDiff.x) <= TotalVariance.x &&
            fabsf(CenterDiff.y) <= TotalVariance.y &&
            fabsf(CenterDiff.z) <= TotalVariance.z);
}

Rectangle3f& Rectangle3f::operator = (const Rectangle3f &R)
{
    Min = R.Min;
    Max = R.Max;
    return *this;
}

__forceinline void Rectangle3f::ExpandBoundingBox(const Vec3f &V)
{
    Min = Vec3f::Minimize(Min, V);
    Max = Vec3f::Maximize(Max, V);
}

__forceinline Vec3f Rectangle3f::Center() const
{
    return (Max + Min) * 0.5f;
}

__forceinline Vec3f Rectangle3f::Variance() const
{
    return (Max - Min) * 0.5f;
}

__forceinline Vec3f Rectangle3f::Dimensions() const
{
    return (Max - Min);
}

__forceinline float Rectangle3f::Volume() const
{
    Vec3f Dimensions = Max - Min;
    return Dimensions.x * Dimensions.y * Dimensions.z;
}

__forceinline float Rectangle3f::SurfaceArea() const
{
    Vec3f Dimensions = Max - Min;
    return (Dimensions.x * Dimensions.y + Dimensions.x * Dimensions.z + Dimensions.y * Dimensions.z) * 2.0f;
}

__forceinline bool Rectangle3f::ContainsPt(const Vec3f &Pt) const
{
    return (Pt.x >= Min.x && Pt.x <= Max.x &&
            Pt.y >= Min.y && Pt.y <= Max.y &&
            Pt.z >= Min.z && Pt.z <= Max.z);
}

__forceinline bool Rectangle3f::PtWithinDistance(const Vec3f &Pt, float Threshold) const
{
    return (Pt.x + Threshold >= Min.x && Pt.x <= Max.x + Threshold &&
            Pt.y + Threshold >= Min.y && Pt.y <= Max.y + Threshold &&
            Pt.z + Threshold >= Min.z && Pt.z <= Max.z + Threshold);
}
