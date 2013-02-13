#ifdef USE_KDTREE

template<class LearnerInput>
class MulticlassClassifierNearestNeighborANN : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierNearestNeighborANN()
    {
        _Configured = false;
    }

    MulticlassClassifierNearestNeighborANN(const NearestNeighborConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypeNearestNeighborANN;
    }

    void Configure(const NearestNeighborConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples)
    {
        Console::WriteString(String("Training multiclass nearest neighbor classifier, ") + String(Examples.Entries().Length()) + String(" examples..."));
        PersistentAssert(_Configured, "Classifier not configured");

        _ClassCount = Examples.ClassCount();
        const UINT ExampleCount = Examples.Entries().Length();
        const UINT DimensionCount = Examples.AttributeCount();

        //_TrainingSet = Examples;
        _TrainingSetClass.Allocate(Examples.Entries().Length());
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            _TrainingSetClass[ExampleIndex] = Examples.Entries()[ExampleIndex].Class;
        }

        Vector<double> AllPointData(ExampleCount * DimensionCount);
        Vector<double *> Points(ExampleCount);
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            Points[ExampleIndex] = &AllPointData[ExampleIndex * DimensionCount];
            for(UINT DimensionIndex = 0; DimensionIndex < DimensionCount; DimensionIndex++)
            {
                AllPointData[ExampleIndex * DimensionCount + DimensionIndex] = Examples.Entries()[ExampleIndex].Input[DimensionIndex];
            }
        }

        _TrainingSetKDTree.BuildTree(Points, DimensionCount, _Config.KNearest);

        _QueryPointStorage.Allocate(Examples.AttributeCount());
        _NeighborIndicesStorage.Allocate(_Config.KNearest);
        _ClassVotesStorage.Allocate(Examples.ClassCount());

        Console::WriteLine("done.");
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        for(UINT DimensionIndex = 0; DimensionIndex < Input.Length(); DimensionIndex++)
        {
            _QueryPointStorage[DimensionIndex] = Input[DimensionIndex];
        }

        _TrainingSetKDTree.KNearest(_QueryPointStorage.CArray(), _Config.KNearest, _NeighborIndicesStorage, 0.0f);

        _ClassVotesStorage.Clear(0);
        for(UINT NeighborIndex = 0; NeighborIndex < _Config.KNearest; NeighborIndex++)
        {
            //UINT NeighborClass = _TrainingSet.Entries()[_NeighborIndicesStorage[NeighborIndex]].Class;
            UINT NeighborClass = _TrainingSetClass[_NeighborIndicesStorage[NeighborIndex]];
            _ClassVotesStorage[NeighborClass]++;
        }

        UINT MaxVotesIndex = 0;
        UINT MaxVotes = _ClassVotesStorage[0];
        if(ClassProbabilities.Length() != _ClassCount)
        {
            ClassProbabilities.Allocate(_ClassCount);
        }
        for(UINT ClassIndex = 0; ClassIndex < _ClassVotesStorage.Length(); ClassIndex++)
        {
            if(_ClassVotesStorage[ClassIndex] > MaxVotes)
            {
                MaxVotes = _ClassVotesStorage[ClassIndex];
                MaxVotesIndex = ClassIndex;
            }
            ClassProbabilities[ClassIndex] = double(_ClassVotesStorage[ClassIndex]) / double(_Config.KNearest);
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
    NearestNeighborConfiguration _Config;

    KDTree _TrainingSetKDTree;
    Vector<UINT> _TrainingSetClass;
    UINT _ClassCount;
    //Dataset _TrainingSet;

    mutable Vector<double> _QueryPointStorage;
    mutable Vector<UINT> _NeighborIndicesStorage;
    mutable Vector<UINT> _ClassVotesStorage;
};

template<class LearnerInput>
class MulticlassClassifierFactoryNearestNeighborANN : public MulticlassClassifierFactory<LearnerInput>
{
public:
    MulticlassClassifierFactoryNearestNeighborANN() {}
    MulticlassClassifierFactoryNearestNeighborANN(const NearestNeighborConfiguration &Config)
    {
        _Config = Config;
    }
    void Configure(const NearestNeighborConfiguration &Config)
    {
        _Config = Config;
    }

    MulticlassClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifier<LearnerInput> *Result = new MulticlassClassifierNearestNeighborANN<LearnerInput>(_Config);
        return Result;
    }

private:
    NearestNeighborConfiguration _Config;
};

#endif