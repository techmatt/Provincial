/*
LineSegment3D.h
Written by Matthew Fisher

a 3D line segment represented by an origin point and a direction vector.
*/

struct LineSegment3D
{
    LineSegment3D() {}
    LineSegment3D(const Vec3f &_P0, const Vec3f &_D)
    {
        P0 = _P0;
        D = _D;
    }
    
    float DistToPoint(const Vec3f &P) const;
    float FindLineT(const Vec3f &WorldPos) const;

    void Expand(float Dist);
    
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
