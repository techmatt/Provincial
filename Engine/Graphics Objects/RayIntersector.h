/*
RayIntersector.h
Written by Matthew Fisher

Abstract interface for ray-triangle mesh intersections.
*/

class RayIntersector
{
public:
    struct Intersection
    {
        UINT FaceIndex;
        Vec3f Pt;
    };
    struct RayIntersectionSorter
    {
        RayIntersectionSorter(const Vec3f &_BasePt)
        {
            BasePt = _BasePt;
        }
        bool operator() (const Intersection &L, const Intersection &R)
        {
            return (Vec3f::DistSq(L.Pt, BasePt) < Vec3f::DistSq(R.Pt, BasePt));
        }
        Vec3f BasePt;
    };

    void InitSingleMesh(const BaseMesh &M)
    {
        Vector<const BaseMesh*> Meshes;
        Meshes.PushEnd(&M);
        InitMeshList(Meshes);
    }
    virtual void InitMeshList(const Vector<const BaseMesh*> &Meshes) = 0;
    virtual void FindIntersections(const Ray3D &R, Vector<Intersection> &IntersectPts) const = 0;
	virtual bool FindFirstIntersection(const Ray3D &R, Intersection &Result, float &DistSq) const = 0;
};
