/*
RayIntersectorBruteForce.h
Written by Matthew Fisher

Brute force implementation of RayIntersector.
*/

class RayIntersectorBruteForce : public RayIntersector
{
public:
    void InitMeshList(const Vector<const BaseMesh*> &M);
    void FindIntersections(const Ray3D &R, Vector<Intersection> &IntersectPts) const;
	bool FindFirstIntersection(const Ray3D &R, Intersection &Result, float &DistSq) const;

private:
    //Vector<Vec3f> _Vertices;
    //Vector<UINT> _Indices;
	Vector<TriangleRayIntersection> _Triangles;
};
