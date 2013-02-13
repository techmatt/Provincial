/*
SpaceVector.h
Written by Matthew Fisher

The Vec2f struct represents a 2-dimensional (x, y) real ordered triplet.
The Vec3f struct represents a 3-dimensional (x, y, z) real ordered triplet.
The Vec4f struct represents a 4-dimensional (x, y, z, w) real ordered triplet.
The Rectangle2f struct represents two ordered Vec2f's.
The Rectangle3f struct represents two ordered Vec3f's.

The "i" equivalents of the above classes represent integer versions.

Most of the Vec3f and Vec4f functions and overloaded operators are self-explanitory and are not commented.
*/

#pragma once

struct Vec2f;
struct Vec2i;
struct Vec3f;
struct Vec3i;
struct Vec4f;
struct Rectangle2f;
struct Rectangle2i;
struct Rectangle3f;

struct Vec2f
{
    //
    // Initalization
    //
    __forceinline Vec2f();
    __forceinline Vec2f(const Vec2f &V);
    __forceinline Vec2f(float _x, float _y);
    __forceinline explicit Vec2f(const Vec2i &V);

    //
    // Assignment
    //
    __forceinline Vec2f& operator = (const Vec2f &V);

    //
    // Overloaded operators
    //
    __forceinline Vec2f& operator *= (float Right);
    __forceinline Vec2f& operator /= (float Right);
    __forceinline Vec2f& operator += (const Vec2f &Right);
    __forceinline Vec2f& operator -= (const Vec2f &Right);

    //
    // Accessors
    //
    __forceinline float Length() const;
    __forceinline float LengthSq() const;

    //
    // Conversion
    //
    __forceinline Vec2i RoundToVec2i() const;

    //
    // String formatting
    //
    String TabSeparatedString() const;
    String CommaSeparatedString() const;

    //
    // Local data
    //
    float x, y;

    //
    // Constants
    //
    static const Vec2f Origin;
    static const Vec2f eX;
    static const Vec2f eY;

    //
    // Static functions
    //
    __forceinline static Vec2f Normalize(const Vec2f &V);
    __forceinline static Vec2f StdRandomVector();
    __forceinline static Vec2f StdRandomNormal();
    __forceinline static Vec2f Maximize(const Vec2f &Left, const Vec2f &Right);
    __forceinline static Vec2f Minimize(const Vec2f &Left, const Vec2f &Right);
    __forceinline static Vec2f Lerp(const Vec2f &Left, const Vec2f &Right, float s);
    __forceinline static float Dist(const Vec2f &Left, const Vec2f &Right);
    __forceinline static float DistSq(const Vec2f &Left, const Vec2f &Right);
    __forceinline static Vec2f ConstructFromPolar(float Radius, float Theta);
    __forceinline static Vec2f Reflect(const Vec2f &Input, const Vec2f &ReflectAbout);
    __forceinline static float Dot(const Vec2f &Left, const Vec2f &Right);
    __forceinline static float Cross(const Vec2f &Left, const Vec2f &Right);
    __forceinline static float AngleBetween(const Vec2f &Left, const Vec2f &Right);
};

struct Vec2i
{
    struct Comparison
    {
        bool operator()(const Vec2i &a, const Vec2i &b) const
        {
            if(a.x == b.x) return (a.y < b.y);
            return (a.x < b.x);
        }
        static bool Compare(const Vec2i &a, const Vec2i &b)
        {
            if(a.x == b.x) return (a.y < b.y);
            return (a.x < b.x);
        }
    };

    //
    // Initalization
    //
    __forceinline Vec2i();
    __forceinline Vec2i(const Vec2i &V);
    __forceinline Vec2i(int _x, int _y);
    __forceinline Vec2i(UINT _x, UINT _y);
    __forceinline Vec2i(int _x, UINT _y);
    __forceinline Vec2i(UINT _x, int _y);

    //
    // Assignment
    //
    __forceinline Vec2i& operator = (const Vec2i &V);

    //
    // Overloaded operators
    //
    __forceinline Vec2i& operator *= (int Right);
    __forceinline Vec2i& operator /= (int Right);
    __forceinline Vec2i& operator += (const Vec2i &Right);
    __forceinline Vec2i& operator -= (const Vec2i &Right);

