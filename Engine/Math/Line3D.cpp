/*
Line3D.cpp
Written by Matthew Fisher

A standard 3D line.
*/

bool Line3D::ClosestPoint(const Line3D &L1, const Line3D &L2, Vec3f &Result1, Vec3f &Result2)
{
    Vec3f NewDir = Vec3f::Cross(L1.D, L2.D);
    float Length = NewDir.Length();
    if(Length == 0.0f)
    {
        return false;
    }

    Plane P1 = Plane::ConstructFromPointVectors(L1.P0, NewDir, L1.D);
    Plane P2 = Plane::ConstructFromPointVectors(L2.P0, NewDir, L2.D);

    Result1 = P2.IntersectLine(L1);
    Result2 = P1.IntersectLine(L2);

    return true;
}

float Line3D::Dist(const Line3D &L1, const Line3D &L2)
{
    Vec3f Cross = Vec3f::Cross(L1.D, L2.D);
    return fabsf(Vec3f::Dot(L2.P0 - L1.P0, Cross)) / Cross.Length();
}

float Line3D::DistSq(const Line3D &L1, const Line3D &L2)
{
    Vec3f Cross = Vec3f::Cross(L1.D, L2.D);
    float Dot = Vec3f::Dot(L2.P0 - L1.P0, Cross);
    return (Dot * Dot) / Cross.LengthSq();
}

float Line3D::DistToPoint(const Vec3f &P) const
{
    float t0 = Vec3f::Dot(D, P - P0) / Vec3f::Dot(D, D);
    float distanceLine = Vec3f::Dist(P, P0 + t0 * D);
    return distanceLine;
}
