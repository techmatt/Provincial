/*
ColorGenerator.h
Written by Matthew Fisher
*/

class ColorGenerator
{
public:
    static void Generate(Vector<RGBColor> &Result);
    static void Generate(Vector<RGBColor> &Result, const Vec3f &Scale);
    static Vector<RGBColor> Generate(UINT colorCount);

private:
    
};