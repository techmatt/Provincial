/*
Distance.h
Written by Matthew Fisher

Code for computing geometric object distances.
http://www.geometrictools.com/LibMathematics/Distance/Distance.html
*/

namespace Math
{
    //
    // Line-Segment
    //
    struct DistanceLineSegmentResult
    {
        float lineParameter;
        float segmentParameter;
    };
    float DistanceLineSegmentSq(const Vec3f &l0, const Vec3f &l1, const Vec3f &s0, const Vec3f &s1, DistanceLineSegmentResult *result = NULL);
    __forceinline float DistanceLineSegment(const Vec3f &l0, const Vec3f &l1, const Vec3f &s0, const Vec3f &s1)
    {
        return sqrtf(DistanceLineSegment(l0, l1, s0, s1));
    }

    //
    // Point-Triangle
    //
    float DistancePointTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &p);
    __forceinline float DistancePointTriangle(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &p)
    {
        return sqrtf(DistancePointTriangleSq(t0, t1, t2, p));
    }

    //
    // Line-Triangle
    //
    struct DistanceLineTriangleResult
    {
        float lineParameter;
    };
    float DistanceLineTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &l0, const Vec3f &l1, DistanceLineTriangleResult *result = NULL);
    __forceinline float DistanceLineTriangle(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &l0, const Vec3f &l1)
    {
        return sqrtf(DistanceLineTriangleSq(t0, t1, t2, l0, l1));
    }

    //
    // Segment-Triangle
    //
    float DistanceSegmentTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &s0, const Vec3f &s1);
    __forceinline float DistanceSegmentTriangle(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &s0, const Vec3f &s1)
    {
        return sqrtf(DistanceSegmentTriangleSq(t0, t1, t2, s0, s1));
    }

    //
    // Triangle-Triangle
    //
    float DistanceTriangleTriangleSq(const Vec3f &t0v0, const Vec3f &t0v1, const Vec3f &t0v2, const Vec3f &t1v0, const Vec3f &t1v1, const Vec3f &t1v2);
    __forceinline float DistanceTriangleTriangle(const Vec3f &t0v0, const Vec3f &t0v1, const Vec3f &t0v2, const Vec3f &t1v0, const Vec3f &t1v1, const Vec3f &t1v2)
    {
        return sqrtf(DistanceTriangleTriangleSq(t0v0, t0v1, t0v2, t1v0, t1v1, t1v2));
    }
}
