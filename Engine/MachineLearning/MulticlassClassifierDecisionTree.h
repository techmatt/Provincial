struct DecisionTreeConfiguration
{
    DecisionTreeConfiguration() {}
    DecisionTreeConfiguration(UINT _MaxTreeDepth, UINT _LeafNodeCountCutoff, UINT _TestsPerDimension, UINT _MaxTestExamples)
    {
        MaxTreeDepth = _MaxTreeDepth;
        LeafNodeCountCutoff = _LeafNodeCountCutoff;
        TestsPerDimension = _TestsPerDimension;
        MaxTestExamples = _MaxTestExamples;
    }

    DecisionTreeConfiguration(UINT _MaxTreeDepth)
    {
        MaxTreeDepth = _MaxTreeDepth;
        LeafNodeCountCutoff = 0;
        TestsPerDimension = 0;
        MaxTestExamples = 0xFFFFFFFF;
    }

    UINT MaxTreeDepth;
    UINT LeafNodeCountCutoff;
    UINT TestsPerDimension;
    UINT MaxTestExamples;
};

template<class LearnerInput>
class MulticlassClassifierDecisionTreeNode
{
public:
    typedef ClassifierDataset<LearnerInput> Dataset;
    typedef ClassifierExample<LearnerInput> Example;

    MulticlassClassifierDecisionTreeNode()
    {
        for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
        {
            _Children[ChildIndex] = NULL;
        }
        _ClassDistribution = NULL;
    }

    ~MulticlassClassifierDecisionTreeNode()
    {
        for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
        {
            if(_Children[ChildIndex] != NULL)
            {
                delete _Children[ChildIndex];
                _Children[ChildIndex] = NULL;
            }
        }
        if(_ClassDistribution)
        {
            delete[] _ClassDistribution;
            _ClassDistribution = NULL;
        }
    }

    void Evaluate(const LearnerInput &Input, Vector<double> &ClassProbabilities) const
    {
        if(Leaf())
        {
            memcpy(ClassProbabilities.CArray(), _ClassDistribution, sizeof(double) * ClassProbabilities.Length());
        }
        else
        {
            return _Children[ComputeChildIndex(Input)]->Evaluate(Input, ClassProbabilities);
        }
    }

    void Train(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT DepthRemaining)
    {
        bool MakeLeafNode = (DepthRemaining == 0 || ActiveExampleIndices.Length() <= Config.LeafNodeCountCutoff);
        if(!MakeLeafNode)
        {
            if(ChooseVariableAndThreshold(Config, AllExamples, ActiveExampleIndices))
            {
                for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
                {
                    TrainChild(Config, AllExamples, ActiveExampleIndices, ChildIndex, DepthRemaining);
                }
            }
            else
            {
                MakeLeafNode = true;
            }
        }
        if(MakeLeafNode)
        {
            InitLeafProbabilities(Config, AllExamples, ActiveExampleIndices);
        }
    }

    void Describe(ostream &os, UINT Depth, UINT ClassCount) const
    {
        for(UINT DepthIndex = 0; DepthIndex < Depth; DepthIndex++)
        {
            os << "  ";
        }
        if(Leaf())
        {
            os << "Leaf: {";
            for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
            {
                os << _ClassDistribution[ClassIndex];
                if(ClassIndex != ClassCount - 1)
                {
                    os << ", ";
                }
            }
            os << "}, ";
            os << _LeafExampleCount << " examples" << endl;
        }
        else
        {
            os << "Split on feature " << _DecisionAttributeIndex << " at " << _DecisionAttributeThreshold << endl;
            _Children[0]->Describe(os, Depth + 1, ClassCount);
            _Children[1]->Describe(os, Depth + 1, ClassCount);
        }
    }

    void SaveToBinaryStream(OutputDataStream &Stream, UINT ClassCount) const
    {
        if(Leaf())
        {
            Stream << UINT(1);
            Stream << _LeafExampleCount;
            for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
            {
                Stream << _ClassDistribution[ClassIndex];
            }
        }
        else
        {
            Stream << UINT(0);
            Stream << _DecisionAttributeIndex << _DecisionAttributeThreshold;
            for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
            {
                _Children[ChildIndex]->SaveToBinaryStream(Stream, ClassCount);
            }
        }
    }

