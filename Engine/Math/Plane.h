/*
Plane.h
Written by Matthew Fisher

A standard 3D plane (space plane;) i. e. the surface defined by a*x + b*y + c*z + d = 0
*/

struct Plane
{
    //
    // Initalization
    //
    Plane();
    Plane(const Plane &P);
    Plane(float _a, float _b, float _c, float _d);
    Plane(const Vec3f &NormalizedNormal, float _d);

    //
    // Static constructors
    //
    static Plane ConstructFromPointNormal(const Vec3f &Pt, const Vec3f &Normal);                //loads the plane from a point on the surface and a normal vector
    static Plane ConstructFromPointVectors(const Vec3f &Pt, const Vec3f &V1, const Vec3f &V2);    //loads the plane from a point on the surface and two vectors in the plane
    static Plane ConstructFromPoints(const Vec3f &V1, const Vec3f &V2, const Vec3f &V3);        //loads the plane from 3 points on the surface

    //
    // Math functions
    //
    float UnsignedDistance(const Vec3f &Pt) const;
    float SignedDistance(const Vec3f &Pt) const;
    bool FitToPoints(const Vector<Vec3f> &Points, float &ResidualError);
    bool FitToPoints(const Vector<Vec4f> &Points, Vec3f &Basis1, Vec3f &Basis2, float &NormalEigenvalue, float &ResidualError);
    Vec3f ClosestPoint(const Vec3f &Point);

    //
    // Line intersection
    //
    Vec3f IntersectLine(const Vec3f &V1, const Vec3f &V2) const;        //determines the intersect of the line defined by the points V1 and V2 with the plane.
                                                            //Returns the point of intersection.  Origin is returned if no intersection exists.
    Vec3f IntersectLine(const Vec3f &V1, const Vec3f &V2, bool &Hit) const;    //determines the intersect of the line defined by the points V1 and V2 with the plane.
                                                                    //If there is no intersection, Hit will be false.
    float IntersectLineRatio(const Vec3f &V1, const Vec3f &V2);    //Paramaterize the line with the variable t such that t = 0 is V1 and t = 1 is V2.
                                                                //returns the t for this line that lies on this plane.
    Vec3f IntersectLine(const Line3D &Line) const;

    static float Dot(const Plane &P, const Vec4f &V);            //dot product of a plane and a 4D vector
    static float DotCoord(const Plane &P, const Vec3f &V);        //dot product of a plane and a 3D coordinate
    static float DotNormal(const Plane &P, const Vec3f &V);    //dot product of a plane and a 3D normal

    //
    // Normalization
    //
    Plane Normalize();

    //
    // Accessors
    //
    __forceinline Vec3f Plane::Normal() const
    {
        return Vec3f(a, b, c);
    }

    __forceinline Plane Flip()
    {
        Plane Result;
        Result.a = -a;
        Result.b = -b;
        Result.c = -c;
        Result.d = -d;
        return Result;
    }

    static bool PlanePlaneIntersection(const Plane &P1, const Plane &P2, Line3D &L);

#ifdef USE_D3D
    operator D3DXPLANE();
#endif

    float a, b, c, d;        //the (a, b, c, d) in a*x + b*y + c*z + d = 0.
};