    //
    // Accessors
    //
    __forceinline float Length() const;
    __forceinline float LengthSq() const;
    __forceinline int MaxElement() const;

    //
    // String formatting
    //
    String TabSeparatedString() const;
    String CommaSeparatedString() const;

    //
    // Local data
    //
    int x, y;

    //
    // Constants
    //
    static const Vec2i Origin;
    static const Vec2i eX;
    static const Vec2i eY;

    //
    // Static functions
    //
    __forceinline static Vec2i Maximize(const Vec2i &Left, const Vec2i &Right);
    __forceinline static Vec2i Minimize(const Vec2i &Left, const Vec2i &Right);
    __forceinline static float Dist(const Vec2i &Left, const Vec2i &Right);
    __forceinline static float DistSq(const Vec2i &Left, const Vec2i &Right);
};

struct Vec3i
{
    //
    // Initalization
    //
    __forceinline Vec3i();
    __forceinline Vec3i(int _x, int _y, int _z);
    __forceinline Vec3i(UINT _x, UINT _y, UINT _z);
    __forceinline explicit Vec3i(RGBColor c);

    //
    // Overloaded operators
    //
    __forceinline Vec3i& operator *= (int Right);
    __forceinline Vec3i& operator /= (int Right);
    __forceinline Vec3i& operator += (const Vec3i &Right);
    __forceinline Vec3i& operator -= (const Vec3i &Right);

    //
    // Accessors
    //
    __forceinline float Length() const;
    __forceinline float LengthSq() const;
    __forceinline int MaxElement() const;

    //
    // String formatting
    //
    String TabSeparatedString() const;
    String CommaSeparatedString() const;

    //
    // Local data
    //
    int x, y, z;

    //
    // Constants
    //
    static const Vec3i Origin;
    static const Vec3i eX;
    static const Vec3i eY;
    static const Vec3i eZ;

    //
    // Static functions
    //
    __forceinline static Vec3i Maximize(const Vec3i &Left, const Vec3i &Right);
    __forceinline static Vec3i Minimize(const Vec3i &Left, const Vec3i &Right);
    __forceinline static float Dist(const Vec3i &Left, const Vec3i &Right);
    __forceinline static float DistSq(const Vec3i &Left, const Vec3i &Right);
};

struct Vec3f
{
    //
    // Initalization
    //
    __forceinline Vec3f();
    __forceinline Vec3f(const Vec3f &V);
    __forceinline Vec3f(const Vec2f &V, float _z);
    __forceinline Vec3f(float _x, float _y, float _z);
    __forceinline explicit Vec3f(RGBColor c);

    //
    // Assignment
    //
    __forceinline Vec3f& operator = (const Vec3f &V);

    //
    // Overloaded operators
    //
    __forceinline Vec3f& operator *= (float Right);
    __forceinline Vec3f& operator *= (UINT Right);
    __forceinline Vec3f& operator /= (float Right);
    __forceinline Vec3f& operator /= (UINT Right);
    __forceinline Vec3f& operator += (const Vec3f &Right);
    __forceinline Vec3f& operator -= (const Vec3f &Right);

    //
    // Normalization
    //
    __forceinline void SetLength(float NewLength);

    //
    // Accessors
    //
    __forceinline float Length() const;
    __forceinline float LengthSq() const;

    __forceinline bool Valid() const;

#ifdef USE_D3D
    __forceinline operator D3DXVECTOR3() const;
#endif

    __forceinline float& operator[](UINT Index)
    {
        return ((float *)this)[Index];
    }
    __forceinline float operator[](UINT Index) const
    {
        return ((float *)this)[Index];
    }

    //
    // Formatting
    //
    String TabSeparatedString() const;
    String CommaSeparatedString() const;
    String ToString(char seperator) const;

    //
    // Local data
    //
    float x, y, z;

    //
    // Constants
    //
    static const Vec3f Origin;
    static const Vec3f eX;
    static const Vec3f eY;
    static const Vec3f eZ;

