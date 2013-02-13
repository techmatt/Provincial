/*
PrimitiveRender.h
Written by Matthew Fisher

Rendering functions for lines and polygons in software.  Not very efficent.
*/

class PrimitiveRender
{
public:
    virtual void DrawLine(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color) = 0;
    void DrawSquare(Bitmap &Bmp, const Vec2i &Coords, int Radius, RGBColor InteriorColor, RGBColor BorderColor);
    virtual void DrawRect(Bitmap &Bmp, const Rectangle2i &Rect, RGBColor InteriorColor, RGBColor BorderColor) = 0;
    virtual void DrawCircle(Bitmap &Bmp, const Vec2i &Coords, int Radius, RGBColor Color) = 0;
};

//
// aliased version of PrimitiveRender
//
class AliasRender : public PrimitiveRender
{
public:
    void DrawLine(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color);
    void DrawRect(Bitmap &Bmp, const Rectangle2i &Rect, RGBColor InteriorColor, RGBColor BorderColor);
    void DrawTransparentRect(Bitmap &Bmp, const Rectangle2i &Rect, RGBColor InteriorColor, RGBColor BorderColor);
    void DrawCircle(Bitmap &Bmp, const Vec2i &Coords, int Radius, RGBColor Color);

private:
    void DrawLineR1(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color, int YSign);
    void DrawLineR2(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color, int YSign);
};