    void LoadFromBinaryStream(InputDataStream &Stream, UINT ClassCount)
    {
        UINT IsLeaf;
        Stream >> IsLeaf;
        if(IsLeaf == 1)
        {
            Stream >> _LeafExampleCount;
            _ClassDistribution = new double[ClassCount];
            for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
            {
                Stream >> _ClassDistribution[ClassIndex];
            }
        }
        else
        {
            Stream >> _DecisionAttributeIndex >> _DecisionAttributeThreshold;
            for(UINT ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
            {
                _Children[ChildIndex] = new MulticlassClassifierDecisionTreeNode<LearnerInput>;
                _Children[ChildIndex]->LoadFromBinaryStream(Stream, ClassCount);
            }
        }
    }

private:
    __forceinline static double lnFunc(double X)
    {
        if (X < 1e-10)
        {
            return 0.0;
        }
        else
        {
            return X * log(X);
        }
    }

    static double EntropyConditionedOnRows(const Grid<double> &G)
    {
        double Result = 0.0, Total = 0.0;

        for (UINT Row = 0; Row < G.Rows(); Row++)
        {
            double RowSum = 0.0;
            for (UINT Col = 0; Col < G.Cols(); Col++)
            {
                double CurValue = G(Row, Col);
                Result += lnFunc(CurValue);
                RowSum += CurValue;
            }
            Result -= lnFunc(RowSum);
            Total += RowSum;
        }
        if(Total == 0.0)
        {
            return 0.0;
        }
        static const double log2 = log(2.0);
        return -Result / (Total * log2);
    }

    __forceinline UINT ComputeChildIndex(const LearnerInput &Input) const
    {
        if(Input[_DecisionAttributeIndex] < _DecisionAttributeThreshold)
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

    double FindBestThesholdFinite(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, Vector<UINT> &ActiveExampleIndices, UINT AttributeIndex, double &BestTheshold, bool &Success, Grid<double> &DistributionStorage) const
    {
        const UINT ClassCount = AllExamples.ClassCount();
        const UINT ExampleCount = ActiveExampleIndices.Length();
        
        const Example *Examples =  AllExamples.Entries().CArray();
        const UINT *Indices = ActiveExampleIndices.CArray();

        double BestValue = 1e100;
        double SmallestValue = Examples[Indices[0]].Input[AttributeIndex];
        double LargestValue = Examples[Indices[0]].Input[AttributeIndex];
        for(UINT ExampleIndex = 1; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const double CurrentValue = Examples[Indices[ExampleIndex]].Input[AttributeIndex];
            SmallestValue = Math::Min(SmallestValue, CurrentValue);
            LargestValue = Math::Max(LargestValue, CurrentValue);
        }

        Success = Math::Abs(SmallestValue - LargestValue) > 1e-8;
        if(!Success)
        {
            return 0.0;
        }

        BestTheshold = (SmallestValue + LargestValue) * 0.5;
        
        for (UINT SplitIndex = 0; SplitIndex < Config.TestsPerDimension; SplitIndex++)
        {
            double CandidateTheshold = Math::LinearMap(0.0, Config.TestsPerDimension + 1.0, SmallestValue, LargestValue, SplitIndex + 1.0);

            DistributionStorage.Clear(0.0);
            for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
            {
                const Example &CurExample = Examples[Indices[ExampleIndex]];
                if(CurExample.Input[AttributeIndex] < CandidateTheshold)
                {
                    DistributionStorage(0, CurExample.Class) += CurExample.Weight;
                }
                else
                {
                    DistributionStorage(1, CurExample.Class) += CurExample.Weight;
                }
            }
        
            double CurValue = EntropyConditionedOnRows(DistributionStorage);
            if (CurValue < BestValue)
            {
                BestValue = CurValue;
                BestTheshold = CandidateTheshold;
            }
        }

        return BestValue;
    }

    double FindBestThesholdExhaustive(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, Vector<UINT> &ActiveExampleIndices, UINT AttributeIndex, double &BestTheshold, bool &Success) const
    {
        const UINT ClassCount = AllExamples.ClassCount();
        struct ActiveExampleSorter
        {
            bool operator() (UINT L, UINT R)
            {
                return (AllExamples->Entries()[L].Input[AttributeIndex] < AllExamples->Entries()[R].Input[AttributeIndex]);
            }
            UINT AttributeIndex;
            const Dataset *AllExamples;
        };
        ActiveExampleSorter Sorter;
        Sorter.AttributeIndex = AttributeIndex;
        Sorter.AllExamples = &AllExamples;

        ActiveExampleIndices.Sort<ActiveExampleSorter>(Sorter);
        
        double BestValue = 1e100;
        double SmallestValue = AllExamples.Entries()[ActiveExampleIndices.First()].Input[AttributeIndex];
        double LargestValue = AllExamples.Entries()[ActiveExampleIndices.Last()].Input[AttributeIndex];
        Success = Math::Abs(SmallestValue - LargestValue) > 1e-8;
        if(!Success)
        {
            return 0.0;
        }
        BestTheshold = (SmallestValue + LargestValue) * 0.5;
        //Vector<double> Sum(ClassCount);
        //Grid<double> BestDistribution(2, ClassCount);
        Grid<double> CurDistribution(2, ClassCount);
        CurDistribution.Clear(0.0);
        
        // Compute counts for all the values
        for (UINT ExampleIndex = 0; ExampleIndex < ActiveExampleIndices.Length(); ExampleIndex++)
        {
            const Example &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            CurDistribution(1, CurExample.Class) += CurExample.Weight;
        }
        //Sum = CurDistribution.ExtractRow(1);
        //BestDistribution = CurDistribution;
        
        // Make split counts for each possible split and evaluate
        for (UINT ExampleIndex = 0; ExampleIndex < ActiveExampleIndices.Length() - 1; ExampleIndex++)
        {
            const Example &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            const Example &NextExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex + 1]];
            CurDistribution(0, CurExample.Class) += CurExample.Weight;
            CurDistribution(1, CurExample.Class) -= CurExample.Weight;
            if (CurExample.Input[AttributeIndex] < NextExample.Input[AttributeIndex])
            {
                double CandidateTheshold = (CurExample.Input[AttributeIndex] + NextExample.Input[AttributeIndex]) * 0.5;
                double CurValue = EntropyConditionedOnRows(CurDistribution);
                if (CurValue < BestValue)
                {
                    BestValue = CurValue;
                    BestTheshold = CandidateTheshold;
                    //BestDistribution = CurDistribution;
                }
            }
        }

        return BestValue;
    }

