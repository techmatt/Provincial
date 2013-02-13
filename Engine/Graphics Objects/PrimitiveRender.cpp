/*
PrimitiveRender.cpp
Written by Matthew Fisher

Rendering functions for lines and polygons in software.  Not very efficent.
DrawLine uses integer arithmetic but DrawPolygon doesn't.  I decide not to comment
these because I intend to rewrite them, but they work good enough under most conditions.
*/

void PrimitiveRender::DrawSquare(Bitmap &Bmp, const Vec2i &Coords, int Radius, RGBColor InteriorColor, RGBColor BorderColor)
{
    DrawRect(Bmp, Rectangle2i::ConstructFromCenterVariance(Coords, Vec2i(Radius, Radius)), InteriorColor, BorderColor);
}

void AliasRender::DrawRect(Bitmap &Bmp, const Rectangle2i &Rect, RGBColor InteriorColor, RGBColor BorderColor)
{
    const int Height = Bmp.Height();
    const int Width = Bmp.Width();
    for(int y = Rect.Min.y; y <= Rect.Max.y; y++)
    {
        if(y >= 0 && y < Height)
        {
            for(int x = Rect.Min.x; x <= Rect.Max.x; x++)
            {
                if(x >= 0 && x < Width)
                {
                    if(x == Rect.Min.x || x == Rect.Max.x || y == Rect.Min.y || y == Rect.Max.y)
                    {
                        Bmp[y][x] = BorderColor;
                    }
                    else
                    {
                        Bmp[y][x] = InteriorColor;
                    }
                }
            }
        }
    }
}

void AliasRender::DrawTransparentRect(Bitmap &Bmp, const Rectangle2i &Rect, RGBColor InteriorColor, RGBColor BorderColor)
{
    const int Height = Bmp.Height();
    const int Width = Bmp.Width();
    for(int y = Rect.Min.y; y <= Rect.Max.y; y++)
    {
        if(y >= 0 && y < Height)
        {
            for(int x = Rect.Min.x; x <= Rect.Max.x; x++)
            {
                if(x >= 0 && x < Width)
                {
                    if(x == Rect.Min.x || x == Rect.Max.x || y == Rect.Min.y || y == Rect.Max.y)
                    {
                        Bmp[y][x] = BorderColor;
                    }
                    else
                    {
                        RGBColor &C = Bmp[y][x];
                        C = RGBColor(int(C.r) * InteriorColor.r / 255, int(C.g) * InteriorColor.g / 255, int(C.b) * InteriorColor.b / 255);
                    }
                }
            }
        }
    }
}

void AliasRender::DrawCircle(Bitmap &Bmp, const Vec2i &Coords, int Radius, RGBColor Color)
{
    const int Height = Bmp.Height();
    const int Width = Bmp.Width();
    const int RadiusSq = Radius * Radius;
    for(int y = -Radius; y <= Radius; y++)
    {
        int BmpY = y + Coords.y;
        if(BmpY >= 0 && BmpY < Height)
        {
            int ySq = y * y;
            for(int x = -Radius; x <= Radius; x++)
            {
                int BmpX = x + Coords.x;
                if(BmpX >= 0 && BmpX < Width && ySq + x * x < RadiusSq)
                {
                    Bmp[BmpY][BmpX] = Color;
                }
            }
        }
    }
}

void AliasRender::DrawLine(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color)
{
    int YSign = 1;

    if(x2 < x1)
    {
        Utility::Swap(x1, x2);
        Utility::Swap(y1, y2);
    }

    if(y2 < y1)
    {
        YSign = -1;
        if(x2 - x1 + y2 - y1 < 0)
            DrawLineR2(B, x1, y1, x2, y2, Color, YSign);
        else
            DrawLineR1(B, x1, y1, x2, y2, Color, YSign);
    } else {
        if(x2 - x1 + y1 - y2 < 0)
            DrawLineR2(B, x1, y1, x2, y2, Color, YSign);
        else
            DrawLineR1(B, x1, y1, x2, y2, Color, YSign);
    }
}

void AliasRender::DrawLineR1(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color, int YSign)
{
    int y = y1;
    int dy = (y2 - y1) * YSign;
    int dxdy = (y2 - y1) * YSign + x1 - x2;
    int F = dxdy;
    int x;
    for(x = x1;x <= x2;x++)
    {
        if(x >= 0 && x < int(B.Width()) && y >= 0 && y < int(B.Height()))
            B[y][x] = Color;
        if( F < 0 )
            F += dy;
        else
        {
            y += YSign;
            F += dxdy;
        }
    }
}

void AliasRender::DrawLineR2(Bitmap &B, int x1, int y1, int x2, int y2, const RGBColor &Color, int YSign)
{
    int x = x1;
    int dx = x2-x1;
    int dxdy = x2-x1+(y1-y2)*YSign;
    int F = x2-x1+(y1-y2)*YSign;
    int y;

    if(YSign == 1)
    {
        for(y = y1;y <= y2;y++)
        {
            if(x >= 0 && x < int(B.Width()) && y >= 0 && y < int(B.Height()))
                B[y][x] = Color;
            if( F < 0 )
                F += dx;
            else
            {
                x++;
                F += dxdy;
            }
        }
    } else {
        for(y = y1;y >= y2;y--)
        {
            if(x >= 0 && x < int(B.Width()) && y >= 0 && y < int(B.Height()))
                B[y][x] = Color;
            if( F < 0 )
                F += dx;
            else
            {
                x++;
                F += dxdy;
            }
        }
    }
}
