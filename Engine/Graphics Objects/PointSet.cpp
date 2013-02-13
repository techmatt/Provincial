/*
PointSet.cpp
Written by Matthew Fisher

Point set is informationa about a collection of points and possibly normal information.
*/

PointSet::PointSet()
{
    _KDTreeDirty = true;
}

PointSet::PointSet(const PointSet &P)
{
    _KDTreeDirty = true;
    _Points = P._Points;
}

PointSet::~PointSet()
{
    FreeMemory();
}

PointSet& PointSet::operator = (const PointSet &P)
{
    _KDTreeDirty = true;
#ifdef USE_KDTREE
    _KDTree.FreeMemory();
#endif
    _Points = P._Points;
    return *this;
}

void PointSet::FreeMemory()
{
    _Points.FreeMemory();
#ifdef USE_KDTREE
    _KDTree.FreeMemory();
#endif
    _KDTreeDirty = true;
}

void PointSet::DumpToVector(Vector<Vec3f> &V)
{
    V.Allocate(_Points.Length());
    for(UINT PointIndex = 0; PointIndex < V.Length(); PointIndex++)
    {
        V[PointIndex] = _Points[PointIndex].Position;
    }
}

void PointSet::AddPoints(const Vector<SamplePoint> &NewPoints)
{
    for(UINT NewPointIndex = 0; NewPointIndex < NewPoints.Length(); NewPointIndex++)
    {
        _Points.PushEnd(NewPoints[NewPointIndex]);
    }
    _KDTreeDirty = true;
}

void PointSet::LoadFromPositions(const Vector<Vec3f> &Positions)
{
    FreeMemory();
    UINT VertexCount = Positions.Length();
    _Points.Allocate(VertexCount);
    for(UINT i = 0; i < VertexCount; i++)
    {
        _Points[i] = SamplePoint(Positions[i], Vec3f::Origin);
    }
    _KDTreeDirty = true;
}

void PointSet::LoadFromMesh(const ComplexMesh &M)
{
    FreeMemory();
    UINT VertexCount = M.Vertices().Length();
    const Vertex *Vertices = M.Vertices().CArray();
    _Points.Allocate(VertexCount);
    for(UINT i = 0; i < VertexCount; i++)
    {
        _Points[i] = SamplePoint(Vertices[i].Pos(), Vertices[i].AreaWeightedNormal());
    }
    _KDTreeDirty = true;
}

void PointSet::LoadFromMesh(const BaseMesh &M)
{
    FreeMemory();
    UINT VertexCount = M.VertexCount();
    const MeshVertex *Vertices = M.Vertices();
    _Points.Allocate(VertexCount);
    for(UINT i = 0; i < VertexCount; i++)
    {
        _Points[i] = SamplePoint(Vertices[i].Pos, Vertices[i].Normal);
    }
    _KDTreeDirty = true;
}

void PointSet::LoadFromMaksFile(const String &Filename)
{
    FreeMemory();
    ifstream File(Filename.CString());
    Vector<Vec3f> PointsOnly;
    Vector<String> Partition;
    while(!File.fail())
    {
        String CurLine = Utility::GetNextLine(File);
        CurLine.Partition(' ', Partition);
        if(Partition.Length() == 4 && Partition[0].ConvertToDouble() != 0.0f && Partition[1].ConvertToDouble() != 0.0f)
        {
            Vec3f NewPoint(Partition[0].ConvertToFloat(), Partition[1].ConvertToFloat(), Partition[2].ConvertToFloat());
            PointsOnly.PushEnd(NewPoint);
            _Points.PushEnd(SamplePoint(NewPoint, Vec3f::eX));
        }
    }

    Plane P;
    float ResiudalError;
    P.FitToPoints(PointsOnly, ResiudalError);
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        _Points[i].Normal = Vec3f::eZ;//P.Normal();
    }
    _KDTreeDirty = true;
}

void PointSet::LoadFromTextFile(const String &Filename, bool ExpectNormals)
{
    FreeMemory();
    ifstream File(Filename.CString());
    Vector<Vec3f> PointsOnly;
    while(!File.fail())
    {
        SamplePoint NewPoint;
        File >> NewPoint.Position.x >> NewPoint.Position.y >> NewPoint.Position.z;
        if(ExpectNormals)
        {
            File >> NewPoint.Normal.x >> NewPoint.Normal.y >> NewPoint.Normal.z;
        }
        if(!File.fail())
        {
            if(ExpectNormals)
            {
                NewPoint.Normal = Vec3f::Normalize(NewPoint.Normal);
            }
            else
            {
                PointsOnly.PushEnd(NewPoint.Position);
            }
            _Points.PushEnd(NewPoint);
        }
    }

    if(!ExpectNormals)
    {
        Plane P;
        float ResiudalError;
        P.FitToPoints(PointsOnly, ResiudalError);
        for(UINT i = 0; i < _Points.Length(); i++)
        {
            _Points[i].Normal = P.Normal();
        }
    }
    _KDTreeDirty = true;
}