    bool ChooseVariableAndThreshold(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        const UINT ClassCount = AllExamples.ClassCount();
        const UINT AttributeCount = AllExamples.AttributeCount();
        const UINT ExampleCount = ActiveExampleIndices.Length();

        int BestAttributeIndex = -1;
        double BestThreshold = 0.0;
        double BestValue;
        //Grid<double> BestDistributions(ChildCount, ClassCount);
        //Grid<double> CurDistributions(ChildCount, ClassCount);
        //BestDistributions.Clear(0.0);

        Grid<double> DistributionStorage(2, ClassCount);

        Vector<UINT> MutableActiveExampleIndices = ActiveExampleIndices;
        if(MutableActiveExampleIndices.Length() > Config.MaxTestExamples)
        {
            MutableActiveExampleIndices.Randomize();
            MutableActiveExampleIndices.ReSize(Config.MaxTestExamples);
        }
        for(UINT CandidateAttributeIndex = 0; CandidateAttributeIndex < AttributeCount; CandidateAttributeIndex++)
        {
            double CandidateTheshold, CurValue;
            bool Success;
            if(Config.TestsPerDimension == 0)
            {
                CurValue = FindBestThesholdExhaustive(Config, AllExamples, MutableActiveExampleIndices, CandidateAttributeIndex, CandidateTheshold, Success);
            }
            else
            {
                CurValue = FindBestThesholdFinite(Config, AllExamples, MutableActiveExampleIndices, CandidateAttributeIndex, CandidateTheshold, Success, DistributionStorage);
            }
            if(Success && (BestAttributeIndex == -1 || CurValue < BestValue))
            {
                BestValue = CurValue;
                BestAttributeIndex = CandidateAttributeIndex;
                BestThreshold = CandidateTheshold;
                //BestDistributions = CurDistributions;
            }
        }

        _DecisionAttributeIndex = BestAttributeIndex;
        _DecisionAttributeThreshold = BestThreshold;

        return (BestAttributeIndex != -1);
    }

