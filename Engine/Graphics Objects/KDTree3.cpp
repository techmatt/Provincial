/*
KDTree3.cpp
Written by Matthew Fisher

A 3D KD-tree that supports fast points-within-rectangle queries
*/

#ifdef USE_ANN
#ifdef USE_KDTREE

KDTree3::KDTree3()
{
    nnIdx = NULL;
    dists = NULL;
    queryPt = NULL;
    dataPts = NULL;
    kdTree = NULL;
}

KDTree3::~KDTree3()
{
    FreeMemory();
}

void KDTree3::FreeMemory()
{
    if(nnIdx)
    {
        delete[] nnIdx;
        nnIdx = NULL;
    }
    if(dists)
    {
        delete[] dists;
        dists = NULL;
    }
    if(kdTree)
    {
        delete kdTree;
        kdTree = NULL;
    }
    if(queryPt)
    {
        annDeallocPt(queryPt);
        queryPt = NULL;
    }
    if(dataPts)
    {
        annDeallocPts(dataPts);
        dataPts = NULL;
    }
}

void KDTree3::BuildTree(const PointSet &Points)
{
    FreeMemory();
    UINT PointCount = Points.Points().Length();
    Console::WriteString(String("Building KD tree, ") + String(PointCount) + String(" points..."));
    queryPt = annAllocPt(3); // allocate query point
    dataPts = annAllocPts(PointCount, 3); // allocate data points
    nnIdx = new ANNidx[KDTree3MaxK];  // allocate near neigh indices
    dists = new ANNdist[KDTree3MaxK]; // allocate near neighbor dists
    for(UINT i = 0; i < PointCount; i++)
    {
        for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
        {
            dataPts[i][ElementIndex] = Points.Points()[i].Position[ElementIndex];
        }
    }

    kdTree = new ANNkd_tree( // build search structure
        dataPts,    // the data points
        PointCount, // number of points
        3);            // dimension of space
    Console::WriteString(String("done.\n"));
}

void KDTree3::BuildTree(const Vector<Vec3f> &Points)
{
    FreeMemory();
    UINT PointCount = Points.Length();
    Console::WriteString(String("Building KD tree, ") + String(PointCount) + String(" points..."));
    queryPt = annAllocPt(3); // allocate query point
    dataPts = annAllocPts(PointCount, 3); // allocate data points
    nnIdx = new ANNidx[KDTree3MaxK];  // allocate near neigh indices
    dists = new ANNdist[KDTree3MaxK]; // allocate near neighbor dists
    for(UINT i = 0; i < PointCount; i++)
    {
        for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
        {
            dataPts[i][ElementIndex] = Points[i][ElementIndex];
        }
    }

    kdTree = new ANNkd_tree( // build search structure
        dataPts,    // the data points
        PointCount, // number of points
        3);         // dimension of space
    Console::WriteString(String("done.\n"));
}

UINT KDTree3::Nearest(const Vec3f &Pos)
{
    Vector<UINT> Result(1);
    KNearest(Pos, 1, Result, 0.0f);
    return Result[0];
}

void KDTree3::KNearest(const Vec3f &Pos, UINT k, Vector<UINT> &Result, float Epsilon)
{
    Assert(k <= KDTree3MaxK, "k too large");
    for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
    {
        queryPt[ElementIndex] = Pos[ElementIndex];
    }
    kdTree->annkSearch( // search
        queryPt,        // query point
        k,                // number of near neighbors
        nnIdx,            // nearest neighbors (returned)
        dists,            // distance (returned)
        Epsilon);        // error bound

    if(Result.Length() < k)
    {
        Result.ReSize(k);
    }
    for(UINT i = 0; i < k; i++)
    {
        Result[i] = nnIdx[i];
    }
}

void KDTree3::KNearest(const Vec3f &Pos, UINT k, UINT *Result, float Epsilon)
{
    Assert(k <= KDTree3MaxK, "k too large");
    for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
    {
        queryPt[ElementIndex] = Pos[ElementIndex];
    }
    kdTree->annkSearch( // search
        queryPt,        // query point
        k,                // number of near neighbors
        nnIdx,            // nearest neighbors (returned)
        dists,            // distance (returned)
        Epsilon);        // error bound
    for(UINT i = 0; i < k; i++)
    {
        Result[i] = nnIdx[i];
    }
}

void KDTree3::WithinDistance(const Vec3f &Pos, float Radius, Vector<UINT> &Result) const
{
    for(UINT ElementIndex = 0; ElementIndex < 3; ElementIndex++)
    {
        queryPt[ElementIndex] = Pos[ElementIndex];
    }

    int NeighborCount = kdTree->annkFRSearch(
        queryPt,
        Radius * Radius,
        KDTree3MaxK,
        nnIdx,
        dists,
        0.0f);

    Result.ReSize(Math::Min(UINT(NeighborCount), KDTree3MaxK));
    for(UINT i = 0; int(i) < Result.Length(); i++)
    {
        Result[i] = nnIdx[i];
    }
}

#endif
#endif