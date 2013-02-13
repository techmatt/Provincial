class GraphicalModel
{
public:
    //
    // Statistics
    //
    virtual void Sample(Vector<UINT> &result) const = 0;
};

class GraphicalModelBayesNetBooleanizer : public GraphicalModel
{
public:
    struct BooleanVariable
    {
        UINT variableIndex;
        UINT value;
    };

    GraphicalModelBayesNetBooleanizer()
    {
        _bayes = NULL;
    }

    void Sample(Vector<UINT> &result) const;
    void Learn(const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges, int maxIterations);

    void SaveBinary(const String &filename) const;
    void LoadBinary(const String &filename);

    __forceinline const BayesNet& Bayes() const
    {
        return *_bayes;
    }

    __forceinline const Vector<BooleanVariable>& Booleans() const
    {
        return _booleanVariables;
    }

private:
    Vector<UINT> ObservationToBooleans(const Vector<UINT> &observation) const;
    Vector<UINT> BooleansToObservation(const Vector<UINT> &booleans) const;

    Vector<UINT> _cardinalities;
    Vector<BooleanVariable> _booleanVariables;
    
    BayesNet *_bayes;
};
