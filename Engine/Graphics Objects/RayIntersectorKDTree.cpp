/*
RayIntersectorKDTree.h
Written by Matthew Fisher

KD-tree implementation of RayIntersector.
*/

RayIntersectorKDTreeNode::RayIntersectorKDTreeNode(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Depth, Vector<UINT> &LeafTriangles)
{
    Assert(Depth < 60, "Tree unexpectedly deep");
    _BBox = ComputeBBox(TriangleIndices, Root);
    const UINT MinTriCount = 8;
    if(TriangleIndices.Length() <= MinTriCount)
    {
        MakeLeafNode(TriangleIndices, LeafTriangles);
    }
    else
    {
        MakeInteriorNode(TriangleIndices, Root, Depth, LeafTriangles);
    }
}

void RayIntersectorKDTreeNode::TestLeafTriangles(const Ray3D &R, Vec3f &IntersectPt, bool &HitFound, const RayIntersectorKDTree &Root) const
{
    for(UINT TriangleIndex = 0; TriangleIndex < _Leaf.TrianglesCount; TriangleIndex++)
    {
        UINT BaseIndex = Root._LeafTriangles[_Leaf.TrianglesStartIndex + TriangleIndex] * 3;
        const Vec3f &P0 = Root._Vertices[Root._Indices[BaseIndex + 0]];
        const Vec3f &P1 = Root._Vertices[Root._Indices[BaseIndex + 1]];
        const Vec3f &P2 = Root._Vertices[Root._Indices[BaseIndex + 2]];
        Vec3f CurIntersectPt;
        if(Math::RayIntersectTriangle(R.P0, R.D, P0, P1, P2, CurIntersectPt))
        {
            float CurIntersectDistSq = Vec3f::DistSq(R.P0, CurIntersectPt);
            if(HitFound)
            {
                if(CurIntersectDistSq < Vec3f::DistSq(R.P0, IntersectPt))
                {
                    IntersectPt = CurIntersectPt;
                }
            }
            else
            {
                HitFound = true;
                IntersectPt = CurIntersectPt;
            }
        }
    }
}

void RayIntersectorKDTreeNode::FindFirstIntersect(const Ray3D &R, Vec3f &IntersectPt, bool &HitFound, const RayIntersectorKDTree &Root) const
{
    if(_Flags.Leaf)
    {
        TestLeafTriangles(R, IntersectPt, HitFound, Root);
    }
    else
    {
        bool IntersectLeft = Math::RayIntersectRectangle(R, _Interior.Left->_BBox);
        bool IntersectRight = Math::RayIntersectRectangle(R, _Interior.Right->_BBox);
        if(IntersectLeft && !IntersectRight)
        {
            _Interior.Left->FindFirstIntersect(R, IntersectPt, HitFound, Root);
        }
        else if(!IntersectLeft && IntersectRight)
        {
            _Interior.Right->FindFirstIntersect(R, IntersectPt, HitFound, Root);
        }
        else if(IntersectLeft && IntersectRight)
        {
            _Interior.Left->FindFirstIntersect(R, IntersectPt, HitFound, Root);
            _Interior.Right->FindFirstIntersect(R, IntersectPt, HitFound, Root);
        }
    }
}

void RayIntersectorKDTreeNode::MakeLeafNode(const Vector<UINT> &TriangleIndices, Vector<UINT> &LeafTriangles)
{
    _Flags.Leaf = 1;
    _Leaf.TrianglesStartIndex = LeafTriangles.Length();
    _Leaf.TrianglesCount = TriangleIndices.Length();
    LeafTriangles.Append(TriangleIndices);
}

