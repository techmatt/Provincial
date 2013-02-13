/*
RGBColor.cpp
Written by Matthew Fisher

Generic 32-bit RGBA triplet structure.  Rather self-explanitory.
*/

const RGBColor RGBColor::White(255, 255, 255);
const RGBColor RGBColor::Gray(128, 128, 128);
const RGBColor RGBColor::Red(255, 0, 0);
const RGBColor RGBColor::Green(0, 255, 0);
const RGBColor RGBColor::Blue(0, 0, 255);
const RGBColor RGBColor::Yellow(255, 255, 0);
const RGBColor RGBColor::Magenta(255, 0, 255);
const RGBColor RGBColor::Cyan(0, 255, 255);
const RGBColor RGBColor::Black(0, 0, 0);
const RGBColor RGBColor::Orange(255, 91, 0);
const RGBColor RGBColor::Purple(111, 49, 152);

RGBColor::RGBColor()
{

}

RGBColor::RGBColor(BYTE _r, BYTE _g, BYTE _b)
{
    r = _r;
    g = _g;
    b = _b;
    a = 0;
}

RGBColor::RGBColor(BYTE _r, BYTE _g, BYTE _b, BYTE _a)
{
    r = _r;
    g = _g;
    b = _b;
    a = _a;
}

RGBColor::RGBColor(const Vec3f &V)
{
    r = Utility::BoundToByte(V.x * 255.0f);
    g = Utility::BoundToByte(V.y * 255.0f);
    b = Utility::BoundToByte(V.z * 255.0f);
}

RGBColor::RGBColor(const Vec4f &V)
{
    r = Utility::BoundToByte(V.x * 255.0f);
    g = Utility::BoundToByte(V.y * 255.0f);
    b = Utility::BoundToByte(V.z * 255.0f);
    a = Utility::BoundToByte(V.w * 255.0f);
}

#ifdef USE_D3D
RGBColor::operator D3DCOLOR()
{
    return D3DCOLOR_RGBA(r, g, b, a);
}
#endif

RGBColor RGBColor::Interpolate(RGBColor L, RGBColor R, float s)
{
    return RGBColor(Utility::BoundToByte(int(int(L.r) + s * float(int(R.r) - int(L.r)))),
                    Utility::BoundToByte(int(int(L.g) + s * float(int(R.g) - int(L.g)))),
                    Utility::BoundToByte(int(int(L.b) + s * float(int(R.b) - int(L.b)))),
                    Utility::BoundToByte(int(int(L.a) + s * float(int(R.a) - int(L.a)))));
}

String RGBColor::CommaSeparatedString() const
{
    return String("(") +
           String(int(r)) + String(", ") +
           String(int(g)) + String(", ") +
           String(int(b)) +
           String(")");
}

String RGBColor::SpaceSeparatedString() const
{
    return String(int(r)) + String(" ") +
           String(int(g)) + String(" ") +
           String(int(b));
}

RGBColor RGBColor::RGBToHSV() const
{
    // RGB are from 0..1, H is from 0..360, SV from 0..1
    
    double maxC = b;
    if (maxC < g) maxC = g;
    if (maxC < r) maxC = r;
    double minC = b;
    if (minC > g) minC = g;
    if (minC > r) minC = r;

    double delta = maxC - minC;

    double V = maxC;
    double S = 0;
    double H = 0;

    if (delta == 0)
    {
	    H = 0;
	    S = 0;
    }
    else
    {
	    S = delta / maxC;
	    double dR = 60*(maxC - r)/delta + 180;
	    double dG = 60*(maxC - g)/delta + 180;
	    double dB = 60*(maxC - b)/delta + 180;
	    if (r == maxC)
		    H = dB - dG;
	    else if (g == maxC)
		    H = 120 + dR - dB;
	    else
		    H = 240 + dG - dR;
    }

    if (H<0)
	    H+=360;
    if (H>=360)
	    H-=360;

    return RGBColor(Utility::BoundToByte(H / 360.0f * 255.0f), Utility::BoundToByte(S * 255.0f), Utility::BoundToByte(V * 255.0f));
}

#ifdef USE_D3D
D3DCOLORVALUE MakeColor(float r, float g, float b, float a)
{
    D3DCOLORVALUE C;
    C.r = r;
    C.g = g;
    C.b = b;
    C.a = a;
    return C;
}
#endif
