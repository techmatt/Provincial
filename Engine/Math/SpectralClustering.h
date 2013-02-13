class SpectralClusteringCache
{
private:
    friend class SpectralClustering;

    Vector<double> eigenvalues;
    DenseMatrix<double> eigenvectors;
};

class SpectralClustering
{
public:
    enum Method
    {
        Unnormalized,
        RandomWalk,
        Symmetric,
    };

    static void MakeSimilarityMatrix(Vector<float*> &points, UINT dimension, float sigma, bool zeroDiagonal, DenseMatrix<double> &result);
    static void Cluster(const DenseMatrix<double> &S, UINT kMeansClusters, UINT reducedDimensionCount, Method method, Vector<UINT> &clusterIDs, SpectralClusteringCache *cache = NULL);
    
private:
    static void MakeLaplacian(const DenseMatrix<double> &S, Method method, DenseMatrix<double> &L);
};