void RayIntersectorKDTreeNode::MakeInteriorNode(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Depth, Vector<UINT> &LeafTriangles)
{
    UINT BestAxis;
    float SplitValue;
    if(FindBestSplit(TriangleIndices, Root, BestAxis, SplitValue))
    {
        _Flags.Leaf = 0;
        //_Flags.Axis = BestAxis;
        Vector<UINT> LeftTriangleIndices, RightTriangleIndices;

        for(UINT TriangleIndex = 0; TriangleIndex < TriangleIndices.Length(); TriangleIndex++)
        {
            UINT LeftVertices = 0, RightVertices = 0;
            const UINT BaseIndexIndex = TriangleIndices[TriangleIndex] * 3;
            for(UINT VertexIndex = 0; VertexIndex < 3; VertexIndex++)
            {
                const Vec3f &CurVertex = Root._Vertices[Root._Indices[BaseIndexIndex + VertexIndex]];
                if(CurVertex[BestAxis] < SplitValue)
                {
                    LeftVertices++;
                }
                if(CurVertex[BestAxis] >= SplitValue)
                {
                    RightVertices++;
                }
            }
            bool OnLeftSide = (LeftVertices > RightVertices);
            bool OnRightSide = !OnLeftSide;
            if(OnLeftSide)
            {
                LeftTriangleIndices.PushEnd(TriangleIndices[TriangleIndex]);
            }
            if(OnRightSide)
            {
                RightTriangleIndices.PushEnd(TriangleIndices[TriangleIndex]);
            }
        }

        //_Interior.SplitValue = SplitValue;
        _Interior.Left = new RayIntersectorKDTreeNode(LeftTriangleIndices, Root, Depth + 1, LeafTriangles);
        _Interior.Right = new RayIntersectorKDTreeNode(RightTriangleIndices, Root, Depth + 1, LeafTriangles);
    }
    else
    {
        MakeLeafNode(TriangleIndices, LeafTriangles);
    }
}

Rectangle3f RayIntersectorKDTreeNode::ComputeBBox(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root) const
{
    Rectangle3f BBox;
    for(UINT TriangleIndex = 0; TriangleIndex < TriangleIndices.Length(); TriangleIndex++)
    {
        const UINT BaseIndexIndex = TriangleIndices[TriangleIndex] * 3;
        for(UINT VertexIndex = 0; VertexIndex < 3; VertexIndex++)
        {
            const Vec3f &CurVertex = Root._Vertices[Root._Indices[BaseIndexIndex + VertexIndex]];
            if(TriangleIndex == 0 && VertexIndex == 0)
            {
                BBox = Rectangle3f(CurVertex, CurVertex);
            }
            else
            {
                BBox.Min = Vec3f::Minimize(BBox.Min, CurVertex);
                BBox.Max = Vec3f::Maximize(BBox.Max, CurVertex);
            }
        }
    }
    return BBox;
}

bool RayIntersectorKDTreeNode::FindBestSplit(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT &Axis, float &SplitValue) const
{
    Rectangle3f BBox = ComputeBBox(TriangleIndices, Root);
    int BestAxis = -1;
    UINT BestChildImprovement = 8;
    for(UINT CurAxis = 0; CurAxis < 3; CurAxis++)
    {
        UINT LeftCount, RightCount;
        TestSplit(TriangleIndices, Root, CurAxis, BBox, LeftCount, RightCount);
        
        /*const UINT BestChildCost = TriangleIndices.Length();
        const UINT WorstChildCost = TriangleIndices.Length() * 2;
        const UINT ActualChildCost = LeftCount + RightCount;
        const float CurAxisValue = float(ActualChildCost) / float(WorstChildCost);*/
        const UINT CurChildImprovement = Math::Min(TriangleIndices.Length() - LeftCount, TriangleIndices.Length() - RightCount);
        if(CurChildImprovement > BestChildImprovement && LeftCount > 0 && RightCount > 0)
        {
            BestAxis = CurAxis;
            BestChildImprovement = CurChildImprovement;
        }
    }
    if(BestAxis == -1)
    {
        return false;
    }
    else
    {
        Axis = BestAxis;
        SplitValue = BBox.Center()[Axis];
        return true;
    }
}

