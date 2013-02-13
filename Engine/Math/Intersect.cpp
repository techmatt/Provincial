/*
Intersect.cpp
Written by Matthew Fisher

Code for computing geometric object intersections.
*/

bool Math::RayIntersectRectangle(const Ray3D &Ray, const Rectangle3f &Rect)
{
    for(UINT PlaneIndex = 0; PlaneIndex < 6; PlaneIndex++)
    {
        Vec3f Pt;
        UINT Axis;
        if(PlaneIndex < 3)
        {
            Pt = Rect.Min;
            Axis = PlaneIndex;
        }
        else
        {
            Pt = Rect.Max;
            Axis = PlaneIndex - 3;
        }
        const float t = (Pt[Axis] - Ray.P0[Axis]) / Ray.D[Axis];
        const Vec3f IntersectPt = Ray.P0 + Ray.D * t;

        if(Rect.PtWithinDistance(IntersectPt, 1e-4f))
        {
            return true;
        }
    }
    return false;
}

//
// Code from http://geometryalgorithms.com/Archive/algorithm_0105/algorithm_0105.htm
//
bool Math::RayIntersectTriangle(const Vec3f &RayPt, const Vec3f &RayDir, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt)
{
    Vec3f u, v, n;             // triangle vectors
    Vec3f w0, w;               // ray vectors
    float r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = T1 - T0;
    v = T2 - T0;
    n = Vec3f::Cross(u, v);
    
    w0 = RayPt - T0;
    a = -Vec3f::Dot(n, w0);
    b = Vec3f::Dot(n, RayDir);
    if(b == 0.0f)
    {
        return false;
    }
    /*if (fabsf(b) < SMALL_NUM) {  // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }*/

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0f)                  // ray goes away from triangle
    {
        return false;              // => no intersect
    }
    // for a segment, also test if (r > 1.0) => no intersect

    IntersectPt = RayPt + r * RayDir; // intersect point of ray and plane

    // is I inside T?
    float uu, uv, vv, wu, wv, D;
    uu = Vec3f::Dot(u, u);
    uv = Vec3f::Dot(u, v);
    vv = Vec3f::Dot(v, v);
    w = IntersectPt - T0;
    wu = Vec3f::Dot(w, u);
    wv = Vec3f::Dot(w, v);
    D = 1.0f / (uv * uv - uu * vv);

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) * D;
    if (s < 0.0f || s > 1.0f)        // I is outside T
    {
        return false;
    }
    t = (uv * wu - uu * wv) * D;
    if (t < 0.0f || (s + t) > 1.0f)  // I is outside T
    {
        return false;
    }
    return true;
}

//
// Code from http://geometryalgorithms.com/Archive/algorithm_0105/algorithm_0105.htm
//
bool Math::LineIntersectTriangle(const Vec3f &LinePt, const Vec3f &LineDir, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt)
{
    Vec3f u, v, n;             // triangle vectors
    Vec3f w0, w;               // ray vectors
    float r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = T1 - T0;
    v = T2 - T0;
    n = Vec3f::Cross(u, v);    // cross product
    
    w0 = LinePt - T0;
    a = -Vec3f::Dot(n, w0);
    b = Vec3f::Dot(n, LineDir);
    if(b == 0.0f)
    {
        return false;
    }
    /*if (fabsf(b) < SMALL_NUM) {  // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }*/

    // get intersect point of ray with triangle plane
    r = a / b;
    // for a segment, also test if (r > 1.0) => no intersect

    IntersectPt = LinePt + r * LineDir; // intersect point of ray and plane

    // is I inside T?
    float uu, uv, vv, wu, wv, D;
    uu = Vec3f::Dot(u, u);
    uv = Vec3f::Dot(u, v);
    vv = Vec3f::Dot(v, v);
    w = IntersectPt - T0;
    wu = Vec3f::Dot(w, u);
    wv = Vec3f::Dot(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0f || s > 1.0f)        // I is outside T
    {
        return false;
    }
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0f || (s + t) > 1.0f)  // I is outside T
    {
        return false;
    }
    return true;
}

bool Math::SegmentIntersectTriangle(const Vec3f &P0, const Vec3f &P1, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt)
{
    bool Result = LineIntersectTriangle(P0, P1 - P0, T0, T1, T2, IntersectPt);
    if(Result)
    {
        return Vec3f::WithinRect(IntersectPt, Rectangle3f::ConstructFromTwoPoints(P0, P1));
    }
    else
    {
        return false;
    }
}

bool Rectangle3f::ParametricRayIntersect(const Vec3f &RayOrigin, const Vec3f &RayDir, float &TMin, float &TMax) const
{
    TMin = 0.0f;
    TMax = 1e10f;
    for (UINT AxisIndex = 0; AxisIndex < 3; AxisIndex++)
    {
        float invRayDir = 1.0f / RayDir[AxisIndex];
        float TNear = (Min[AxisIndex] - RayOrigin[AxisIndex]) * invRayDir;
        float TFar  = (Max[AxisIndex] - RayOrigin[AxisIndex]) * invRayDir;
        
        if (TNear > TFar)
        {
            swap(TNear, TFar);
        }
        TMin = TNear > TMin ? TNear : TMin;
        TMax = TFar  < TMax ? TFar  : TMax;
        if (TMin > TMax)
        {
            return false;
        }
    }
    return true;
}
