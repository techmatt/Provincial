/*
KMeansClusteringFloat.h
Written by Matthew Fisher
*/

template<UINT dimension>
class KMeansClusterFloat
{
public:
    void Init(const float *start)
    {
        memcpy(_centroid, start, sizeof(float) * dimension);
    }
    void UpdateCentroid(const float *fallbackElement)
    {
        const UINT entryCount = _entries.Length();
        if(entryCount == 0)
        {
            memcpy(_centroid, fallbackElement, sizeof(float) * dimension);
        }
        else
        {
            memcpy(_centroid, _entries[0], sizeof(float) * dimension);
            for(UINT entryIndex = 1; entryIndex < entryCount; entryIndex++)
            {
                const float *curEntry = _entries[entryIndex];
                for(UINT dimensionIndex = 0; dimensionIndex < dimension; dimensionIndex++) _centroid[dimensionIndex] += curEntry[dimensionIndex];
            }
            const float scale = 1.0f / entryCount;
            for(UINT dimensionIndex = 0; dimensionIndex < dimension; dimensionIndex++) _centroid[dimensionIndex] *= scale;
            _entries.FreeMemory();
        }
    }
    __forceinline const float* Centroid() const
    {
        return _centroid;
    }
    __forceinline void AddEntry(const float *entry)
    {
        _entries.PushEnd(entry);
    }

private:
    float _centroid[dimension];
    Vector<const float*> _entries;
};

struct KMeansClusteringTierEntry
{
    KMeansClusteringTierEntry() {}
    KMeansClusteringTierEntry(UINT _clusters, UINT _maxIterations)
    {
        clusters = _clusters;
        maxIterations = _maxIterations;
    }
    UINT clusters;
    UINT maxIterations;
};

template<UINT dimension>
class KMeansClusteringFloat
{
public:
    void Cluster(const Vector<const float*> &points, UINT clusterCount, UINT iterationCount, bool verbose = true)
    {
        if(verbose)
        {
            Console::WriteLine(String("K-means clustering float, ") + String(points.Length()) + String (" points, ") + String(clusterCount) + String(" clusters"));
        }
        _clusters.Allocate(clusterCount);
        for(UINT ClusterIndex = 0; ClusterIndex < clusterCount; ClusterIndex++)
        {
            _clusters[ClusterIndex].Init(points.RandomElement());
        }
        for(UINT iteration = 0; iteration < iterationCount; iteration++)
        {
            Iterate(points);
        }
    }

    void Cluster(const Vector<const float*> &points, Vector<KMeansClusteringTierEntry> &tierInfo, bool verbose = true)
    {
        if(verbose)
        {
            Console::WriteLine(String("K-means clustering tiered, ") + String(points.Length()) + String (" points, ") + String(tierInfo.Length()) + String(" tiers"));
        }

        UINT clusterCount = 1;
        for(UINT tierIndex = 0; tierIndex < tierInfo.Length(); tierIndex++)
        {
            clusterCount *= tierInfo[tierIndex].clusters;
        }

        _clusters.Reserve(clusterCount);

        UINT resultIndex = 0;
        ClusterTier(points, tierInfo, 0, _clusters, resultIndex, verbose);
    }

    __forceinline const float* ClusterCentroid(UINT index)
    {
        return _clusters[index].Centroid();
    }

    __forceinline const float* QuantizeToNearestClusterCenter(const float *element)
    {
        return _clusters[QuantizeToNearestClusterCenter(element)].Center();
    }

    __forceinline UINT QuantizeToNearestClusterIndex(const float *element)
    {
        UINT closestClusterIndex = 0;
        double closestClusterDist = Dist(element, _clusters[0].Centroid());
        for(UINT clusterIndex = 1; clusterIndex < _clusters.Length(); clusterIndex++)
        {
            double curClusterDist = Dist(element, _clusters[clusterIndex].Centroid());
            if(curClusterDist < closestClusterDist)
            {
                closestClusterIndex = clusterIndex;
                closestClusterDist = curClusterDist;
            }
        }
        return closestClusterIndex;
    }

    __forceinline UINT ClusterCount()
    {
        return _clusters.Length();
    }

private:
    __forceinline float Dist(const float *a, const float *b)
    {
        float result = 0.0f;
        for(UINT index = 0; index < dimension; index++)
        {
            const float diff = a[index] - b[index];
            result += diff * diff;
        }
        return result;
    }

    void ClusterTier(const Vector<const float*> &points, Vector<KMeansClusteringTierEntry> &tierInfo, UINT tierLevel, Vector< KMeansClusterFloat<dimension> > &result, UINT &resultIndex, bool verbose)
    {
        if(verbose)
        {
            Console::WriteLine(String("K-means tier ") + String(tierLevel) + String(", ") + String(points.Length()) + String (" points, ") + String(tierInfo.Length()) + String(" tiers"));
        }

        const KMeansClusteringTierEntry &curTier = tierInfo[tierLevel];

        KMeansClusteringFloat<dimension> children;
        children.Cluster(points, curTier.clusters, curTier.maxIterations, verbose);

        if(tierLevel + 1 == tierInfo.Length())
        {
            for(UINT childIndex = 0; childIndex < curTier.clusters; childIndex++)
            {
                result[resultIndex++].Init(children.ClusterCentroid(childIndex));
            }
        }
        else
        {
            Vector< Vector<const float *> > childrenPoints(curTier.clusters);
            for(UINT pointIndex = 0; pointIndex < points.Length(); pointIndex++)
            {
                const float *curPoint = points[pointIndex];
                childrenPoints[children.QuantizeToNearestClusterIndex(curPoint)].PushEnd(curPoint);
            }

            for(UINT childIndex = 0; childIndex < curTier.clusters; childIndex++)
            {
                if(childrenPoints[childIndex].Length() == 0) childrenPoints[childIndex] = points; //hopefully this rarely happens
                ClusterTier(childrenPoints[childIndex], tierInfo, tierLevel + 1, result, resultIndex, verbose);
            }
        }
    }

    void Iterate(const Vector<const float*> &elements)
    {
        const UINT elementCount = elements.Length();
        auto elementArray = elements.CArray();
        const UINT clusterCount = _clusters.Length();
        KMeansClusterFloat<dimension> *clusterArray = _clusters.CArray();
        for(UINT elementIndex = 0; elementIndex < elementCount; elementIndex++)
        {
            const float *curElement = elementArray[elementIndex];
            UINT closestClusterIndex = 0;
            double closestClusterDist = Dist(curElement, clusterArray[0].Centroid());
            for(UINT clusterIndex = 1; clusterIndex < clusterCount; clusterIndex++)
            {
                double curClusterDist = Dist(curElement, clusterArray[clusterIndex].Centroid());
                if(curClusterDist < closestClusterDist)
                {
                    closestClusterIndex = clusterIndex;
                    closestClusterDist = curClusterDist;
                }
            }
            clusterArray[closestClusterIndex].AddEntry(curElement);
        }
        for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
        {
            clusterArray[clusterIndex].UpdateCentroid(elements.RandomElement());
        }
    }

    Vector< KMeansClusterFloat<dimension> > _clusters;
};
