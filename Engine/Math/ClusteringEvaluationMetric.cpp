#include "Main.h"

//
// http://nlp.uned.es/docs/amigo2007a.pdf
//

double ClusteringEvaluationMetricBCubed::Evaluate(const Vector<UINT> &clusteringA, const Vector<UINT> &clusteringB) const
{
    PersistentAssert(clusteringA.Length() == clusteringB.Length(), "clusterings do not contain the same number of elements");
    
    const UINT elementCount = clusteringA.Length();
    const UINT clusterACount = clusteringA.MaxValue() + 1;
    const UINT clusterBCount = clusteringB.MaxValue() + 1;

    Vector< Vector<UINT> > clustersA(clusterACount);
    Vector< Vector<UINT> > clustersB(clusterBCount);

    for(UINT elementIndex = 0; elementIndex < elementCount; elementIndex++)
    {
        clustersA[clusteringA[elementIndex]].PushEnd(elementIndex);
        clustersB[clusteringB[elementIndex]].PushEnd(elementIndex);
    }

    double precision = 0.0;
    double recall    = 0.0;

    for(UINT elementIndex = 0; elementIndex < elementCount; elementIndex++)
    {
        const UINT targetA = clusteringA[elementIndex];
        const UINT targetB = clusteringB[elementIndex];

        const Vector<UINT> &clusterA = clustersA[targetA];
        const Vector<UINT> &clusterB = clustersB[targetB];

        UINT localPrecision = 0;
        UINT localRecall = 0;

        for(UINT clusterAIndex = 0; clusterAIndex < clusterA.Length(); clusterAIndex++)
        {
            if(clusteringB[clusterA[clusterAIndex]] == targetB) localPrecision++;
        }

        for(UINT clusterBIndex = 0; clusterBIndex < clusterB.Length(); clusterBIndex++)
        {
            if(clusteringA[clusterB[clusterBIndex]] == targetA) localRecall++;
        }

        precision += double(localPrecision) / double(clusterA.Length());
        recall    += double(localRecall)    / double(clusterB.Length());
    }

    precision /= double(elementCount);
    recall    /= double(elementCount);

    if(precision <= 0.0 || recall <= 0.0)
    {
        return 0.0;
    }
    else
    {
        const double beta = 1.0;
        return (1.0 + beta * beta) * precision * recall / (beta * beta * precision + recall);
    }
}
