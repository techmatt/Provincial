/*
KMeansClusteringKDTree.h
Written by Matthew Fisher
*/

#ifdef USE_ANN
#ifdef USE_KDTREE

struct KMeansClusterKDTree
{
public:
    void Init(UINT dimensionCount, const float *start);
    void FinishIteration(UINT dimensionCount, const float *fallback);

    __forceinline void AddEntry(const float *entry)
    {
        entries.PushEnd(entry);
    }

    VecNf center;
    Vector<const float*> entries;
    float assignmentError;
};

class KMeansClusteringKDTree
{
public:
    void Cluster(const Vector<const float*> &elements, UINT dimensionCount, UINT clusterCount, UINT maxIterations = 0, double maxDelta = 0.0, bool verbose = true);
    __forceinline const VecNf& ClusterCenter(UINT clusterIndex)
    {
        return _clusters[clusterIndex].center;
    }
    __forceinline UINT ClusterCount()
    {
        return _clusters.Length();
    }
    __forceinline const VecNf& QuantizeToNearestClusterCenter(const float *element)
    {
        return _clusters[QuantizeToNearestClusterIndex(element)].center;
    }
    UINT QuantizeToNearestClusterIndex(const float *element);

private:
    void UpdateClusterTree();
    void Iterate(const Vector<const float*> &elements);

    UINT _dimensionCount;
    KDTree _clusterTree;
    Vector< KMeansClusterKDTree > _clusters;
    mutable Vector<UINT> _indicesStorage;
};

#endif
#endif