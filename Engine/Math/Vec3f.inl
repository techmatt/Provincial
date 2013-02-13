/*
Vec3f.inl
Written by Matthew Fisher

Inline file for a 3-dimensional vector of floats
*/

#pragma once

Vec3f::Vec3f()
{

}

Vec3f::Vec3f(const Vec2f &V, float _z)
{
    x = V.x;
    y = V.y;
    z = _z;
}

Vec3f::Vec3f(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}

Vec3f::Vec3f(const Vec3f &V)
{
    x = V.x;
    y = V.y;
    z = V.z;
}

Vec3f::Vec3f(RGBColor c)
{
    x = c.r / 255.0f;
    y = c.g / 255.0f;
    z = c.b / 255.0f;
}

__forceinline Vec3f& Vec3f::operator = (const Vec3f &V)
{
    x = V.x;
    y = V.y;
    z = V.z;
    return *this;
}

__forceinline Vec3f Vec3f::StdRandomVector()
{
    float x, y, z;
    x = float(rand()) / RAND_MAX * 2.0f - 1.0f;
    y = float(rand()) / RAND_MAX * 2.0f - 1.0f;
    z = float(rand()) / RAND_MAX * 2.0f - 1.0f;
    return Vec3f(x, y, z);
}

__forceinline Vec3f Vec3f::StdRandomNormal()
{
    return Normalize(StdRandomVector());
}

__forceinline float Vec3f::Length() const
{
    return sqrtf(x * x + y * y + z * z);
}

__forceinline float Vec3f::LengthSq() const
{
    return x * x + y * y + z * z;
}

__forceinline bool Vec3f::Valid() const
{
    return ((x == x) && (y == y) && (z == z));
}

__forceinline Vec3f Vec3f::Normalize(const Vec3f &V)
{
    float Len = V.Length();
    if(Len == 0.0f)
    {
        return V;
    }
    else
    {
        float Factor = 1.0f / Len;
        return Vec3f(V.x * Factor, V.y * Factor, V.z * Factor);
    }
}

__forceinline void Vec3f::SetLength(float NewLength)
{
    float Len = Length();
    if(Len != 0.0f)
    {
        float Factor = NewLength / Len;
        x *= Factor;
        y *= Factor;
        z *= Factor;
    }
}

#ifdef USE_D3D
__forceinline Vec3f::operator D3DXVECTOR3() const
{
    D3DXVECTOR3 V(x, y, z);
    return V;
}
#endif

__forceinline float Vec3f::AngleBetween(const Vec3f &Left, const Vec3f &Right)
{
    float LeftLength = Left.Length();
    float RightLength = Right.Length();
    if(LeftLength > 0.0f && RightLength > 0.0f)
    {
        return acosf(Utility::Bound(Vec3f::Dot(Left, Right) / LeftLength / RightLength, -1.0f, 1.0f));
    }
    else
    {
        return 0.0f;
    }
}

__forceinline Vec3f Vec3f::Cross(const Vec3f &Left, const Vec3f &Right)
{
    Vec3f Result;
    Result.x = Left.y * Right.z - Left.z * Right.y;
    Result.y = Left.z * Right.x - Left.x * Right.z;
    Result.z = Left.x * Right.y - Left.y * Right.x;
    return Result;
}

__forceinline float Vec3f::Dot(const Vec3f &Left, const Vec3f &Right)
{
    return (Left.x * Right.x + Left.y * Right.y + Left.z * Right.z);
}

__forceinline Vec3f Vec3f::DirectProduct(const Vec3f &Left, const Vec3f &Right)
{
    return Vec3f(Left.x * Right.x, Left.y * Right.y, Left.z * Right.z);
}

__forceinline Vec3f Vec3f::Lerp(const Vec3f &Left, const Vec3f &Right, float s)
{
    return (Left + s * (Right - Left));
}

__forceinline Vec3f Vec3f::Maximize(const Vec3f &Left, const Vec3f &Right)
{
    Vec3f Result = Right;
    if(Left.x > Right.x) Result.x = Left.x;
    if(Left.y > Right.y) Result.y = Left.y;
    if(Left.z > Right.z) Result.z = Left.z;
    return Result;
}

__forceinline Vec3f Vec3f::Minimize(const Vec3f &Left, const Vec3f &Right)
{
    Vec3f Result = Right;
    if(Left.x < Right.x) Result.x = Left.x;
    if(Left.y < Right.y) Result.y = Left.y;
    if(Left.z < Right.z) Result.z = Left.z;
    return Result;
}

__forceinline Vec3f Vec3f::SphericalFromCartesian(const Vec3f &Cartesian)
{
    Vec3f Result;
    Result.x = Cartesian.Length();
    Result.y = atan2f(Cartesian.y, Cartesian.x);
    if(Result.x == 0.0f)
    {
        Result.z = 0.0f;
    }
    else
    {
        Result.z = acosf(Cartesian.z / Result.x);
    }
    return Result;
}

