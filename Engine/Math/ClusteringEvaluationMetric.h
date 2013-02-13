class ClusteringEvaluationMetric
{
public:
    virtual double Evaluate(const Vector<UINT> &clusteringA, const Vector<UINT> &clusteringB) const = 0;
};

class ClusteringEvaluationMetricBCubed
{
public:
    double Evaluate(const Vector<UINT> &clusteringA, const Vector<UINT> &clusteringB) const;
};
