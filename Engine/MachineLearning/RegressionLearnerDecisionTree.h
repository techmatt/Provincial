const double EntropyFactor = 1.0f / logf(2.0);

template<class LearnerInput, class LearnerOutput>
class RegressionDecisionTreeNode
{
public:
    typedef RegressionLearnerDataset<LearnerInput, LearnerOutput> Dataset;
    typedef RegressionLearnerExample<LearnerInput, LearnerOutput> Example;

    RegressionDecisionTreeNode()
    {
        for(UINT childIndex = 0; childIndex < ChildCount; childIndex++)
        {
            _Children[childIndex] = NULL;
        }
    }

    ~RegressionDecisionTreeNode()
    {
        for(UINT childIndex = 0; childIndex < ChildCount; childIndex++)
        {
            if(_Children[childIndex] != NULL)
            {
                delete _Children[childIndex];
                _Children[childIndex] = NULL;
            }
        }
    }

    void Evaluate(const LearnerInput &Input, LearnerOutput &Result)
    {
        if(Leaf())
        {
            Result = _WeightedCount / _TotalCount;
        }
        else
        {
            return _Children[ComputeChildIndex(Input)]->Evaluate(Input, Result);
        }
    }

    void Train(const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT DepthRemaining)
    {
        if(DepthRemaining == 0 || ActiveExampleIndices.Length() <= 1)
        {
            InitLeafProbabilities(AllExamples, ActiveExampleIndices);
        }
        else
        {
            ChooseVariableAndThreshold(AllExamples, ActiveExampleIndices);

            for(UINT childIndex = 0; childIndex < ChildCount; childIndex++)
            {
                TrainChild(AllExamples, ActiveExampleIndices, childIndex, DepthRemaining);
            }
        }
    }

private:
    __forceinline UINT ComputeChildIndex(const LearnerInput &Input)
    {
        if(Input[_DecisionVariableIndex] < _DecisionVariableThreshold)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    __forceinline bool Leaf()
    {
        return (_Children[0] == NULL);
    }

    static __forceinline double Entropy(double p)
    {
        if(p <= 0.0f || p >= 1.0f)
        {
            return 0.0f;
        }
        else
        {
            double np = 1.0 - p;
            return -(p * LogBaseTwo(p) + np * LogBaseTwo(np));
        }
    }

    static __forceinline double LogBaseTwo(double x)
    {
        return log(x) * EntropyFactor;
    }

    void ChooseVariableAndThreshold(const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        UINT BestVariableIndex = 0;
        double BestThreshold = 0.0;
        double BestInformationGain = -1e15;

        const UINT VariableCount = AllExamples.Entries()[0].Input.Length();
        const UINT ExampleCount = ActiveExampleIndices.Length();

        for(UINT CandidateVariableIndex = 0; CandidateVariableIndex < VariableCount; CandidateVariableIndex++)
        {
            for(UINT thresholdIndex = 0; thresholdIndex < intervalCount - 1; thresholdIndex++)
            {
                double CandidateThreshold = float(thresholdIndex + 1) / float(intervalCount);
                double PartitionTotalNodes[ChildCount];
                double PartitionPositives[ChildCount];

                for(UINT childIndex = 0; childIndex < ChildCount; childIndex++)
                {
                    partitionTotalNodes[childIndex] = 0.0f;
                    partitionPositives[childIndex] = 0.0f;
                }

                _DecisionVariableIndex = candidateVariableIndex;
                _DecisionVariableThreshold = candidateThreshold;

                //float totalPositives = 0.0f, totalCount = 0.0f;
                for(UINT exampleIndex = 0; exampleIndex < numExamples; exampleIndex++)
                {
                    const RegressionLearnerExample<LearnerInput, LearnerOutput> &curExample = AllExamples.Entries()[ActiveExampleIndices[exampleIndex]];
                    int childNodeIndex = ComputeChildIndex(curExample.Input);
                    
                    partitionTotalNodes[childNodeIndex] += curExample.Weight;
                    //totalCount += curExample.Weight;

                    //totalPositives += curExample.Weight * curExample.Output;
                    partitionPositives[childNodeIndex] += curExample.Weight * curExample.Output;
                }
                							 
                /*float totalEntropy = 0.0f
                if(totalCount > 0.0f)
                {
                    totalEntropy = Entropy(totalPositives / totalCount);
                }*/
                float leftEntropy = 0.0f, rightEntropy = 0.0f;
                if(partitionTotalNodes[0] > 0.0f)
                {
                    leftEntropy = Entropy(partitionPositives[0] / partitionTotalNodes[0]);
                }
                if(partitionTotalNodes[1] > 0.0f)
                {
                    rightEntropy = Entropy(partitionPositives[1] / partitionTotalNodes[1]);
                }
                float curInformationGain = //totalCount * totalEntropy +
							                 -partitionTotalNodes[0] * leftEntropy +
							                 -partitionTotalNodes[1] * rightEntropy;

                if(curInformationGain > bestInformationGain)
                {
                    bestInformationGain = curInformationGain;
                    bestThreshold = candidateThreshold;
                    bestVariableIndex = candidateVariableIndex;
                }
            }
        }

        _DecisionVariableIndex = bestVariableIndex;
        _DecisionVariableThreshold = bestThreshold;
    }

    void InitLeafProbabilities(const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        _WeightedCount = 0;
        _TotalCount = 0;

        const UINT numExamples = ActiveExampleIndices.Length();
        for (UINT exampleIndex = 0; exampleIndex < numExamples; exampleIndex++)
        {
            const RegressionLearnerExample<LearnerInput, LearnerOutput> &curExample = AllExamples.Entries()[ActiveExampleIndices[exampleIndex]];
            _TotalCount += curExample.Weight;
            _WeightedCount += curExample.Weight * curExample.Output;
        }

        if(_TotalCount == 0.0)
        {
            _TotalCount = 1.0;
        }
    }

    void TrainChild(const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT ChildIndex, UINT DepthRemaining)
    {
        UINT childDatasetSize = 0;
        const UINT numExamples = ActiveExampleIndices.Length();
        for (UINT exampleIndex = 0; exampleIndex < numExamples; exampleIndex++)
        {
            const RegressionLearnerExample<LearnerInput, LearnerOutput> &curExample = AllExamples.Entries()[ActiveExampleIndices[exampleIndex]];
            if(ComputeChildIndex(curExample.Input) == ChildIndex)
            {
                childDatasetSize++;
            }
        }

        Vector<UINT> ChildActiveExampleIndices(childDatasetSize);

        UINT childExampleIndex = 0;
        for (UINT exampleIndex = 0; exampleIndex < numExamples; exampleIndex++)
        {
            const RegressionLearnerExample<LearnerInput, LearnerOutput> &curExample = AllExamples.Entries()[ActiveExampleIndices[exampleIndex]];
            if(ComputeChildIndex(curExample.Input) == ChildIndex)
            {
                ChildActiveExampleIndices[childExampleIndex++] = ActiveExampleIndices[exampleIndex];
            }
        }

        _Children[ChildIndex] = new RegressionDecisionTreeNode;
        _Children[ChildIndex]->Train(AllExamples, ChildActiveExampleIndices, DepthRemaining - 1);
    }
    
    float _WeightedCount;
    float _TotalCount;
    UINT _DecisionVariableIndex;
    float _DecisionVariableThreshold;

    static const int ChildCount = 2;
    RegressionDecisionTreeNode *_Children[ChildCount];
};

template<class LearnerInput, class LearnerOutput>
class RegressionLearnerDecisionTree : public RegressionLearner<LearnerInput, LearnerOutput>
{
public:
    RegressionLearnerDecisionTree(UINT MaxDepth)
    {
        _Root = NULL;
        _MaxDepth = MaxDepth;
    }

