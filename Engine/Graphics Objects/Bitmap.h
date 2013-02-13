/*
Bitmap.h
Written by Matthew Fisher

A bitmap class (a 2D array of RGBColor's)
Rather self-explanitory, can only save and load a few primitive formats.
*/

#pragma once

struct BitmapSaveOptions
{
    BitmapSaveOptions()
    {
        //BGREncoding = false;
        //VerticalFlip = false;

        Region = Rectangle2i(Vec2i::Origin, Vec2i::Origin);
        ScratchSpace = NULL;
        ScratchSpaceSize = 0;
        SaveAlpha = false;
        UseBGR = false;
    }
    //bool BGREncoding;
    //bool VerticalFlip;

    Rectangle2i Region;
    BYTE *ScratchSpace;
    UINT ScratchSpaceSize;
    bool SaveAlpha;
    bool UseBGR;
};

class Bitmap
{
public:
    Bitmap();
    Bitmap(const Bitmap &B);
    Bitmap(Bitmap &&B);
    Bitmap(const Bitmap &B, UINT x, UINT y, UINT Width, UINT Height);
    Bitmap(UINT Width, UINT Height);
    Bitmap(UINT Width, UINT Height, RGBColor clearColor);

    ~Bitmap();

    //
    // Memory
    //
    void FreeMemory();
    void Allocate(UINT Width, UINT Height);
    void Allocate(UINT Width, UINT Height, RGBColor clearColor);
    void operator = (const Bitmap &Bmp);
    void operator = (Bitmap &&Bmp);

    //
    // Accessors
    //
    __forceinline RGBColor* operator [] (UINT Row) {return &_Data[Row * _Width];}
    __forceinline const RGBColor* operator [] (UINT Row) const {return &_Data[Row * _Width];}
    __forceinline RGBColor* Pixels() {return _Data;}
    __forceinline const RGBColor* Pixels() const {return _Data;}
    __forceinline UINT Width() const {return _Width;}
    __forceinline UINT Height() const {return _Height;}
    __forceinline UINT PixelCount() const {return _Width * _Height;}
    __forceinline Vec2i Dimensions() const {return Vec2i(_Width, _Height);}
    __forceinline RGBColor SampleNearest(const Vec2f &Pos)
    {
        Vec2i SamplePos(Utility::Bound(Math::Round(Pos.x * (_Width - 1)), 0, int(_Width) - 1),
                        Utility::Bound(Math::Round(Pos.y * (_Height - 1)), 0, int(_Height) - 1));
        return _Data[SamplePos.y * _Width + SamplePos.x];
    }

    //
    // File Functions
    //
    void SaveBMP(const String &Filename) const; //saves bitmap to Filename in 32-bit *.BMP format
    void SavePPM(const String &Filename) const; //saves bitmap to Filename in *.PPM format
    void SavePGM(const String &Filename, int Channel) const;
    void SavePNG(const String &Filename) const; //saves bitmap to Filename in 24-bit *.PNG format
    void SavePNG(const String &Filename, const BitmapSaveOptions &Options) const;
    void SavePNGToMemory(Vector<BYTE> &Buffer) const; //save bitmap to Buffer in *.PNG format
    void SavePNGToMemory(Vector<BYTE> &Buffer, const BitmapSaveOptions &Options) const;
    Vector<BYTE> SavePNGToMemory() const;
    UINT SaveDelta(const Bitmap &Bmp, const String &Filename) const;
    void LoadBMP(const String &Filename); //loads bitmap from Filename in *.BMP format
    void LoadPNG(const String &Filename); //loads bitmap from Filename in *.PNG format
    void LoadPNGFromMemory(const Vector<BYTE> &Buffer); //loads bitmap from Buffer in *.PNG format
    void LoadSDL(const String &Filename); //loads bitmap from most file formats using SDL

#ifdef USE_D3D9
    void LoadFromSurface(LPDIRECT3DSURFACE9 Surface);
	static void SaveSurfaceToPNG(LPDIRECT3DSURFACE9 Surface, const String &Filename, const BitmapSaveOptions &Options);
#endif

    //
    // Transfer Functions
    //
    __forceinline void BltTo(Bitmap &B, const Vec2i &Target) const
    {
        BltTo(B, Target.x, Target.y);
    }
    void BltTo(Bitmap &B, int TargetX, int TargetY) const;
    void BltToNoClipMemcpy(Bitmap &B, UINT TargetX, UINT TargetY) const;
    void BltTo(Bitmap &B, int TargetX, int TargetY, int SourceX, int SourceY, int Width, int Height) const;
    
    enum SamplingType
    {
        SamplingPoint,
        SamplingLinear,
    };
    void StretchBltTo(Bitmap &B, const Rectangle2i &TargetRect, const Rectangle2i &SourceRect, SamplingType Sampling) const;
    void StretchBltTo(Bitmap &B, int TargetX, int TargetY, int TargetWidth, int TargetHeight, int SourceX, int SourceY, int SourceWidth, int SourceHeight, SamplingType Sampling) const;
    
    void LoadGrid(const Grid<float> &Values);
    void LoadGrid(const Grid<float> &Values, float Min, float Max);
    void FlipHorizontal();
    void FlipVertical();
    
    //
    // Query
    //
    UINT Hash32() const;
    UINT64 Hash64() const;
    bool PerfectMatch(const Bitmap &Bmp) const;
    bool Monochrome(RGBColor Color) const;
    bool MonochromeIncludingAlpha(RGBColor Color) const;
    UINT CountPixelsWithColor(RGBColor Color) const;

    __forceinline bool ValidCoordinates(int x, int y) const
    {
        return (x >= 0 && x < int(_Width) && y >= 0 && y < int(_Height));
    }

    //
    // Modifiers
    //
    void ReplaceColor(RGBColor SourceColor, RGBColor NewColor);
    void Clear(const RGBColor &Color);    //clears all pixels to Color
    void Clear();                         //clears all pixels to RGBColor::Black
    void LoadAlphaChannelAsGrayscale();
    void FlipBlueAndRed();
    void DownsampleQuarter();

    //
    // Query
    //
    RGBColor AverageColorOverRegion(const Rectangle2f &Region) const;
    
private:
#ifdef USE_PNG
    static void __cdecl PNGReadFromBuffer(png_structp png_ptr, png_bytep data, png_size_t length);
    static void __cdecl PNGWriteToBuffer(png_structp png_ptr, png_bytep data, png_size_t length);
    static void __cdecl PNGFlushBuffer(png_structp png_ptr);
    void PNGCompleteRead(png_structp PngRead, png_infop PngInfo, const String &Filename);
    void PNGCompleteWrite(png_structp PngWrite, png_infop PngInfo, const BitmapSaveOptions &Options) const;
#endif

    UINT _Width, _Height;   //width and height of the bitmap
    RGBColor* _Data;        //Raw RGBColor data in one big array
};

OutputDataStream& operator << (OutputDataStream &stream, const Bitmap &bmp);
InputDataStream& operator >> (InputDataStream &stream, Bitmap &bmp);

namespace Utility
{
    __forceinline bool PointInsideBitmap(const Bitmap &Bmp, int x, int y)
    {
        return (between(x, 0, int(Bmp.Width()) - 1) &&
                between(y, 0, int(Bmp.Height()) - 1));
    }
}
