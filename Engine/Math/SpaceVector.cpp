/*
SpaceVector.cpp
Written by Matthew Fisher
*/

const Vec2i Vec2i::Origin(0, 0);

const Vec2f Vec2f::Origin(0.0f, 0.0f);
const Vec2f Vec2f::eX(1.0f, 0.0f);
const Vec2f Vec2f::eY(0.0f, 1.0f);

const Vec3f Vec3f::Origin(0.0f, 0.0f, 0.0f);
const Vec3f Vec3f::eX(1.0f, 0.0f, 0.0f);
const Vec3f Vec3f::eY(0.0f, 1.0f, 0.0f);
const Vec3f Vec3f::eZ(0.0f, 0.0f, 1.0f);

const Vec4f Vec4f::Origin(0.0f, 0.0f, 0.0f, 0.0f);
const Vec4f Vec4f::eX(1.0f, 0.0f, 0.0f, 0.0f);
const Vec4f Vec4f::eY(0.0f, 1.0f, 0.0f, 0.0f);
const Vec4f Vec4f::eZ(0.0f, 0.0f, 1.0f, 0.0f);
const Vec4f Vec4f::eW(0.0f, 0.0f, 0.0f, 1.0f);

String Vec2i::TabSeparatedString() const
{
    return String(x) + String("\t") + String(y);
}

String Vec2i::CommaSeparatedString() const
{
    return String(x) + String(", ") + String(y);
}

String Vec2f::TabSeparatedString() const
{
    return String(x) + String("\t") + String(y);
}

String Vec2f::CommaSeparatedString() const
{
    return String(x) + String(", ") + String(y);
}

String Vec3f::TabSeparatedString() const
{
    String Seperator = String("\t");
    return String(x) + Seperator + String(y) + Seperator + String(z);
}

String Vec3f::CommaSeparatedString() const
{
    String Seperator = String(", ");
    return String(x) + Seperator + String(y) + Seperator + String(z);
}

String Vec3f::ToString(char seperator) const
{
    String Seperator = String(seperator);
    return String(x) + Seperator + String(y) + Seperator + String(z);
}

String Vec4f::TabSeparatedString() const
{
    String Seperator = String("\t");
    return String(x) + Seperator + String(y) + Seperator + String(z) + Seperator + String(w);
}

String Vec4f::CommaSeparatedString() const
{
    String Seperator = String(", ");
    return String(x) + Seperator + String(y) + Seperator + String(z) + Seperator + String(w);
}