    void InitLeafProbabilities(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices)
    {
        UINT ClassCount = AllExamples.ClassCount();
        _ClassDistribution = new double[ClassCount];
        for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
        {
            _ClassDistribution[ClassIndex] = 0.0;
        }

        const UINT ExampleCount = ActiveExampleIndices.Length();
        double TotalWeightedSum = 0.0;
        for (UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const Example &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            TotalWeightedSum += CurExample.Weight;
            _ClassDistribution[CurExample.Class] += CurExample.Weight;
        }

        if(TotalWeightedSum != 0.0)
        {
            double NomormalizeTerm = 1.0 / TotalWeightedSum;
            for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
            {
                _ClassDistribution[ClassIndex] *= NomormalizeTerm;
            }
        }

        _LeafExampleCount = ActiveExampleIndices.Length();
    }

    void TrainChild(const DecisionTreeConfiguration &Config, const Dataset &AllExamples, const Vector<UINT> &ActiveExampleIndices, UINT ChildIndex, UINT DepthRemaining)
    {
        Vector<UINT> ChildActiveExampleIndices;
        const UINT ExampleCount = ActiveExampleIndices.Length();
        for (UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const Example &CurExample = AllExamples.Entries()[ActiveExampleIndices[ExampleIndex]];
            if(ComputeChildIndex(CurExample.Input) == ChildIndex)
            {
                ChildActiveExampleIndices.PushEnd(ActiveExampleIndices[ExampleIndex]);
            }
        }

        _Children[ChildIndex] = new MulticlassClassifierDecisionTreeNode<LearnerInput>;
        _Children[ChildIndex]->Train(Config, AllExamples, ChildActiveExampleIndices, DepthRemaining - 1);
    }

    UINT _DecisionAttributeIndex;
    double _DecisionAttributeThreshold;
    
    double *_ClassDistribution;
    UINT _LeafExampleCount;
    
    static const UINT ChildCount = 2;
    MulticlassClassifierDecisionTreeNode<LearnerInput> *_Children[ChildCount];
};

template<class LearnerInput>
class MulticlassClassifierDecisionTree : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierDecisionTree()
    {
        _Configured = false;
    }

    MulticlassClassifierDecisionTree(const DecisionTreeConfiguration &Config)
    {
        _Configured = true;

        _Config = Config;
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypeDecisionTree;
    }

    void Configure(const DecisionTreeConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples)
    {
        Console::WriteLine(String("Training multiclass decision tree classifier, ") + String(Examples.Entries().Length()) + String(" examples, depth ") + String(_Config.MaxTreeDepth));
        PersistentAssert(_Configured, "Classifier not configured");
        
        _ClassCount = Examples.ClassCount();
        const UINT ExampleCount = Examples.Entries().Length();
        Vector<UINT> AllExampleIndices(ExampleCount);
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            AllExampleIndices[ExampleIndex] = ExampleIndex;
        }
        

        _Root.Train(_Config, Examples, AllExampleIndices, _Config.MaxTreeDepth);
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        if(ClassProbabilities.Length() != _ClassCount)
        {
            ClassProbabilities.Allocate(_ClassCount);
        }
        _Root.Evaluate(Input, ClassProbabilities);
        Class = ClassProbabilities.MaxIndex();
    }
    void DescribeTree(ostream &os) const
    {
        _Root.Describe(os, 0, _ClassCount);
    }

    void SaveToBinaryStream(OutputDataStream &Stream) const
    {
        PersistentAssert(_Configured, "Classifier not configured");
        Stream << UINT(Type());
        Stream.WriteData(_Config);
        Stream << _ClassCount;
        _Root.SaveToBinaryStream(Stream, _ClassCount);
    }

    void LoadFromBinaryStream(InputDataStream &Stream)
    {
        _Configured = true;
        Stream.ReadData(_Config);
        Stream >> _ClassCount;
        _Root.LoadFromBinaryStream(Stream, _ClassCount);
    }

private:
    bool _Configured;
    DecisionTreeConfiguration _Config;
    UINT _ClassCount;

    MulticlassClassifierDecisionTreeNode<LearnerInput> _Root;
};

template<class LearnerInput>
class MulticlassClassifierFactoryDecisionTree : public MulticlassClassifierFactory<LearnerInput>
{
public:
    MulticlassClassifierFactoryDecisionTree() {}
    MulticlassClassifierFactoryDecisionTree(const DecisionTreeConfiguration &Config)
    {
        _Config = Config;
    }
    void Configure(const DecisionTreeConfiguration &Config)
    {
        _Config = Config;
    }

    MulticlassClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifierDecisionTree<LearnerInput> *Result = new MulticlassClassifierDecisionTree<LearnerInput>(_Config);
        return Result;
    }

private:
    DecisionTreeConfiguration _Config;
};
