/*
KDTree3.h
Written by Matthew Fisher

A 3D KD-tree that supports fast points-within-rectangle queries
*/

#ifdef USE_ANN
#ifdef USE_KDTREE
#include "ANN/ANN.h"

const UINT KDTree3MaxK = 5000;

class KDTree3
{
public:
    KDTree3();
    ~KDTree3();
    void FreeMemory();

    void BuildTree(const PointSet &Points);
    void BuildTree(const Vector<Vec3f> &Points);
    void KNearest(const Vec3f &Pos, UINT k, Vector<UINT> &Result, float Epsilon);
    void KNearest(const Vec3f &Pos, UINT k, UINT *Result, float Epsilon);
    void WithinDistance(const Vec3f &Pos, float Radius, Vector<UINT> &Result) const;
    UINT Nearest(const Vec3f &Pos);
    __forceinline Vec3f GetPoint(UINT Index)
    {
        Vec3f Result;
        for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
        {
            Result[ElementIndex] = float(dataPts[Index][ElementIndex]);
        }
        return Result;
    }

private:
    __forceinline Vec3f GetDataPoint(UINT PointIndex)
    {
        Vec3f Result;
        for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
        {
            Result[ElementIndex] = float(dataPts[PointIndex][ElementIndex]);
        }
        return Result;
    }
    ANNpointArray    dataPts; // data points
    ANNpoint         queryPt; // query point
    ANNidxArray      nnIdx;   // near neighbor indices
    ANNdistArray     dists;   // near neighbor distances
    ANNkd_tree*      kdTree;  // search structure
    //Mutex          _Lock;
};
#endif
#endif