void PointSet::SaveToTextFile(const String &Filename, bool DumpNormals) const
{
    ofstream File(Filename.CString());
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        File << _Points[i].Position.TabSeparatedString();
        if(DumpNormals)
        {
            File << '\t' << _Points[i].Normal.TabSeparatedString();
        }
        File << endl;
    }
}

void PointSet::SaveToBnpts(const String &Filename) const
{
    FILE *File = Utility::CheckedFOpen(Filename.CString(), "wb");
    Utility::CheckedFWrite(_Points.CArray(), sizeof(SamplePoint), _Points.Length(), File);
    fclose(File);
}

void PointSet::Transform(const Matrix4 &Transform)
{
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        _Points[i].Position = Transform.TransformPoint(_Points[i].Position);
        _Points[i].Normal = Transform.TransformNormal(_Points[i].Normal);
    }
    _KDTreeDirty = true;
}

void PointSet::Translate(const Vec3f &Translation)
{
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        _Points[i].Position += Translation;
    }
    _KDTreeDirty = true;
}

void PointSet::Scale(const Vec3f &ScaleFactors)
{
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        Vec3f &CurPosition = _Points[i].Position;
        CurPosition = Vec3f(CurPosition.x * ScaleFactors.x, 
                           CurPosition.y * ScaleFactors.y, 
                           CurPosition.z * ScaleFactors.z);
    }
    _KDTreeDirty = true;
}

void PointSet::CenterPoints()
{
    Translate(-Center());
}

Vec3f PointSet::Center()
{
    Vec3f Result = Vec3f::Origin;
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        Result += _Points[i].Position;
    }
    return (Result / float(_Points.Length()));
}

float PointSet::Radius()
{
    float MaxDistance = 0.0f;
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        float CurDistance = _Points[i].Position.Length();
        if(CurDistance > MaxDistance)
        {
            MaxDistance = CurDistance;
        }
    }
    return MaxDistance;
}

void PointSet::MakeUnitSphere()
{
    CenterPoints();
    Scale(1.0f / Radius());
}

void PointSet::CreatePointMesh(BaseMesh &M, float PointRadius, bool RenderNormals)
{
    Mesh BaseSphere, BaseCylinder;
    BaseSphere.CreateBox(1.0f, 1.0f, 1.0f, 0);
    BaseSphere.SetRadius(PointRadius);
    if(RenderNormals)
    {
        BaseCylinder.CreateCylinder(PointRadius * 0.3f, PointRadius * 4.0f, 5, 1);
    }
    M.Allocate(_Points.Length() * (BaseSphere.VertexCount() + BaseCylinder.VertexCount()), _Points.Length() * (BaseSphere.FaceCount() + BaseCylinder.FaceCount()));
    MeshVertex *Vertices = M.Vertices();
    DWORD *Indices = M.Indices();
    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        UINT StartVertex = PointIndex * BaseSphere.VertexCount();
        for(UINT VertexIndex = 0; VertexIndex < BaseSphere.VertexCount(); VertexIndex++)
        {
            MeshVertex &SourceVertex = BaseSphere.Vertices()[VertexIndex];
            MeshVertex &DestVertex = Vertices[VertexIndex + StartVertex];
            DestVertex = SourceVertex;
            DestVertex.Pos += CurPoint.Position;
            DestVertex.Color = RGBColor(CurPoint.Normal * 0.5f + Vec3f(0.5f, 0.5f, 0.5f));
        }
        for(UINT FaceIndex = 0; FaceIndex < BaseSphere.FaceCount(); FaceIndex++)
        {
            for(UINT i = 0; i < 3; i++)
            {
                Indices[(FaceIndex + PointIndex * BaseSphere.FaceCount()) * 3 + i] = BaseSphere.Indices()[FaceIndex * 3 + i] + StartVertex;
            }
        }
    }
}

void PointSet::ColorPointMesh(BaseMesh &M, const Vector<RGBColor> &Colors)
{
    Mesh BaseSphere;
    //BaseSphere.CreateSphere(1.0f, 0);
    BaseSphere.CreateBox(1.0f, 1.0f, 1.0f, 0);
    MeshVertex *Vertices = M.Vertices();
    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        UINT StartVertex = PointIndex * BaseSphere.VertexCount();
        for(UINT VertexIndex = 0; VertexIndex < BaseSphere.VertexCount(); VertexIndex++)
        {
            MeshVertex &DestVertex = Vertices[VertexIndex + StartVertex];
            DestVertex.Color = Colors[PointIndex];
        }
    }
}

