/*
KMeansClustering.h
Written by Matthew Fisher
*/

class Vec3fKMeansMetric
{
public:
    static __forceinline float Dist(const Vec3f &L, const Vec3f &R)
    {
        return Vec3f::DistSq(L, R);
    }
};

class Vec2fKMeansMetric
{
public:
    static __forceinline float Dist(const Vec2f &L, const Vec2f &R)
    {
        return Vec2f::DistSq(L, R);
    }
};

class VecNfKMeansMetric
{
public:
    static __forceinline float Dist(const VecNf &L, const VecNf &R)
    {
        return VecNf::DistSq(L, R);
    }
};

template<class T>
class KMeansCluster
{
public:
    void Init(const T &Start)
    {
        _Center = Start;
    }
    void FinishIteration(const T &FallbackElement)
    {
        if(_Entries.Length() == 0)
        {
            _Center = FallbackElement;
        }
        else
        {
            float Sum = _Entries[0].second;
            T NewCenter = _Entries[0].first;
            for(UINT EntryIndex = 1; EntryIndex < _Entries.Length(); EntryIndex++)
            {
                Sum += _Entries[EntryIndex].second;
                NewCenter += _Entries[EntryIndex].first;
            }
            _Center = NewCenter;
            if(Sum > 0.0f)
            {
                _Center *= (1.0f / Sum);
            }
            _Entries.FreeMemory();
        }
    }
    __forceinline const T& Center() const
    {
        return _Center;
    }
    __forceinline void AddEntry(const T &PreWeightedEntry, float Weight)
    {
        _Entries.PushEnd( pair<T, float>(PreWeightedEntry, Weight) );
    }
    __forceinline void AddEntry(const T &Entry)
    {
        _Entries.PushEnd( pair<T, float>(Entry, 1.0f) );
    }

private:
    T _Center;
    Vector< pair<T, float> > _Entries;
};

template<class T, class Metric>
class KMeansClustering
{
public:
    void Cluster(const Vector<T> &Elements, UINT ClusterCount, UINT MaxIterations = 0, bool Verbose = true, double MaxDelta = 0.0)
    {
        if(Verbose)
        {
            Console::WriteLine(String("K-means clustering, ") + String(Elements.Length()) + String (" points, ") + String(ClusterCount) + String(" clusters"));
            Console::AdvanceLine();
        }
        PersistentAssert(Elements.Length() >= ClusterCount, "Too many clusters");
        _Clusters.Allocate(ClusterCount);
        for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
        {
            _Clusters[ClusterIndex].Init(Elements.RandomElement());
        }

        UINT PassIndex = 0;
        Vector<T> PreviousClusterCenters(_Clusters.Length());
        bool Converged = false;
        while(!Converged)
        {
            PassIndex++;
            for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                PreviousClusterCenters[ClusterIndex] = _Clusters[ClusterIndex].Center();
            }

            Iterate(Elements);

            double Delta = 0.0;
            for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                Delta += Metric::Dist(PreviousClusterCenters[ClusterIndex], _Clusters[ClusterIndex].Center());
            }
            Converged = (Delta <= MaxDelta || PassIndex == MaxIterations);
            if(Verbose)
            {
                Console::OverwriteLine(String("Pass ") + String(PassIndex) + String(", ") + String("Delta=") + String(Delta));
            }
        }
    }
    void Cluster(const Vector<T> &Elements, const Vector<float> &Weights, UINT ClusterCount, UINT MaxIterations = 0, bool Verbose = true)
    {
        if(Verbose)
        {
            Console::WriteLine(String("Weighted K-means clustering, ") + String(Elements.Length()) + String (" points, ") + String(ClusterCount) + String(" clusters"));
            Console::AdvanceLine();
        }
        PersistentAssert(Elements.Length() >= ClusterCount, "Too many clusters");
        PersistentAssert(Elements.Length() == Weights.Length(), "Incorrect number of weights");
        _Clusters.Allocate(ClusterCount);
        for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
        {
            _Clusters[ClusterIndex].Init(Elements.RandomElement());
        }

        Vector<T> WeightedElements = Elements;
        for(UINT ElementIndex = 0; ElementIndex < Elements.Length(); ElementIndex++)
        {
            WeightedElements[ElementIndex] *= Weights[ElementIndex];
        }

        UINT PassIndex = 0;
        Vector<T> PreviousClusterCenters(_Clusters.Length());
        bool Converged = false;
        while(!Converged)
        {
            PassIndex++;
            for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                PreviousClusterCenters[ClusterIndex] = _Clusters[ClusterIndex].Center();
            }

            Iterate(Elements, WeightedElements, Weights);

            double Delta = 0.0;
            for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                Delta += Metric::Dist(PreviousClusterCenters[ClusterIndex], _Clusters[ClusterIndex].Center());
            }
            Converged = (Delta == 0.0 || PassIndex == MaxIterations);
            if(Verbose)
            {
                Console::OverwriteLine(String("Pass ") + String(PassIndex) + String(", ") + String("Delta=") + String(Delta));
            }
        }
    }
    __forceinline const T& ClusterCenter(UINT Index)
    {
        return _Clusters[Index].Center();
    }
    __forceinline const T& QuantizeToNearestClusterCenter(const T &Element)
    {
        return _Clusters[QuantizeToNearestClusterCenter(Element)].Center();
    }
    __forceinline UINT QuantizeToNearestClusterIndex(const T &Element)
    {
        UINT ClosestClusterIndex = 0;
        double ClosestClusterDist = Metric::Dist(Element, _Clusters[0].Center());
        for(UINT ClusterIndex = 1; ClusterIndex < _Clusters.Length(); ClusterIndex++)
        {
            double CurClusterDist = Metric::Dist(Element, _Clusters[ClusterIndex].Center());
            if(CurClusterDist < ClosestClusterDist)
            {
                ClosestClusterIndex = ClusterIndex;
                ClosestClusterDist = CurClusterDist;
            }
        }
        return ClosestClusterIndex;
    }
    __forceinline UINT ClusterCount()
    {
        return _Clusters.Length();
    }

