/*
KDTree3.cpp
Written by Matthew Fisher

A 3D KD-tree that supports fast points-within-rectangle queries
*/

#ifdef USE_ANN
#ifdef USE_KDTREE

KDTree::KDTree()
{
    _Dimension = 0;
    _MaxK = 0;
    _Indices = NULL;
    _Dists = NULL;
    _QueryPt = NULL;
    _DataPts = NULL;
    _KDTree = NULL;
}

KDTree::~KDTree()
{
    FreeMemory();
}

void KDTree::FreeMemory()
{
    if(_Indices)
    {
        delete[] _Indices;
        _Indices = NULL;
    }
    if(_Dists)
    {
        delete[] _Dists;
        _Dists = NULL;
    }
    if(_KDTree)
    {
        delete _KDTree;
        _KDTree = NULL;
    }
    if(_QueryPt)
    {
        annDeallocPt(_QueryPt);
        _QueryPt = NULL;
    }
    if(_DataPts)
    {
        annDeallocPts(_DataPts);
        _DataPts = NULL;
    }
    _Dimension = 0;
}

void KDTree::BuildTree(const Vector<double*> &Points, UINT Dimension, UINT MaxK)
{
    UINT PointCount = Points.Length();
    _Dimension = Dimension;
    _MaxK = MaxK;
    _QueryPt = annAllocPt(_Dimension); // allocate query point
    _DataPts = annAllocPts(PointCount, _Dimension); // allocate data points
    _Indices = new ANNidx[_MaxK]; // allocate near neigh indices
    _Dists = new ANNdist[_MaxK];  // allocate near neighbor _Dists
    for(UINT i = 0; i < PointCount; i++)
    {
        for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
        {
            _DataPts[i][ElementIndex] = Points[i][ElementIndex];
        }
    }

    const bool UseBruteForce = false;
    if(UseBruteForce)
    {
        _KDTree = new ANNbruteForce( // build search structure
            _DataPts,    // the data points
            PointCount,  // number of points
            _Dimension); // dimension of space
    }
    else
    {
        _KDTree = new ANNkd_tree( // build search structure
            _DataPts,    // the data points
            PointCount,  // number of points
            _Dimension); // dimension of space
    }
}

void KDTree::BuildTree(const Vector<float*> &Points, UINT Dimension, UINT MaxK)
{
    UINT PointCount = Points.Length();
    _Dimension = Dimension;
    _MaxK = MaxK;
    _QueryPt = annAllocPt(_Dimension); // allocate query point
    _DataPts = annAllocPts(PointCount, _Dimension); // allocate data points
    _Indices = new ANNidx[_MaxK]; // allocate near neigh indices
    _Dists = new ANNdist[_MaxK];  // allocate near neighbor _Dists
    for(UINT i = 0; i < PointCount; i++)
    {
        for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
        {
            _DataPts[i][ElementIndex] = Points[i][ElementIndex];
        }
    }

    const bool UseBruteForce = false;
    if(UseBruteForce)
    {
        _KDTree = new ANNbruteForce( // build search structure
            _DataPts,    // the data points
            PointCount,  // number of points
            _Dimension); // dimension of space
    }
    else
    {
        _KDTree = new ANNkd_tree( // build search structure
            _DataPts,    // the data points
            PointCount,  // number of points
            _Dimension); // dimension of space
    }
}

void KDTree::KNearest(const float *QueryPoint, UINT k, Vector<UINT> &Indices, float Epsilon) const
{
    for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
    {
        _QueryPt[ElementIndex] = QueryPoint[ElementIndex];
    }
    _KDTree->annkSearch( // search
        _QueryPt,        // query point
        k,               // number of near neighbors
        _Indices,        // nearest neighbors (returned)
        _Dists,          // distance (returned)
        Epsilon);        // error bound

    if(Indices.Length() < k)
    {
        Indices.Allocate(k);
    }
    for(UINT i = 0; i < k; i++)
    {
        Indices[i] = _Indices[i];
    }
}

void KDTree::KNearest(const float *QueryPoint, UINT k, Vector<UINT> &Indices, Vector<float> &Distances, float Epsilon) const
{
    for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
    {
        _QueryPt[ElementIndex] = QueryPoint[ElementIndex];
    }
    _KDTree->annkSearch( // search
        _QueryPt,        // query point
        k,               // number of near neighbors
        _Indices,        // nearest neighbors (returned)
        _Dists,          // distance (returned)
        Epsilon);        // error bound

    if(Indices.Length() < k)
    {
        Indices.Allocate(k);
    }
    if(Distances.Length() < k)
    {
        Distances.ReSize(k);
    }
    for(UINT i = 0; i < k; i++)
    {
        Indices[i] = _Indices[i];
        Distances[i] = float(_Dists[i]);
    }
}

void KDTree::KNearest(const double *QueryPoint, UINT k, Vector<UINT> &Indices, float Epsilon) const
{
    for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
    {
        _QueryPt[ElementIndex] = QueryPoint[ElementIndex];
    }
    _KDTree->annkSearch( // search
        _QueryPt,        // query point
        k,               // number of near neighbors
        _Indices,        // nearest neighbors (returned)
        _Dists,          // distance (returned)
        Epsilon);        // error bound

    if(Indices.Length() < k)
    {
        Indices.ReSize(k);
    }
    for(UINT i = 0; i < k; i++)
    {
        Indices[i] = _Indices[i];
    }
}

void KDTree::KNearest(const double *QueryPoint, UINT k, Vector<UINT> &Indices, Vector<double> &Distances, float Epsilon) const
{
    for(UINT ElementIndex = 0; ElementIndex < _Dimension; ElementIndex++)
    {
        _QueryPt[ElementIndex] = QueryPoint[ElementIndex];
    }
    _KDTree->annkSearch( // search
        _QueryPt,        // query point
        k,               // number of near neighbors
        _Indices,        // nearest neighbors (returned)
        _Dists,          // distance (returned)
        Epsilon);        // error bound

    if(Indices.Length() < k)
    {
        Indices.ReSize(k);
    }
    if(Distances.Length() < k)
    {
        Distances.ReSize(k);
    }
    for(UINT i = 0; i < k; i++)
    {
        Indices[i] = _Indices[i];
        Distances[i] = float(_Dists[i]);
    }
}

#endif
#endif