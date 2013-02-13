template<class LearnerInput>
class BinaryClassifierDecisionTreeNode
{
public:
    typedef ClassifierDataset<LearnerInput> Dataset;
    typedef ClassifierExample<LearnerInput> Example;

    BinaryClassifierDecisionTreeNode()
    {
        for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
        {
            _Children[ChildIndex] = NULL;
        }
    }

    ~BinaryClassifierDecisionTreeNode()
    {
        for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
        {
            if(_Children[ChildIndex] != NULL)
            {
                delete _Children[ChildIndex];
                _Children[ChildIndex] = NULL;
            }
        }
    }

    void Evaluate(const LearnerInput &Input, double &ProbabilityClass0) const
    {
        if(Leaf())
        {
            ProbabilityClass0 = 1.0 - _Class1WeightedSum / _TotalWeightedSum;
        }
        else
        {
            return _Children[ComputeChildIndex(Input)]->Evaluate(Input, ProbabilityClass0);
        }
    }

    void Train(const DecisionTreeConfiguration &Config, UINT Class0Index, UINT Class1Index, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT DepthRemaining)
    {
        bool MakeLeafNode = (DepthRemaining == 0 || ActiveExampleIndices.Length() <= Config.LeafNodeCountCutoff);
        if(!MakeLeafNode)
        {
            if(ChooseVariableAndThreshold(Config, Class0Index, Class1Index, AllExamples, ActiveExampleIndices))
            {
                for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
                {
                    TrainChild(Config, Class0Index, Class1Index, AllExamples, ActiveExampleIndices, ChildIndex, DepthRemaining);
                }
            }
            else
            {
                MakeLeafNode = true;
            }
        }
        if(MakeLeafNode)
        {
            InitLeafProbabilities(Config, Class0Index, Class1Index, AllExamples, ActiveExampleIndices);
        }
    }

private:
    __forceinline UINT ComputeChildIndex(const LearnerInput &Input) const
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

    __forceinline bool Leaf() const
    {
        return (_Children[0] == NULL);
    }

    static __forceinline double Entropy(double p)
    {
        if(p <= 0.0 || p >= 1.0)
        {
            return 0.0;
        }
        else
        {
            double np = 1.0 - p;
            return -(p * log(p) + np * log(np));
        }
    }

    //static __forceinline double LogBaseTwo(double x)
    //{
    //    return log(x) * EntropyFactor;
    //}

    bool ChooseVariableAndThreshold(const DecisionTreeConfiguration &Config, UINT Class0Index, UINT Class1Index, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        int BestVariableIndex = -1;
        double BestThreshold = 0.0;
        double BestInformationGain = 0.0;

        const UINT VariableCount = AllExamples.Entries()[0].Input.Length();
        const UINT ExampleCount = ActiveExampleIndices.Length();

        for(UINT CandidateVariableIndex = 0; CandidateVariableIndex < VariableCount; CandidateVariableIndex++)
        {
            for(UINT ThresholdIndex = 0; ThresholdIndex < Config.SplitCandidatesPerFeature; ThresholdIndex++)
            {
                double CandidateThreshold = (AllExamples.Entries()[ActiveExampleIndices.RandomElement()].Input[CandidateVariableIndex] +
                                             AllExamples.Entries()[ActiveExampleIndices.RandomElement()].Input[CandidateVariableIndex]) * 0.5;

                double PartitionTotalNodes[ChildCount];
                double PartitionClass1WeightedSum[ChildCount];
                for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
                {
                    PartitionTotalNodes[ChildIndex] = 0.0;
                    PartitionClass1WeightedSum[ChildIndex] = 0.0;
                }

                _DecisionVariableIndex = CandidateVariableIndex;
                _DecisionVariableThreshold = CandidateThreshold;

                double TotalClass1WeightedSum = 0.0, TotalCount = 0.0;
                for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
                {
                    const ClassifierExample<LearnerInput> &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
                    UINT ChildNodeIndex = ComputeChildIndex(CurExample.Input);
                    
                    PartitionTotalNodes[ChildNodeIndex] += CurExample.Weight;
                    TotalCount += CurExample.Weight;

                    double PositiveTerm = CurExample.Weight * CurExample.ZeroOneClass(Class0Index);
                    PartitionClass1WeightedSum[ChildNodeIndex] += PositiveTerm;
                    TotalClass1WeightedSum += PositiveTerm;
                }

                double TotalEntropy = 0.0;
                if(TotalCount > 0.0)
                {
                    TotalEntropy = Entropy(TotalClass1WeightedSum / TotalCount);
                }

                double LeftEntropy = 0.0, RightEntropy = 0.0;
                if(PartitionTotalNodes[0] > 0.0)
                {
                    LeftEntropy = Entropy(PartitionClass1WeightedSum[0] / PartitionTotalNodes[0]);
                }
                if(PartitionTotalNodes[1] > 0.0)
                {
                    RightEntropy = Entropy(PartitionClass1WeightedSum[1] / PartitionTotalNodes[1]);
                }
                double CurInformationGain = TotalCount * TotalEntropy +
							                -PartitionTotalNodes[0] * LeftEntropy +
							                -PartitionTotalNodes[1] * RightEntropy;
                
                if(CurInformationGain > BestInformationGain)
                {
                    BestInformationGain = CurInformationGain;
                    BestThreshold = CandidateThreshold;
                    BestVariableIndex = CandidateVariableIndex;

                    /*int Child0Classification = Class0Index;
                    int Child1Classification = Class0Index;
                    if(PartitionClass1WeightedSum[0] / PartitionTotalNodes[0] > 0.5)
                    {
                        Child0Classification = Class1Index;
                    }
                    if(PartitionClass1WeightedSum[1] / PartitionTotalNodes[1] > 0.5)
                    {
                        Child1Classification = Class1Index;
                    }

                    double WeightedClassificationError = 0.0;
                    for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
                    {
                        const ClassifierExample<LearnerInput> &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
                        UINT ChildNodeIndex = ComputeChildIndex(CurExample.Input);
                        if((ChildNodeIndex == 0 && CurExample.Class != Child0Classification) ||
                           (ChildNodeIndex == 1 && CurExample.Class != Child1Classification))
                        {
                            WeightedClassificationError += CurExample.Weight;
                        }
                    }
                    Console::WriteLine(String("New best classification error: ") + String(WeightedClassificationError));*/
                }
            }
        }

        _DecisionVariableIndex = BestVariableIndex;
        _DecisionVariableThreshold = BestThreshold;

        return (BestVariableIndex != -1);
    }