void RayIntersectorKDTreeNode::TestSplit(const Vector<UINT> &TriangleIndices, const RayIntersectorKDTree &Root, UINT Axis, const Rectangle3f &BBox, UINT &LeftCount, UINT &RightCount) const
{
    float SplitPoint = BBox.Center()[Axis];
    LeftCount = 0;
    RightCount = 0;
    for(UINT TriangleIndex = 0; TriangleIndex < TriangleIndices.Length(); TriangleIndex++)
    {
        UINT LeftVertices = 0, RightVertices = 0;
        const UINT BaseIndexIndex = TriangleIndices[TriangleIndex] * 3;
        for(UINT VertexIndex = 0; VertexIndex < 3; VertexIndex++)
        {
            const Vec3f &CurVertex = Root._Vertices[Root._Indices[BaseIndexIndex + VertexIndex]];
            if(CurVertex[Axis] < SplitPoint)
            {
                LeftVertices++;
            }
            if(CurVertex[Axis] >= SplitPoint)
            {
                RightVertices++;
            }
        }
        bool OnLeftSide = (LeftVertices > RightVertices);
        bool OnRightSide = !OnLeftSide;
        if(OnLeftSide)
        {
            LeftCount++;
        }
        if(OnRightSide)
        {
            RightCount++;
        }
    }
}

RayIntersectorKDTree::RayIntersectorKDTree()
{
    _Vertices = NULL;
    _Indices = NULL;
    _LeafTriangles = NULL;
    _VertexCount = 0;
    _TriangleCount = 0;
    _LeafTriangleCount = 0;
}

RayIntersectorKDTree::~RayIntersectorKDTree()
{
    FreeMemory();
}

void RayIntersectorKDTree::FreeMemory()
{
    _VertexCount = 0;
    _TriangleCount = 0;
    _LeafTriangleCount = 0;
    if(_Vertices)
    {
        delete _Vertices;
        _Vertices = NULL;
    }
    if(_Indices)
    {
        delete _Indices;
        _Indices = NULL;
    }
    if(_LeafTriangles)
    {
        delete _LeafTriangles;
        _LeafTriangles = NULL;
    }
}

void RayIntersectorKDTree::Init(const BaseMesh &M)
{
    Console::WriteString("Building RayIntersectorKDTree...");
    FreeMemory();

    _VertexCount = M.VertexCount();
    _TriangleCount = M.FaceCount();

    Console::WriteString(String(_TriangleCount) + String(" base triangles, "));
    

    _Vertices = new Vec3f[_VertexCount];
    _Indices = new UINT[_TriangleCount * 3];
    
    const MeshVertex *MeshVertices = M.Vertices();
    const DWORD *MeshIndices = M.Indices();
    
    for(UINT VertexIndex = 0; VertexIndex < _VertexCount; VertexIndex++)
    {
        _Vertices[VertexIndex] = MeshVertices[VertexIndex].Pos;
    }

    for(UINT IndexIndex = 0; IndexIndex < _TriangleCount * 3; IndexIndex++)
    {
        _Indices[IndexIndex] = MeshIndices[IndexIndex];
    }

    Vector<UINT> TriangleIndices(_TriangleCount);
    for(UINT TriangleIndex = 0; TriangleIndex < _TriangleCount; TriangleIndex++)
    {
        TriangleIndices[TriangleIndex] = TriangleIndex;
    }
    
    Vector<UINT> LeafTrianglesVector;
    _Root = new RayIntersectorKDTreeNode(TriangleIndices, *this, 0, LeafTrianglesVector);
    _LeafTriangleCount = LeafTrianglesVector.Length();
    _LeafTriangles = new UINT[_LeafTriangleCount];
    memcpy(_LeafTriangles, LeafTrianglesVector.CArray(), sizeof(UINT) * _LeafTriangleCount);
    Console::WriteLine(String(_LeafTriangleCount) + String(" leaf triangles."));
}

bool RayIntersectorKDTree::FindFirstIntersect(const Ray3D &R, Vec3f &IntersectPt) const
{
    bool HitFound = false;
    _Root->FindFirstIntersect(R, IntersectPt, HitFound, *this);
    return HitFound;
}
