/*
RGBColor.h
Written by Matthew Fisher

Generic 32-bit RGBA triplet structure.
*/

struct RGBColor
{
    RGBColor();
    RGBColor(BYTE _r, BYTE _g, BYTE _b);
    RGBColor(BYTE _r, BYTE _g, BYTE _b, BYTE _a);
    explicit RGBColor(const Vec3f &V);
    explicit RGBColor(const Vec4f &V);

    String CommaSeparatedString() const;
    String SpaceSeparatedString() const;
    
#ifdef USE_D3D
    operator D3DCOLOR();
#endif

    __forceinline RGBColor& operator += (RGBColor c)
    {
        r = Utility::BoundToByte(int(r) + int(c.r));
        g = Utility::BoundToByte(int(g) + int(c.g));
        b = Utility::BoundToByte(int(b) + int(c.b));
        a = Utility::BoundToByte(int(a) + int(c.a));
        return *this;
    }

    __forceinline static UINT DistL1(RGBColor a, RGBColor b)
    {
        return Math::AbsInt(int(a.r) - int(b.r)) +
               Math::AbsInt(int(a.g) - int(b.g)) +
               Math::AbsInt(int(a.b) - int(b.b));
    }
    __forceinline static UINT DistL2(RGBColor a, RGBColor b)
    {
        int DiffR = int(a.r) - int(b.r);
        int DiffG = int(a.g) - int(b.g);
        int DiffB = int(a.b) - int(b.b);
        return DiffR * DiffR + DiffG * DiffG + DiffB * DiffB;
    }

    __forceinline static RGBColor RandomColor()
    {
        return RGBColor(rand() & 255, rand() & 255, rand() & 255);
    }

    __forceinline RGBColor FlipBlueAndRed() const
    {
        return RGBColor(b, g, r, a);
    }

    __forceinline RGBColor Inverse() const
    {
        return RGBColor(255 - r, 255 - g, 255 - b, 255 - a);
    }

    RGBColor RGBToHSV() const;

    static RGBColor Interpolate(RGBColor LowColor, RGBColor HighColor, float s);
    static const RGBColor White;
    static const RGBColor Red;
    static const RGBColor Green;
    static const RGBColor Gray;
    static const RGBColor Blue;
    static const RGBColor Yellow;
    static const RGBColor Orange;
    static const RGBColor Magenta;
    static const RGBColor Black;
    static const RGBColor Cyan;
    static const RGBColor Purple;

    BYTE r, g, b, a;
};

union RGBColor16
{
    union
    {
        struct
        {
            UINT r  : 5;
            UINT g  : 6;
            UINT b  : 5;
        };
        WORD Data;
    };
};

__forceinline RGBColor16 RGBColor16FromRGBColor32(RGBColor c)
{
    RGBColor16 Result;
    Result.r = c.r >> 3;
    Result.g = c.g >> 2;
    Result.b = c.b >> 3;
    return Result;
}

__forceinline RGBColor RGBColor32FromRGBColor16(RGBColor16 c)
{
    return RGBColor(c.r << 3, c.g << 2, c.b << 3);
}

//
// Exact comparison functions.  Does not match the alpha channel.
//
__forceinline bool operator == (RGBColor left, RGBColor right)
{
    return ((left.r == right.r) && (left.g == right.g) && (left.b == right.b));
}

__forceinline bool operator != (RGBColor left, RGBColor right)
{
    return ((left.r != right.r) || (left.g != right.g) || (left.b != right.b));
}

#ifdef USE_D3D
//
// Helper function since D3DCOLORVALUE lacks a constructor
//
D3DCOLORVALUE MakeColor(float r, float g, float b, float a);
#endif
