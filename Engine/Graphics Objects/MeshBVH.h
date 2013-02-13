/*
MeshBVH.h
Written by Matthew Fisher

A 3D bounding volume heiarchy for representing meshes.
*/

const bool MeshBVHDebugging = false;

struct MeshBVHTriangleInfo
{
    Vec3f v[3];
    mutable float value;

    struct Sorter
    {
        bool operator()(const MeshBVHTriangleInfo *left, const MeshBVHTriangleInfo *right)
        {
            return (left->value < right->value);
        }
    };
};

struct MeshIntersectStatistics
{
    MeshIntersectStatistics()
    {
        triComparisons = 0;
        bboxComparisons = 0;
    }
    void Describe() const;
    UINT triComparisons;
    UINT bboxComparisons;
};

struct MeshBVHNodeConstructorInfo
{
    const MeshBVHTriangleInfo **storage;
    UINT storageIndex;
    UINT triangleCountCutoff;
};

struct MeshBVHNode
{
    MeshBVHNode(MeshBVHTriangleInfo **data, UINT dataLength, MeshBVHNodeConstructorInfo &info, UINT depth);
    ~MeshBVHNode()
    {
        if(children[0] != NULL) delete children[0];
        if(children[1] != NULL) delete children[1];
    }

    bool isLeafNode() const
    {
        return (children[0] == NULL);
    }

    float size() const
    {
        return variance.x + variance.y + variance.z;
    }

    Vec3f center, variance;
    MeshBVHNode* children[2];
    UINT16 triIndex;
    UINT16 triCount;
};

class MeshBVH
{
public:
    MeshBVH();
    ~MeshBVH();
    void FreeMemory();

    void InitSingleMesh(const BaseMesh &M, UINT triangleCountCutoff = 8);
    void InitMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes, UINT triangleCountCutoff = 8);

    static bool Intersect(const MeshBVH &left, const MeshBVH &right, MeshIntersectStatistics &stats);

private:
    MeshBVHNode *_root;
    Vector<MeshBVHTriangleInfo> _triInfo;
    Vector<const MeshBVHTriangleInfo*> _storage;
};