    //
    // Static functions
    //
    __forceinline static Vec3f Normalize(const Vec3f &V);
    __forceinline static Vec3f StdRandomVector();
    __forceinline static Vec3f StdRandomNormal();
    __forceinline static float AngleBetween(const Vec3f &Left, const Vec3f &Right);
    __forceinline static Vec3f Cross(const Vec3f &Left, const Vec3f &Right);
    __forceinline static Vec3f DirectProduct(const Vec3f &Left, const Vec3f &Right);
    __forceinline static float Dot(const Vec3f &Left, const Vec3f &Right);
    __forceinline static float Dist(const Vec3f &Left, const Vec3f &Right);
    __forceinline static float DistSq(const Vec3f &Left, const Vec3f &Right);
    __forceinline static Vec3f Lerp(const Vec3f &Left, const Vec3f &Right, float s);
    __forceinline static Vec3f Maximize(const Vec3f &Left, const Vec3f &Right);
    __forceinline static Vec3f Minimize(const Vec3f &Left, const Vec3f &Right);
    __forceinline static bool WithinRect(const Vec3f &Pt, const Rectangle3f &Rect);
    __forceinline static Vec3f SphericalFromCartesian(const Vec3f &Cartesian);
    __forceinline static Vec3f CartesianFromSpherical(const Vec3f &Spherical);
    __forceinline static Vec3f LinearMap(const Vec3f &s1, const Vec3f &e1, const Vec3f &s2, const Vec3f &e2, const Vec3f &s);
    __forceinline static void CompleteOrthonormalBasis(const Vec3f &Normal, Vec3f &v1, Vec3f &v2);
};

struct Vec4f
{
    //
    // Initalization
    //
    __forceinline Vec4f();
    __forceinline Vec4f(const Vec4f &V);
    __forceinline Vec4f(const Vec3f &V, float _w);
    __forceinline Vec4f(const Vec2f &V0, const Vec2f &V1);
    __forceinline Vec4f(float _x, float _y, float _z, float _w);
    __forceinline explicit Vec4f(const RGBColor &c);

    //
    // Assignment
    //
    __forceinline Vec4f& operator = (const Vec4f &V);

    //
    // Overloaded operators
    //
    __forceinline Vec4f& operator *= (float Right);
    __forceinline Vec4f& operator /= (float Right);
    __forceinline Vec4f& operator += (const Vec4f &Right);
    __forceinline Vec4f& operator -= (const Vec4f &Right);

    //
    // Accessors
    //
    __forceinline float Length() const;
    __forceinline float LengthSq() const;

    __forceinline float& Element(UINT Index)
    {
        return ((float *)this)[Index];
    }
    __forceinline float Element(UINT Index) const
    {
        return ((float *)this)[Index];
    }

    String TabSeparatedString() const;
    String CommaSeparatedString() const;

    __forceinline float& operator[](UINT Index)
    {
        return ((float *)this)[Index];
    }
    __forceinline float operator[](UINT Index) const
    {
        return ((float *)this)[Index];
    }

    //
    // Local data
    //
    float x, y, z, w;

    //
    // Constants
    //
    static const Vec4f Origin;
    static const Vec4f eX;
    static const Vec4f eY;
    static const Vec4f eZ;
    static const Vec4f eW;

    //
    // Static functions
    //
    __forceinline static Vec4f Normalize(const Vec4f &V);
    __forceinline static float Dot(const Vec4f &Left, const Vec4f &Right);
    __forceinline static Vec4f Lerp(const Vec4f &Left, const Vec4f &Right, float s);
    __forceinline static Vec4f Maximize(const Vec4f &Left, const Vec4f &Right);
    __forceinline static Vec4f Minimize(const Vec4f &Left, const Vec4f &Right);
    __forceinline static Vec4f Abs(const Vec4f &V);
};

struct VecNf
{
    //
    // Initalization
    //
    __forceinline VecNf();
    __forceinline VecNf(UINT Dimension);
    __forceinline VecNf(const VecNf &V);

    //
    // Assignment
    //
    __forceinline VecNf& operator = (const VecNf &V);

    //
    // Overloaded operators
    //
    __forceinline VecNf& operator *= (float Right);
    __forceinline VecNf& operator /= (float Right);
    __forceinline VecNf& operator += (const VecNf &Right);
    __forceinline VecNf& operator -= (const VecNf &Right);

    //
    // Accessors
    //
    __forceinline UINT Dimension() const;
    __forceinline float Length() const;
    __forceinline float LengthSq() const;

    __forceinline float& Element(UINT Index)
    {
        return v[Index];
    }
    __forceinline float Element(UINT Index) const
    {
        return v[Index];
    }

