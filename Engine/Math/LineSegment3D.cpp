/*
LineSegment3D.cpp
Written by Matthew Fisher

a 3D line segment represented by an origin point and a direction vector.
*/

void LineSegment3D::Expand(float Dist)
{
    Vec3f Extension = Vec3f::Normalize(D) * Dist;
    P0 -= Extension;
    D += Extension * 2.0f;
}

float LineSegment3D::FindLineT(const Vec3f &P) const
{
    SignalError("Not implemented");
    return 0.0f;
}

float LineSegment3D::DistToPoint(const Vec3f &P) const
{
    float t0 = Vec3f::Dot(D, P - P0) / Vec3f::Dot(D, D);

    float distanceLine = Vec3f::Dist(P, P0 + t0 * D);

    if ( (t0 >= 0) && (t0 <= 1) )
    {
        return distanceLine;
    }
    else
    {
        float dist1 = Vec3f::Dist(P, P0);
        float dist2 = Vec3f::Dist(P, P0 + D);
        return Math::Min(dist1, dist2);
    }
}