    ~RegressionLearnerDecisionTree()
    {
        FreeMemory();
    }

    void FreeMemory()
    {
        if(_Root != NULL)
        {
            delete _Root;
            _Root = NULL;
        }
    }

    void Train(const Dataset &Examples)
    {
        FreeMemory();
        
        Vector<UINT> AllExampleIndices(Examples.Entries().Length());
        for(UINT exampleIndex = 0; exampleIndex < AllExampleIndices.Length(); exampleIndex++)
        {
            AllExampleIndices[exampleIndex] = exampleIndex;
        }

        _Root = new RegressionDecisionTreeNode<LearnerInput, LearnerOutput>;
        _Root->Train(Examples, AllExampleIndices, _MaxDepth);
    }
    void Evaluate(const LearnerInput &Input, LearnerOutput &Result) const
    {
        Assert(_Root != NULL, "_Root == NULL");
        _Root->Evaluate(Input, Result);
    }

private:
    RegressionDecisionTreeNode<LearnerInput, LearnerOutput> *_Root;
    UINT _MaxDepth;
};

template<class LearnerInput, class LearnerOutput>
class RegressionLearnerDecisionTreeFactory : public RegressionLearnerFactory<LearnerInput, LearnerOutput>
{
public:
    RegressionLearnerDecisionTreeFactory(UINT MaxDepth)
    {
        _MaxDepth = MaxDepth;
    }

    RegressionLearner<LearnerInput, LearnerOutput>* MakeLearner()
    {
        return new RegressionLearnerDecisionTree<LearnerInput, LearnerOutput>(_MaxDepth);
    }

private:
    UINT _MaxDepth;
};