/*
Line3D.h
Written by Matthew Fisher

A standard 3D line.
*/

struct Line3D
{
    Line3D() {}
    Line3D(const Vec3f &_P0, const Vec3f &_D)
    {
        P0 = _P0;
        D = _D;
    }

    float DistToPoint(const Vec3f &V) const;

    static bool ClosestPoint(const Line3D &L1, const Line3D &L2, Vec3f &Result1, Vec3f &Result2);
    static float Dist(const Line3D &L1, const Line3D &L2);
    static float DistSq(const Line3D &L1, const Line3D &L2);

    Vec3f P0;
    Vec3f D;
};
