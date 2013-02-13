
//
// Abstract base class for all bipartite matching algorithms
//
// Note that some algorithms may modify the input matrix.
//
class BipartiteMatcher
{
public:
    virtual void match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result) = 0;
};


namespace BipartiteHelper
{
    struct Match
    {
        Match() {}
        Match(int _i, int _j, float _c) : i(_i), j(_j), c(_c) {}
        int i, j;
        float c;
    };

    struct MatchComp
    {
        bool operator() (const Match& m1, const Match& m2)
        {
            return m1.c < m2.c;
        }
    };

    struct MatchCompRev
    {
        bool operator() (const Match& m1, const Match& m2)
        {
            return m1.c > m2.c;
        }
    };
};

//
// Greedy bipartite matcher using a priority queue
//
class BipartiteMatcherGreedyPriority : public BipartiteMatcher
{
public:
    void match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result);
};


//
// Greedy bipartite matcher sorting all the scores
//
class BipartiteMatcherGreedyVector : public BipartiteMatcher
{
public:
    void match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result);
};


//
// Bipartite matching upper bound, allowing duplicate matches to the same object.
//
class BipartiteMatcherUpperBound : public BipartiteMatcher
{
public:
    void match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result);
};



//
// Hungarian solver using code adapted from Knuth's Stanford Graphbase
// http://reptar.uta.edu/NOTES5311/hungarian.c
//
class BipartiteMatcherHungarianKnuth : public BipartiteMatcher
{
public:
    void match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result);

private:
    void initArrays(const Grid<float> &m, UINT rows, UINT cols);
    void deinitArrays();
    void hungarian(Vector<UINT> &result);

    float** Array;
    int size1, size2;
};
