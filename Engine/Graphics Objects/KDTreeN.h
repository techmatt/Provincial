/*
KDTree3.h
Written by Matthew Fisher

A 3D KD-tree that supports fast points-within-rectangle queries
*/

#ifdef USE_ANN
#ifdef USE_KDTREE
#include "ANN/ANN.h"

class KDTree
{
public:
    KDTree();
    ~KDTree();
    void FreeMemory();

    void BuildTree(const Vector<double*> &Points, UINT Dimension, UINT MaxK);
    void BuildTree(const Vector<float*> &Points, UINT Dimension, UINT MaxK);
    void KNearest(const float *QueryPoint, UINT k, Vector<UINT> &Indices, float Epsilon) const;
    void KNearest(const double *QueryPoint, UINT k, Vector<UINT> &Indices, float Epsilon) const;
    void KNearest(const float *QueryPoint, UINT k, Vector<UINT> &Indices, Vector<float> &Distances, float Epsilon) const;
    void KNearest(const double *QueryPoint, UINT k, Vector<UINT> &Indices, Vector<double> &Distances, float Epsilon) const;
    __forceinline UINT MaxK()
    {
        return _MaxK;
    }
    __forceinline UINT Dimension()
    {
        return _Dimension;
    }
    double* GetDataPoint(UINT Index)
    {
        return _DataPts[Index];
    }

private:
    UINT                 _MaxK;        // Maximum value of k
    UINT                 _Dimension;   // dimensionality of KDTree
    ANNpointArray        _DataPts;     // data points
    mutable ANNpoint     _QueryPt;     // query point
    mutable ANNidxArray  _Indices;     // near neighbor indices
    mutable ANNdistArray _Dists;       // near neighbor distances
    ANNpointSet*         _KDTree;      // search structure
};

#endif
#endif