void PointSet::ColorPointMesh(BaseMesh &M, RGBColor Color)
{
    Mesh BaseSphere;
    BaseSphere.CreateBox(1.0f, 1.0f, 1.0f, 0);
    MeshVertex *Vertices = M.Vertices();
    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        UINT StartVertex = PointIndex * BaseSphere.VertexCount();
        for(UINT VertexIndex = 0; VertexIndex < BaseSphere.VertexCount(); VertexIndex++)
        {
            Vertices[VertexIndex + StartVertex].Color = Color;
        }
    }
}

#ifdef USE_KDTREE
void PointSet::OrientNormals(const Plane &CameraViewPlane)
{
    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        if(Plane::DotNormal(CameraViewPlane, CurPoint.Normal) < 0.0f)
        {
            CurPoint.Normal = -CurPoint.Normal;
        }
    }
}

void PointSet::EstimateNormals(UINT k)
{
    Vector<EstimateNormalsVertexInfo> FitInfo;
    EstimateNormals(k, FitInfo, false);
}

void PointSet::EstimateNormals(UINT k, Vector<EstimateNormalsVertexInfo> &FitInfo, bool RecordInfo)
{
    Console::WriteLine("Estimating normals...");
    PersistentAssert(_Points.Length() > 0, "No points in point set");
    UpdateKDTree();
    Vector<UINT> Result;
    Vector<Vec4f> PlaneFitPoints(k + 1);

    if(RecordInfo)
    {
        FitInfo.Allocate(_Points.Length());
    }

    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        _KDTree.KNearest(CurPoint.Position, k, Result, 0.0f);

        Plane P;
        Vec3f Basis1, Basis2;
        float h2 = (CurPoint.Position - _Points[Result[k - 1]].Position).LengthSq() / 3.0f;
        float Factor = -1.0f / h2;
        for(UINT NeighborIndex = 0; NeighborIndex < k; NeighborIndex++)
        {
            //float Weight = 1.0f;
            Vec3f &OtherPosition = _Points[Result[NeighborIndex]].Position;
            float Weight = expf((OtherPosition - CurPoint.Position).LengthSq() * Factor);
            PlaneFitPoints[NeighborIndex] = Vec4f(OtherPosition, Weight);
        }
        PlaneFitPoints[k] = Vec4f(CurPoint.Position, exp(0.0f));
        float FitError, NormalEigenvalue;
        P.FitToPoints(PlaneFitPoints, Basis1, Basis2, NormalEigenvalue, FitError);
        if(Vec3f::Dot(CurPoint.Normal, P.Normal()) < 0.0f)
        {
            P = P.Flip();
        }
        CurPoint.Normal = P.Normal();
        if(RecordInfo)
        {
            FitInfo[PointIndex].FitError = FitError;
            FitInfo[PointIndex].FitPlane = P;
        }
    }
    Console::WriteLine("done estimating normals.");
}

float PointSet::EstimateKNearestDistance(UINT k)
{
    UpdateKDTree();
    Vector<UINT> Result;

    float DistanceSum = 0.0f;
    for(UINT PointIndex = 0; PointIndex < _Points.Length(); PointIndex++)
    {
        SamplePoint &CurPoint = _Points[PointIndex];
        _KDTree.KNearest(CurPoint.Position, k, Result, 0.0f);
        DistanceSum += (_Points[Result.Last()].Position - CurPoint.Position).Length();
    }
    return (DistanceSum / float(_Points.Length())); 
}
#endif

void PointSet::RandomReduce(UINT NewPointCount)
{
    _Points.Randomize();
    if(_Points.Length() > NewPointCount)
    {
        _Points.ReSize(NewPointCount);
    }
    _KDTreeDirty = true;
}

void PointSet::CullToRectangle(const Rectangle3f &R)
{
    Vector<SamplePoint> NewPoints;
    NewPoints.Reserve(_Points.Length());
    for(UINT i = 0; i < _Points.Length(); i++)
    {
        if(Vec3f::WithinRect(_Points[i].Position, R))
        {
            NewPoints.PushEnd(_Points[i]);
        }
    }
    _Points = NewPoints;
    _KDTreeDirty = true;
}

void PointSet::UpdateKDTree()
{
    _KDTreeDirty = false;
#ifdef USE_KDTREE
    _KDTree.BuildTree(*this);
#endif
}
