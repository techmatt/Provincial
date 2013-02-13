/*
KMeansClusteringKDTree.cpp
Written by Matthew Fisher
*/

#ifdef USE_ANN
#ifdef USE_KDTREE

void KMeansClusterKDTree::Init(UINT dimensionCount, const float *start)
{
    center.v.Allocate(dimensionCount);
    memcpy(center.v.CArray(), start, sizeof(float) * dimensionCount);
}

void KMeansClusterKDTree::FinishIteration(UINT dimensionCount, const float *fallback)
{
    if(entries.Length() == 0)
    {
        memcpy(center.v.CArray(), fallback, sizeof(float) * dimensionCount);
    }
    else
    {
        center.v.Clear(0.0f);
        float *centerArray = center.v.CArray();
        const UINT entryCount = entries.Length();
        for(UINT entryIndex = 0; entryIndex < entryCount; entryIndex++)
        {
            const float *curEntry = entries[entryIndex];
            for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
            {
                centerArray[dimensionIndex] += curEntry[dimensionIndex];
            }
        }

        const float scaleFactor = 1.0f / float(entryCount);
        for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            centerArray[dimensionIndex] *= scaleFactor;
        }

        assignmentError = 0.0f;
        for(UINT entryIndex = 0; entryIndex < entryCount; entryIndex++)
        {
            const float *curEntry = entries[entryIndex];
            for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
            {
                float diff = curEntry[dimensionIndex] - centerArray[dimensionIndex];
                assignmentError += diff * diff;
            }
        }

        entries.FreeMemory();
    }
}

void KMeansClusteringKDTree::Cluster(const Vector<const float*> &elements, UINT dimensionCount, UINT clusterCount, UINT maxIterations, double maxDelta, bool verbose)
{
    _dimensionCount = dimensionCount;

    if(verbose)
    {
        Console::WriteLine(String("K-means clustering (KD-tree), ") + String(elements.Length()) + String(" ") + String(dimensionCount) + String("D points, ") + String(clusterCount) + String(" clusters"));
        Console::AdvanceLine();
    }
    PersistentAssert(elements.Length() >= clusterCount, "Too many clusters");
    _clusters.Allocate(clusterCount);
    for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
    {
        _clusters[clusterIndex].Init(dimensionCount, elements.RandomElement());
    }

    UINT passIndex = 0;
    Vector<VecNf> previousClusterCenters(clusterCount);
    bool converged = false;
    while(!converged)
    {
        passIndex++;
        for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
        {
            previousClusterCenters[clusterIndex] = _clusters[clusterIndex].center;
        }

        Iterate(elements);

        double delta = 0.0, assignmentError = 0.0;
        for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
        {
            delta += VecNfKMeansMetric::Dist(previousClusterCenters[clusterIndex], _clusters[clusterIndex].center);
            assignmentError += _clusters[clusterIndex].assignmentError;
        }
        assignmentError /= double(elements.Length());
        converged = (delta <= maxDelta || passIndex == maxIterations);
        if(verbose)
        {
            Console::OverwriteLine(String("Pass ") + String(passIndex) + String(", delta=") + String(delta) + String(", error=") + String(assignmentError));
        }
    }
    UpdateClusterTree();
}

UINT KMeansClusteringKDTree::QuantizeToNearestClusterIndex(const float *element)
{
    _clusterTree.KNearest(element, 1, _indicesStorage, 0.0f);
    return _indicesStorage[0];
}

void KMeansClusteringKDTree::UpdateClusterTree()
{
    const UINT clusterCount = _clusters.Length();
    Vector<float*> clusterCenters(clusterCount);
    for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
    {
        clusterCenters[clusterIndex] = const_cast<float*>(_clusters[clusterIndex].center.v.CArray());
    }
    _clusterTree.BuildTree(clusterCenters, _dimensionCount, 1);
}

void KMeansClusteringKDTree::Iterate(const Vector<const float*> &elements)
{
    UpdateClusterTree();

    const UINT elementCount = elements.Length();
    auto ElementCArray = elements.CArray();
    const UINT clusterCount = _clusters.Length();
    auto *clusterCArray = _clusters.CArray();
    for(UINT elementIndex = 0; elementIndex < elementCount; elementIndex++)
    {
        const float* curElement = ElementCArray[elementIndex];
        _clusterTree.KNearest(curElement, 1, _indicesStorage, 0.0f);
        clusterCArray[_indicesStorage[0]].AddEntry(curElement);
    }
    for(UINT clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
    {
        clusterCArray[clusterIndex].FinishIteration(_dimensionCount, elements.RandomElement());
    }
}

#endif
#endif