    void InitLeafProbabilities(const DecisionTreeConfiguration &Config, UINT Class0Index, UINT Class1Index, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        _Class1WeightedSum = 0.0;
        _TotalWeightedSum = 0.0;

        const UINT ExampleCount = ActiveExampleIndices.Length();
        for (UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const ClassifierExample<LearnerInput> &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            _TotalWeightedSum += CurExample.Weight;
            _Class1WeightedSum += CurExample.Weight * CurExample.ZeroOneClass(Class0Index);
        }

        if(_TotalWeightedSum == 0.0)
        {
            _TotalWeightedSum = 1.0;
        }
    }

    void TrainChild(const DecisionTreeConfiguration &Config, UINT Class0Index, UINT Class1Index, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT ChildIndex, UINT DepthRemaining)
    {
        UINT ChildDatasetSize = 0;
        const UINT ExampleCount = ActiveExampleIndices.Length();
        for (UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const ClassifierExample<LearnerInput> &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            if(ComputeChildIndex(CurExample.Input) == ChildIndex)
            {
                ChildDatasetSize++;
            }
        }

        Vector<UINT> ChildActiveExampleIndices(ChildDatasetSize);

        UINT ChildExampleIndex = 0;
        for (UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const ClassifierExample<LearnerInput> &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            if(ComputeChildIndex(CurExample.Input) == ChildIndex)
            {
                ChildActiveExampleIndices[ChildExampleIndex++] = ActiveExampleIndices[ExampleIndex];
            }
        }

        _Children[ChildIndex] = new BinaryClassifierDecisionTreeNode<LearnerInput>;
        _Children[ChildIndex]->Train(Config, Class0Index, Class1Index, AllExamples, ChildActiveExampleIndices, DepthRemaining - 1);
    }
    
    UINT _DecisionVariableIndex;
    double _DecisionVariableThreshold;
    double _Class1WeightedSum;
    double _TotalWeightedSum;

    static const UINT ChildCount = 2;
    BinaryClassifierDecisionTreeNode *_Children[ChildCount];
};

template<class LearnerInput>
class BinaryClassifierDecisionTree : public BinaryClassifier<LearnerInput>
{
public:
    BinaryClassifierDecisionTree()
    {
        _Configured = false;
    }

    BinaryClassifierDecisionTree(const DecisionTreeConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Configure(const DecisionTreeConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        Console::WriteLine(String("Training decision tree classifier, ") + String(Examples.Entries().Length()) + String(" examples"));
        PersistentAssert(_Configured, "Classifier not configured");
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;

        const UINT ExampleCount = Examples.Entries().Length();
        Vector<UINT> AllExampleIndices(ExampleCount);
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            AllExampleIndices[ExampleIndex] = ExampleIndex;
        }
        

        _Root.Train(_Config, Class0Index, Class1Index, Examples, AllExampleIndices, _Config.MaxTreeDepth);
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        _Root.Evaluate(Input, ProbabilityClass0);

        if(ProbabilityClass0 > 0.5)
        {
            Class = _Class0Index;
        }
        else
        {
            Class = _Class1Index;
        }
    }

private:
    UINT _Class0Index, _Class1Index;

    bool _Configured;
    DecisionTreeConfiguration _Config;

    BinaryClassifierDecisionTreeNode<LearnerInput> _Root;
};

template<class LearnerInput>
class BinaryClassifierFactoryDecisionTree : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactoryDecisionTree(const DecisionTreeConfiguration &Config)
    {
        _Config = Config;
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        BinaryClassifierDecisionTree<LearnerInput> *Result = new BinaryClassifierDecisionTree<LearnerInput>(_Config);
        return Result;
    }

private:
    DecisionTreeConfiguration _Config;
};
