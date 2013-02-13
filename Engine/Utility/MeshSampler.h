
struct MeshSample
{
    Vec3f pos;
    Vec3f normal;
};

struct MeshUVSample
{
    UINT meshIndex;
    UINT triangleIndex;
    Vec2f uv;
    Vec3f pos;
    Vec3f normal;
};

class MeshSampler
{
public:
    static void Sample(const BaseMesh &m, UINT sampleCount, Vector<MeshSample> &samples);
    static void Sample(const Vector< pair<const BaseMesh *, Matrix4> > &geometry, UINT sampleCount, Vector<MeshSample> &samples);

    static void Sample(const BaseMesh &m, UINT& sampleCount, float densityThresh, const Vec3f &direction, float targetOrientation, float orientThresh, Vector<MeshUVSample> &samples);
    static void Sample(const Vector< pair<const BaseMesh *, Matrix4> > &geometry, UINT sampleCount, float densityThresh, const Vec3f &direction, float targetOrientation, float orientThresh, Vector<MeshUVSample> &samples);

private:
    static double DirectionalSurfaceArea(const BaseMesh &m, const Vec3f &direction, float targetOrientation, float orientThresh);
    static float GetTriangleArea(const BaseMesh &m, UINT triangleIndex);
    static Vec3f GetTriangleNormal(const BaseMesh &m, UINT triangleIndex);
    static MeshSample SampleTriangle(const BaseMesh &m, UINT triangleIndex, double sampleValue);
    static MeshUVSample SampleTriangleUV(const BaseMesh &m, UINT triangleIndex, double sampleValue);
    static Vec2f StratifiedSample2D(double s, UINT depth = 0);
};