/*
Palette.cpp
Written by Matthew Fisher
*/

void Palette::Init(const String &imageFilename, bool wrapping, double minValue, double maxValue)
{
    _minValue = minValue;
    _maxValue = maxValue;

    Bitmap bmp;
    bmp.LoadPNG(imageFilename);
    //bmp.FlipBlueAndRed();

    _colors.FreeMemory();
    for(UINT x = 0; x < bmp.Width(); x++)
    {
        _colors.PushEnd(bmp[0][x]);
    }
    if(wrapping)
    {
        for(UINT x = 0; x < bmp.Width(); x++)
        {
            _colors.PushEnd(bmp[0][bmp.Width() - 1 - x]);
        }
    }
}

RGBColor Palette::SampleColor(double value) const
{
    if(value < _minValue || value > _maxValue)
    {
        return RGBColor::Magenta;
    }

    const int colorCount = _colors.Length();
    
    const double remappedValue = Math::LinearMap(_minValue, _maxValue, 0.0, double(colorCount - 1), value);
    int baseIndex = Math::Floor(remappedValue);
    const RGBColor c0 = _colors[Utility::Bound(baseIndex + 0, 0, colorCount - 1)];
    const RGBColor c1 = _colors[Utility::Bound(baseIndex + 1, 0, colorCount - 1)];
    float s = float(remappedValue - double(baseIndex));
    return RGBColor::Interpolate(c0, c1, s);
}
