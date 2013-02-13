#include "Main.h"

//
// http://www.kyb.mpg.de/fileadmin/user_upload/files/publications/attachments/Luxburg07_tutorial_4488%5B0%5D.pdf
//

void SpectralClustering::MakeSimilarityMatrix(Vector<float*> &points, UINT dimension, float sigma, bool zeroDiagonal, DenseMatrix<double> &result)
{
    const UINT pointCount = points.Length();
    result.Allocate(pointCount, pointCount);
    const double expScale = -1.0f / (2.0f * sigma * sigma);
    for(UINT outerPointIndex = 0; outerPointIndex < pointCount; outerPointIndex++)
    {
        float *outerPoint = points[outerPointIndex];
        for(UINT innerPointIndex = 0; innerPointIndex < pointCount; innerPointIndex++)
        {
            float *innerPoint = points[innerPointIndex];
            float sum = 0.0f;
            for(UINT dimensionIndex = 0; dimensionIndex < dimension; dimensionIndex++)
            {
                float diff = innerPoint[dimensionIndex] - outerPoint[dimensionIndex];
                sum += diff * diff;
            }
            result(outerPointIndex, innerPointIndex) = exp(sum * expScale);
        }
    }
    if(zeroDiagonal)
    {
        for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
        {
            result(pointIndex, pointIndex) = 0.0;
        }
    }
}

void SpectralClustering::MakeLaplacian(const DenseMatrix<double> &S, Method method, DenseMatrix<double> &L)
{
    const UINT pointCount = S.RowCount();
    Vector<double> weightSums(pointCount, 0.0);
    for(UINT outerPointIndex = 0; outerPointIndex < pointCount; outerPointIndex++)
    {
        for(UINT innerPointIndex = 0; innerPointIndex < pointCount; innerPointIndex++)
        {
            weightSums[outerPointIndex] += S(outerPointIndex, innerPointIndex);
        }
    }

    if(method == Unnormalized || method == RandomWalk)
    {
        L = S;
        for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
        {
            L(pointIndex, pointIndex) = weightSums[pointIndex] - L(pointIndex, pointIndex);
        }
        if(method == RandomWalk)
        {
            for(UINT outerPointIndex = 0; outerPointIndex < pointCount; outerPointIndex++)
            {
                for(UINT innerPointIndex = 0; innerPointIndex < pointCount; innerPointIndex++)
                {
                    L(outerPointIndex, innerPointIndex) /= weightSums[outerPointIndex];
                }
            }
        }
    }
    if(method == Symmetric)
    {
        Vector<double> weightInvSqrts(pointCount);
        for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
        {
            weightInvSqrts[pointIndex] = sqrt(1.0f / weightSums[pointIndex]);
        }
        for(UINT outerPointIndex = 0; outerPointIndex < pointCount; outerPointIndex++)
        {
            for(UINT innerPointIndex = 0; innerPointIndex < pointCount; innerPointIndex++)
            {
                L(outerPointIndex, innerPointIndex) = S(outerPointIndex, innerPointIndex) * weightInvSqrts[outerPointIndex] * weightInvSqrts[innerPointIndex];
            }
        }
    }
}

void SpectralClustering::Cluster(const DenseMatrix<double> &S, UINT kMeansClusters, UINT reducedDimensionCount, Method method, Vector<UINT> &clusterIDs, SpectralClusteringCache *cache)
{
    const UINT pointCount = S.RowCount();
    
    Console::WriteLine("Spectral clustering on " + String(pointCount) + " points, " + String(kMeansClusters) + " clusters, " + String(reducedDimensionCount) + String(" dimensions"));
    
    DenseMatrix<double> L(pointCount, pointCount);
    MakeLaplacian(S, method, L);

    Vector<double> eigenvalues;
    DenseMatrix<double> eigenvectors;
    {
        if(cache != NULL && cache->eigenvalues.Length() == pointCount)
        {
            Console::WriteString("Loading eigensystem from cache...");
            eigenvalues = cache->eigenvalues;
            eigenvectors = cache->eigenvectors;
        }
        else
        {
            Clock timer;
            Console::WriteString("Solving eigensystem...");
            L.EigenSystemTNT(eigenvalues, eigenvectors);
            Console::WriteLine(String(timer.Elapsed()) + "s");
        }
        if(cache != NULL)
        {
            cache->eigenvalues = eigenvalues;
            cache->eigenvectors = eigenvectors;
        }
    }

    //
    // Note that the eigensystem is sorted in decreasing order.
    //
    const bool useLargestEigenvector = (method == RandomWalk || method == Symmetric);
    
    Vector<VecNf> points(pointCount);
    for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
    {
        VecNf &curPoint = points[pointIndex];
        curPoint.v.Allocate(reducedDimensionCount);
        for(UINT dimensionIndex = 0; dimensionIndex < reducedDimensionCount; dimensionIndex++)
        {
            if(useLargestEigenvector)
            {
                curPoint[dimensionIndex] = float(eigenvectors(pointIndex, pointCount - dimensionIndex - 1));
            }
            else
            {
                curPoint[dimensionIndex] = float(eigenvectors(pointIndex, dimensionIndex));
            }
            
        }
    }

    if(method == Symmetric)
    {
        for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
        {
            points[pointIndex] = VecNf::Normalize(points[pointIndex]);
        }
    }

    KMeansClustering<VecNf, VecNfKMeansMetric> clustering;
    clustering.Cluster(points, kMeansClusters, 100);
    
    clusterIDs.Allocate(pointCount);
    for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
    {
        clusterIDs[pointIndex] = clustering.QuantizeToNearestClusterIndex(points[pointIndex]);
    }
}