private:
    void Iterate(const Vector<T> &Elements)
    {
        const UINT ElementCount = Elements.Length();
        const T* ElementCArray = Elements.CArray();
        const UINT ClusterCount = _Clusters.Length();
        KMeansCluster<T> *ClusterCArray = _Clusters.CArray();
        for(UINT ElementIndex = 0; ElementIndex < ElementCount; ElementIndex++)
        {
            const T& CurElement = ElementCArray[ElementIndex];
            UINT ClosestClusterIndex = 0;
            double ClosestClusterDist = Metric::Dist(CurElement, ClusterCArray[0].Center());
            for(UINT ClusterIndex = 1; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                double CurClusterDist = Metric::Dist(CurElement, ClusterCArray[ClusterIndex].Center());
                if(CurClusterDist < ClosestClusterDist)
                {
                    ClosestClusterIndex = ClusterIndex;
                    ClosestClusterDist = CurClusterDist;
                }
            }
            ClusterCArray[ClosestClusterIndex].AddEntry(CurElement);
        }
        for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
        {
            ClusterCArray[ClusterIndex].FinishIteration(Elements.RandomElement());
        }
    }

    void Iterate(const Vector<T> &Elements, const Vector<T> &WeightedElements, const Vector<float> &Weights)
    {
        const UINT ElementCount = Elements.Length();
        const T* ElementCArray = Elements.CArray();
        const float* WeightsCArray = Weights.CArray();
        const UINT ClusterCount = _Clusters.Length();
        KMeansCluster<T> *ClusterCArray = _Clusters.CArray();
        for(UINT ElementIndex = 0; ElementIndex < ElementCount; ElementIndex++)
        {
            const T& CurElement = ElementCArray[ElementIndex];
            float CurWeight = WeightsCArray[ElementIndex];
            UINT ClosestClusterIndex = 0;
            double ClosestClusterDist = Metric::Dist(CurElement, ClusterCArray[0].Center());
            for(UINT ClusterIndex = 1; ClusterIndex < ClusterCount; ClusterIndex++)
            {
                double CurClusterDist = Metric::Dist(CurElement, ClusterCArray[ClusterIndex].Center());
                if(CurClusterDist < ClosestClusterDist)
                {
                    ClosestClusterIndex = ClusterIndex;
                    ClosestClusterDist = CurClusterDist;
                }
            }
            ClusterCArray[ClosestClusterIndex].AddEntry(WeightedElements[ElementIndex], CurWeight);
        }
        for(UINT ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++)
        {
            ClusterCArray[ClusterIndex].FinishIteration(Elements.RandomElement());
        }
    }

    Vector< KMeansCluster<T> > _Clusters;
};