__forceinline Vec3f Vec3f::CartesianFromSpherical(const Vec3f &Spherical)
{
    const float &r = Spherical.x;
    const float &Theta = Spherical.y;
    const float &Phi = Spherical.z;
    float RSinPhi = r * sinf(Phi);
    return Vec3f(cosf(Theta) * RSinPhi, sinf(Theta) * RSinPhi, r * cosf(Phi));
}

__forceinline bool Vec3f::WithinRect(const Vec3f &Pt, const Rectangle3f &Rect)
{
    return((Pt.x >= Rect.Min.x && Pt.x <= Rect.Max.x) &&
           (Pt.y >= Rect.Min.y && Pt.y <= Rect.Max.y) &&
           (Pt.z >= Rect.Min.z && Pt.z <= Rect.Max.z));
}

__forceinline Vec3f Vec3f::LinearMap(const Vec3f &s1, const Vec3f &e1, const Vec3f &s2, const Vec3f &e2, const Vec3f &s)
{
    return Vec3f(float(Math::LinearMap(s1.x, e1.x, s2.x, e2.x, s.x)),
                float(Math::LinearMap(s1.y, e1.y, s2.y, e2.y, s.y)),
                float(Math::LinearMap(s1.z, e1.z, s2.z, e2.z, s.z)));
}

__forceinline void Vec3f::CompleteOrthonormalBasis(const Vec3f &Normal, Vec3f &v1, Vec3f &v2)
{
    Vec3f AxisTest1 = Cross(Normal, Vec3f::eX);
    Vec3f AxisTest2 = Cross(Normal, Vec3f::eY);
    if(AxisTest1.Length() >= AxisTest2.Length())
    {
        v1 = Normalize(AxisTest1);
    }
    else
    {
        v1 = Normalize(AxisTest2);
    }
    v2 = Normalize(Cross(v1, Normal));
    Math::ReorientBasis(v1, v2, Normal);
}

__forceinline Vec3f& Vec3f::operator *= (float Right)
{
    x *= Right;
    y *= Right;
    z *= Right;
    return *this;
}

__forceinline Vec3f& Vec3f::operator /= (float Right)
{
    x /= Right;
    y /= Right;
    z /= Right;
    return *this;
}

__forceinline Vec3f& Vec3f::operator *= (UINT Right)
{
    x *= Right;
    y *= Right;
    z *= Right;
    return *this;
}

__forceinline Vec3f& Vec3f::operator /= (UINT Right)
{
    x /= Right;
    y /= Right;
    z /= Right;
    return *this;
}

__forceinline Vec3f& Vec3f::operator += (const Vec3f &Right)
{
    x += Right.x;
    y += Right.y;
    z += Right.z;
    return *this;
}

__forceinline Vec3f& Vec3f::operator -= (const Vec3f &Right)
{
    x -= Right.x;
    y -= Right.y;
    z -= Right.z;
    return *this;
}

__forceinline Vec3f operator * (const Vec3f &Left, float Right)
{
    return Vec3f(Left.x * Right,
                Left.y * Right,
                Left.z * Right);
}

__forceinline Vec3f operator * (float Left, const Vec3f &Right)
{
    return Vec3f(Right.x * Left,
                Right.y * Left,
                Right.z * Left);
}

__forceinline Vec3f operator / (const Vec3f &Left, float Right)
{
    return Vec3f(Left.x / Right,
                Left.y / Right,
                Left.z / Right);
}

__forceinline Vec3f operator + (const Vec3f &Left, const Vec3f &Right)
{
    return Vec3f(Left.x + Right.x,
                Left.y + Right.y,
                Left.z + Right.z);
}

__forceinline Vec3f operator - (const Vec3f &Left, const Vec3f &Right)
{
    return Vec3f(Left.x - Right.x,
                Left.y - Right.y,
                Left.z - Right.z);
}

__forceinline Vec3f operator - (const Vec3f &V)
{
    return Vec3f(-V.x, -V.y, -V.z);
}

__forceinline float Vec3f::Dist(const Vec3f &Left, const Vec3f &Right)
{
    const float XDiff = Right.x - Left.x;
    const float YDiff = Right.y - Left.y;
    const float ZDiff = Right.z - Left.z;
    return sqrtf(XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff);
}

__forceinline float Vec3f::DistSq(const Vec3f &Left, const Vec3f &Right)
{
    const float XDiff = Right.x - Left.x;
    const float YDiff = Right.y - Left.y;
    const float ZDiff = Right.z - Left.z;
    return (XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff);
}
