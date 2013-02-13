/*
RayIntersectorKDTree.h
Written by Matthew Fisher

KD-tree implementation of RayIntersector.
*/

class RayIntersectorKDTree;
struct RayIntersectorKDTreeNode
{
public:
    RayIntersectorKDTreeNode(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Depth, Vector<UINT> &LeafTriangles);

    void FindFirstIntersect(const Ray3D &R, Vec3f &IntersectPt, bool &HitFound, const RayIntersectorKDTree &Root) const;
    
private:
    void MakeLeafNode(const Vector<UINT> &TriangleIndices, Vector<UINT> &LeafTriangles);
    void MakeInteriorNode(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Depth, Vector<UINT> &LeafTriangles);
    bool FindBestSplit(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT &Axis, float &SplitValue) const;
    void TestSplit(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Axis, const Rectangle3f &BBox, UINT &LeftCount, UINT &RightCount) const;
    void TestLeafTriangles(const Ray3D &R, Vec3f &IntersectPt, bool &HitFound, const RayIntersectorKDTree &Root) const;
    Rectangle3f ComputeBBox(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root) const;

    struct
    {
        UINT Leaf       : 1;
        UINT Unused     : 31;
    } _Flags;
    Rectangle3f _BBox;
    union
    {
        struct
        {
            RayIntersectorKDTreeNode *Left, *Right;
        } _Interior;
        struct
        {
            UINT TrianglesStartIndex, TrianglesCount;
        } _Leaf;
    };
};

class RayIntersectorKDTree : public RayIntersector
{
public:
    RayIntersectorKDTree();
    ~RayIntersectorKDTree();
    void FreeMemory();

    void Init(const BaseMesh &M);
    bool FindFirstIntersect(const Ray3D &R, Vec3f &IntersectPt) const;

private:
    friend struct RayIntersectorKDTreeNode;

    Vec3f *_Vertices;
    UINT *_Indices;
    UINT *_LeafTriangles;

    UINT _VertexCount;
    UINT _TriangleCount;
    UINT _LeafTriangleCount;

    RayIntersectorKDTreeNode *_Root;
};