    __forceinline float& operator[](UINT Index)
    {
        return v[Index];
    }
    __forceinline float operator[](UINT Index) const
    {
        return v[Index];
    }

    //
    // Local data
    //
    Vector<float> v;

    //
    // Static functions
    //
    __forceinline static VecNf Normalize(const VecNf &Left);
    __forceinline static float Dist(const VecNf &Left, const VecNf &Right);
    __forceinline static float DistSq(const VecNf &Left, const VecNf &Right);
};

struct Rectangle2f
{
    __forceinline Rectangle2f();
    __forceinline Rectangle2f(const Rectangle2f &Rect);
    __forceinline Rectangle2f(const Vec2f &_Min, const Vec2f &_Max);
    __forceinline Rectangle2f(const RECT &Rect);

    __forceinline static Rectangle2f ConstructFromCenterVariance(const Vec2f &Center, const Vec2f &Variance);
    __forceinline static Rectangle2f ConstructFromTwoPoints(const Vec2f &P1, const Vec2f &P2);
    __forceinline static Rectangle2f ConstructFromMinDimensions(const Vec2f &P1, const Vec2f &Dimensions);

    __forceinline Rectangle2f& operator = (const Rectangle2f &Rect);

    __forceinline Vec2f Center() const;
    __forceinline Vec2f Variance() const;
    __forceinline Vec2f Dimensions() const;
    __forceinline float Width() const;
    __forceinline float Height() const;

    __forceinline Rectangle2i RoundToRectangle2i() const;
    __forceinline RECT ToRect() const;
    __forceinline void Expand(float Value);
    __forceinline bool ContainsPt(const Vec2f &Pt) const;

    __forceinline void ExpandBoundingBox(const Vec2f &V);

    Vec2f Min;
    Vec2f Max;
};

struct Rectangle2i
{
    __forceinline Rectangle2i();
    __forceinline Rectangle2i(const Rectangle2i &Rect);
    __forceinline Rectangle2i(const Vec2i &_Min, const Vec2i &_Max);
    __forceinline Rectangle2i(int MinX, int MinY, int MaxX, int MaxY);
    __forceinline Rectangle2i(const RECT &Rect);

    __forceinline static Rectangle2i ConstructFromCenterVariance(const Vec2i &Center, const Vec2i &Variance);
    __forceinline static Rectangle2i ConstructFromTwoPoints(const Vec2i &P1, const Vec2i &P2);
    __forceinline static Rectangle2i ConstructFromMinDimensions(const Vec2i &P1, const Vec2i &Dimensions);

    __forceinline Rectangle2i& operator = (const Rectangle2i &Rect);

    __forceinline Vec2i Center() const;
    __forceinline Vec2i Variance() const;
    __forceinline Vec2i Dimensions() const;
    __forceinline UINT Width() const;
    __forceinline UINT Height() const;

    __forceinline RECT ToRect() const;
    __forceinline void Expand(int Value);
    __forceinline void Expand(const Vec2i &Pt);
    __forceinline bool ContainsPt(const Vec2i &Pt) const;

    Vec2i Min;
    Vec2i Max;
};

struct Rectangle3f
{
    __forceinline Rectangle3f();
    __forceinline Rectangle3f(const Rectangle3f &Rect);
    __forceinline Rectangle3f(const Vec3f &_Min, const Vec3f &_Max);

    //__forceinline static Rectangle3f RectangleFromMinMax(const Vec3f &Min, const Vec3f &Max);
    //__forceinline static Rectangle3f RectangleFromPointDimensions(const Vec3f &Point, const Vec3f &Dimensions);
    __forceinline static Rectangle3f ConstructFromCenterVariance(const Vec3f &Center, const Vec3f &Variance);
    __forceinline static Rectangle3f ConstructFromTwoPoints(const Vec3f &P1, const Vec3f &P2);
    __forceinline static Rectangle3f ConstructFromThreePoints(const Vec3f &P1, const Vec3f &P2, const Vec3f &P3);
    __forceinline static bool Intersects(const Rectangle3f &R0, const Rectangle3f &R1);

    bool ParametricRayIntersect(const Vec3f &RayOrigin, const Vec3f &RayDir, float &TMin, float &TMax) const;
    
    __forceinline Rectangle3f& operator = (const Rectangle3f &Rect);

