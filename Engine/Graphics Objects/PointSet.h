/*
PointSet.h
Written by Matthew Fisher

Point set is informationa about a collection of points and possibly normal information.
*/

struct SamplePoint
{
    SamplePoint() {}
    SamplePoint(const Vec3f &_Position, const Vec3f &_Normal)
    {
        Position = _Position;
        Normal = _Normal;
    }
    Vec3f Position;
    Vec3f Normal;
};

struct EstimateNormalsVertexInfo
{
    float FitError;
    Plane FitPlane;
};

class PointSet
{
public:
    PointSet();
    PointSet(const PointSet &P);
    ~PointSet();
    void FreeMemory();

    PointSet& operator = (const PointSet &P);

    //
    // Accessors
    //
    __forceinline Vector<SamplePoint>& Points()
    {
        return _Points;
    }
    __forceinline const Vector<SamplePoint>& Points() const
    {
        return _Points;
    }
#ifdef USE_KDTREE
    __forceinline KDTree3& KDTree()
    {
        if(_KDTreeDirty)
        {
            UpdateKDTree();
        }
        return _KDTree;
    }
#endif


    //
    // Load from & Save to files and objects
    //
    void LoadFromPositions(const Vector<Vec3f> &Positions);
    void LoadFromMesh(const BaseMesh &M);
    void LoadFromMesh(const ComplexMesh &M);
    void LoadFromTextFile(const String &Filename, bool ExpectNormals);
    void LoadFromMaksFile(const String &Filename);
    void SaveToTextFile(const String &Filename, bool DumpNormals = true) const;
    void SaveToBnpts(const String &Filename) const;

    //
    // Modifiers
    //
    void AddPoints(const Vector<SamplePoint> &NewPoints);
    void CullToRectangle(const Rectangle3f &R);
    void RandomReduce(UINT NewPointCount);
    
    //
    // Query
    //
    float Radius();
    Vec3f Center();

    //
    // Transforms
    //
    void Transform(const Matrix4 &Transform);
    void Translate(const Vec3f &Translation);
    void Scale(const Vec3f &ScaleFactors);
    __forceinline void Scale(float ScaleFactor)
    {
        Scale(Vec3f(ScaleFactor, ScaleFactor, ScaleFactor));
    }
    void CenterPoints();
    void MakeUnitSphere();

    //
    // Utility
    //
    void CreatePointMesh(BaseMesh &M, float PointRadius, bool RenderNormals);
    void ColorPointMesh(BaseMesh &M, RGBColor Color);
    void ColorPointMesh(BaseMesh &M, const Vector<RGBColor> &Colors);
    void DumpToVector(Vector<Vec3f> &V);

#ifdef USE_KDTREE
    void EstimateNormals(UINT k);
    void OrientNormals(const Plane &CameraViewPlane);
    void EstimateNormals(UINT k, Vector<EstimateNormalsVertexInfo> &FitInfo, bool RecordInfo);
    float EstimateKNearestDistance(UINT k);
#endif

    void UpdateKDTree();

private:
    Vector<SamplePoint> _Points;
    bool                _KDTreeDirty;
#ifdef USE_KDTREE
    KDTree3             _KDTree;
#endif
};
