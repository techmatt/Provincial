/*
Palette.h
Written by Matthew Fisher
*/

class Palette
{
public:
    void Init(const String &imageFilename, bool wrapping, double minValue, double maxValue);
    RGBColor SampleColor(double value) const;

    __forceinline double& MinValue()
    {
        return _minValue;
    }
    __forceinline double& MaxValue()
    {
        return _maxValue;
    }

private:
    double _minValue, _maxValue;
    Vector<RGBColor> _colors;
};
