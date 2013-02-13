/*
MeshDistance.h
Written by Matthew Fisher

Abstract interface for mesh-mesh distance.
*/

class MeshDistance
{
public:
    void InitSingleMesh(const BaseMesh &m, const Matrix4 &transform)
    {
        Vector< pair<const BaseMesh*, Matrix4> > meshes;
        meshes.PushEnd( pair<const BaseMesh*, Matrix4>(&m, transform) );
        InitMeshList(meshes);
    }
    virtual void InitMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes) = 0;
    virtual bool IntersectMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes) const = 0;
    bool IntersectSingleMesh(const BaseMesh &m, const Matrix4 &transform) const
    {
        Vector< pair<const BaseMesh*, Matrix4> > meshes(1);
        meshes[0] = make_pair(&m, transform);
        return IntersectMeshList(meshes);
    }
};

class MeshDistanceBruteForce : public MeshDistance
{
public:
    void InitMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes);
    bool IntersectMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes) const;

private:
    struct TriangleEntryA
    {
        Vec3f center, variance;
    };
    struct TriangleEntryB
    {
        Vec3f V[3];
    };
    Vector<TriangleEntryA> _trianglesA;
    Vector<TriangleEntryB> _trianglesB;
};