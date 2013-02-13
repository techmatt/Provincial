//
// Note that the copy constructor is not yet implemented for BayesNet
//

struct BayesNode
{
    void SetTableProbabilities(const Grid<UINT> &observations);
    
    bool TryToSample(Vector<UINT> &assignments) const;
    double ComputeAssignmentProbability(const Vector<UINT> &assignment) const;

    void SaveBinary(OutputDataStream &stream) const;
    void LoadBinary(InputDataStream &stream, const Vector<BayesNode*> &allNodes);

    UINT index;
    UINT cardinality;
    Vector<BayesNode*> parents;
    MultiGrid<UINT> table;
    mutable Vector<UINT> tableIndicesStorage;
    mutable Vector<double> tableProbabilityStorage;
};

class BayesNet
{
public:
    BayesNet(const Vector<UINT> &variableCardinalities, const Vector< pair<UINT, UINT> > &directedEdges, const Grid<UINT> &observations, const String &description);
    BayesNet(const BayesNet &b);
    BayesNet();
    ~BayesNet();
    
    //
    // Statistics
    //
    void SetTableProbabilities(const Grid<UINT> &observations);
    void Sample(Vector<UINT> &result) const;
    double ComputeAssignmentProbability(const Vector<UINT> &assignment) const;
    double KLDivergence(const Grid<UINT> &observations) const;

    void SaveASCII(const String &filename) const;
    void SaveGraphViz(const String &filename, const Vector<String> &labels) const;
    void SaveBinary(OutputDataStream &stream) const;
    
    void LoadBinary(InputDataStream &stream);

    //
    // Structure Learning
    //
    double MDLScore(const Grid<UINT> &observations) const;
    void GenerateChildren(Vector<BayesNet*> &children, const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges) const;

    static BayesNet* LearnBayesNet(const Vector<UINT> &variableCardinalities, const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges, int maxIterations);
    static bool EdgeSetIsValid(const Vector< pair<UINT, UINT> > &directedEdges, UINT variableCount);

private:
    void InitInternal(const Vector<UINT> &variableCardinalities, const Vector< pair<UINT, UINT> > &directedEdges, const Grid<UINT> &observations);

    Vector<BayesNode*> _nodes;
    String _description;
};
