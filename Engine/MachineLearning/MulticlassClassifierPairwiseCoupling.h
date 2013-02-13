template<class LearnerInput>
struct PairwiseCouplingConfiguration
{
    PairwiseCouplingConfiguration()
    {
        Factory = NULL;
    }
    PairwiseCouplingConfiguration(BinaryClassifierFactory<LearnerInput> *_Factory)
    {
        Factory = _Factory;
    }

    BinaryClassifierFactory<LearnerInput> *Factory;
};

template<class LearnerInput>
class MulticlassClassifierPairwiseCoupling : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierPairwiseCoupling()
    {
        _Configured = false;
    }

    ~MulticlassClassifierPairwiseCoupling()
    {
        FreeMemory();
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypePairwiseCoupling;
    }

    void FreeMemory()
    {
        for(UINT RowIndex = 0; RowIndex < _Classifiers.Rows(); RowIndex++)
        {
            for(UINT ColIndex = 0; ColIndex < _Classifiers.Cols(); ColIndex++)
            {
                BinaryClassifier<LearnerInput> *CurClassifier = _Classifiers(RowIndex, ColIndex);
                if(CurClassifier != NULL)
                {
                    delete CurClassifier;
                }
            }
        }
        _Classifiers.FreeMemory();
    }

    MulticlassClassifierPairwiseCoupling(const PairwiseCouplingConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Configure(const PairwiseCouplingConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples)
    {
        FreeMemory();
        Console::WriteLine(String("Training multiclass pairwise coupling classifier, ") + String(Examples.Entries().Length()) + String(" examples"));
        PersistentAssert(_Configured, "Classifier not configured");
        
        const UINT ExampleCount = Examples.Entries().Length();
        const UINT ClassCount = Examples.ClassCount();
        _ClassCount = ClassCount;
        _Classifiers.Allocate(Examples.ClassCount(), Examples.ClassCount());
        _Classifiers.Clear(NULL);

        for(UINT RowIndex = 0; RowIndex < ClassCount; RowIndex++)
        {
            for(UINT ColIndex = RowIndex + 1; ColIndex < ClassCount; ColIndex++)
            {
                _Classifiers(RowIndex, ColIndex) = _Config.Factory->MakeClassifier();
                BinaryClassifier<LearnerInput> &CurClassifier = *(_Classifiers(RowIndex, ColIndex));
                
                ClassifierDataset<LearnerInput> LocalDataset;
                LocalDataset.SubclassFromDataset(Examples, RowIndex, ColIndex);

                Console::WriteLine(String("Training ") +
                                   String(RowIndex) + String("(") + String(LocalDataset.CountExamplesOfClass(RowIndex)) + String(" samples) vs. ") +
                                   String(ColIndex) + String("(") + String(LocalDataset.CountExamplesOfClass(ColIndex)) + String(" samples)"));

                CurClassifier.Train(LocalDataset, RowIndex, ColIndex);
            }
        }
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        Assert(_Configured, "Classifier not configured");
        if(ClassProbabilities.Length() != _ClassCount)
        {
            ClassProbabilities.Allocate(_ClassCount);
        }

        const UINT ClassCount = _Classifiers.Rows();
        Vector<UINT> ClassVotes(ClassCount);
        Vector<double> ClassProbabilitySum(ClassCount);
        ClassVotes.Clear(0);
        ClassProbabilitySum.Clear(0.0);
        for(UINT RowIndex = 0; RowIndex < ClassCount; RowIndex++)
        {
            for(UINT ColIndex = RowIndex + 1; ColIndex < ClassCount; ColIndex++)
            {
                BinaryClassifier<LearnerInput> &CurClassifier = *(_Classifiers(RowIndex, ColIndex));
                double LocalProbabilityClass0;
                UINT LocalClass;
                CurClassifier.Evaluate(Input, LocalClass, LocalProbabilityClass0);
                ClassProbabilitySum[RowIndex] += LocalProbabilityClass0;
                ClassProbabilitySum[ColIndex] += 1.0 - LocalProbabilityClass0;
                if(LocalClass == RowIndex)
                {
                    ClassVotes[RowIndex]++;
                }
                else
                {
                    ClassVotes[ColIndex]++;
                }
            }
        }

        UINT MaxVotesIndex = 0;
        UINT MaxVotes = ClassVotes[0];
        if(ClassProbabilities.Length() != ClassCount)
        {
            ClassProbabilities.Allocate(ClassCount);
        }
        for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
        {
            if(ClassVotes[ClassIndex] > MaxVotes)
            {
                MaxVotes = ClassVotes[ClassIndex];
                MaxVotesIndex = ClassIndex;
            }
            //ClassProbabilities[ClassIndex] = double(ClassVotes[ClassIndex]) / double(ClassCount - 1);
            ClassProbabilities[ClassIndex] = ClassProbabilitySum[ClassIndex] / double(ClassCount - 1);
        }
        Class = MaxVotesIndex;
    }

    void SaveToBinaryStream(OutputDataStream &Stream) const
    {
        SignalError("Not implemented");
    }

    void LoadFromBinaryStream(InputDataStream &Stream)
    {
        SignalError("Not implemented");
    }

private:
    bool _Configured;
    PairwiseCouplingConfiguration<LearnerInput> _Config;
    Grid<BinaryClassifier<LearnerInput>*> _Classifiers;
    UINT _ClassCount;
};

template<class LearnerInput>
class MulticlassClassifierFactoryPairwiseCoupling : public MulticlassClassifierFactory<LearnerInput>
{
public:
    MulticlassClassifierFactoryPairwiseCoupling(const PairwiseCouplingConfiguration<LearnerInput> &Config)
    {
        _Config = Config;
    }

    MulticlassClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifier<LearnerInput> *Result = new MulticlassClassifierPairwiseCoupling<LearnerInput>(_Config);
        return Result;
    }

private:
    PairwiseCouplingConfiguration<LearnerInput> _Config;
};
