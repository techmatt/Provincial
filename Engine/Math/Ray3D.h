/*
Ray3D.h
Written by Matthew Fisher

a 3D ray represented by an origin point and a direction vector.
*/

struct Ray3D
{
    Ray3D() {}
    Ray3D(const Vec3f &_P0, const Vec3f &_D)
    {
        P0 = _P0;
        D = _D;
    }
    
    __forceinline Vec3f Evaluate(float t) const
    {
        return (P0 + D * t);
    }
    __forceinline bool Valid() const
    {
        return (P0.Valid() && D.Valid());
    }

    Vec3f P0, D;
};