    __forceinline Vec3f Center() const;
    __forceinline Vec3f Variance() const;
    __forceinline Vec3f Dimensions() const;

    __forceinline bool ContainsPt(const Vec3f &Pt) const;
    __forceinline bool PtWithinDistance(const Vec3f &Pt, float Threshold) const;

    __forceinline float Volume() const;
    __forceinline float SurfaceArea() const;

    __forceinline void ExpandBoundingBox(const Vec3f &V);

    Vec3f Min;
    Vec3f Max;
};

//
// Rectangle2i overloaded operators
//
__forceinline bool operator == (const Rectangle2i &Left, const Rectangle2i &Right);
__forceinline bool operator != (const Rectangle2i &Left, const Rectangle2i &Right);

//
// Vec2f overloaded operators
//
__forceinline Vec2f operator * (const Vec2f &Left, float Right);
__forceinline Vec2f operator * (float Left, const Vec2f &Right);
__forceinline Vec2f operator + (const Vec2f &Left, const Vec2f &Right);
__forceinline Vec2f operator - (const Vec2f &Left, const Vec2f &Right);

//
// Vec2i overloaded operators
//
//__forceinline Vec2i operator * (const Vec2i &Left, float Right);
//__forceinline Vec2i operator * (float Left, const Vec2i &Right);
__forceinline Vec2i operator / (const Vec2i &Left, int Right);
__forceinline Vec2i operator + (const Vec2i &Left, const Vec2i &Right);
__forceinline Vec2i operator - (const Vec2i &Left, const Vec2i &Right);
__forceinline bool operator == (const Vec2i &Left, const Vec2i &Right);
__forceinline bool operator != (const Vec2i &Left, const Vec2i &Right);

//
// Vec3f overloaded operators
//
__forceinline Vec3f operator * (const Vec3f &Left, float Right);
__forceinline Vec3f operator * (float Left, const Vec3f &Right);
__forceinline Vec3f operator / (const Vec3f &Left, float Right);
__forceinline Vec3f operator + (const Vec3f &Left, const Vec3f &Right);
__forceinline Vec3f operator - (const Vec3f &Left, const Vec3f &Right);
__forceinline Vec3f operator - (const Vec3f &V);

//
// Vec4f overloaded operators
//
__forceinline Vec4f operator * (const Vec4f &Left, float Right);
__forceinline Vec4f operator * (float Left, const Vec4f &Right);
__forceinline Vec4f operator / (const Vec4f &Left, float Right);
__forceinline Vec4f operator + (const Vec4f &Left, const Vec4f &Right);
__forceinline Vec4f operator - (const Vec4f &Left, const Vec4f &Right);
__forceinline Vec4f operator - (const Vec4f &V);

//
// VecNf overloaded operators
//
__forceinline VecNf operator * (const VecNf &Left, float Right);
__forceinline VecNf operator * (float Left, const VecNf &Right);
__forceinline VecNf operator / (const VecNf &Left, float Right);
__forceinline VecNf operator + (const VecNf &Left, const VecNf &Right);
__forceinline VecNf operator - (const VecNf &Left, const VecNf &Right);
__forceinline VecNf operator - (const VecNf &V);

namespace Math
{
    void ReorientBasis(Vec3f &V1, const Vec3f &V2, const Vec3f &V3);
}

#include "Vec2f.inl"
#include "Vec2i.inl"
#include "Vec3i.inl"
#include "Vec3f.inl"
#include "Vec4f.inl"
#include "VecNf.inl"
#include "Rectangle2f.inl"
#include "Rectangle2i.inl"
#include "Rectangle3f.inl"

namespace Math
{
    __forceinline float TriangleArea(const Vec3f &P0, const Vec3f &P1, const Vec3f &P2)
    {
        return Vec3f::Cross(P1 - P0, P2 - P0).Length() / 2.0f;
    }

    __forceinline float TriangleArea(const Vec2f &P0, const Vec2f &P1, const Vec2f &P2)
    {
        return TriangleArea(Vec3f(P0, 0.0f), Vec3f(P1, 0.0f), Vec3f(P2, 0.0f));
    }

    __forceinline Vec3f TriangleNormal(const Vec3f &P0, const Vec3f &P1, const Vec3f &P2)
    {
        return Vec3f::Normalize(Vec3f::Cross(P1 - P0, P2 - P0));
    }
}
