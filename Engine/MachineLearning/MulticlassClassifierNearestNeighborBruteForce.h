struct NearestNeighborConfiguration
{
    NearestNeighborConfiguration() {}
    NearestNeighborConfiguration(UINT _KNearest)
    {
        KNearest = _KNearest;
    }
    UINT KNearest;
};

template<class T>
class KNearestNeighborQueue
{
public:
    struct NeighborInfo
    {
        NeighborInfo() {}
        NeighborInfo(int _Index, T _Dist)
        {
            Index = _Index;
            Dist = _Dist;
        }
        int Index;
        T Dist;
    };

    void Init(UINT k)
    {
        _Queue.Allocate(k);
    }

    void Clear(T ClearValue)
    {
        _Queue.Clear(NeighborInfo(-1, ClearValue));
        _FarthestDist = ClearValue;
    }

    __forceinline void Insert(const NeighborInfo &Info)
    {
        if(Info.Dist < _FarthestDist)
        {
            _Queue.Last() = Info;
            const int QueueLength = _Queue.Length();
            if(QueueLength > 1)
            {
                NeighborInfo *QueueData = _Queue.CArray();
                for(int Index = QueueLength - 2; Index >= 0; Index--)
                {
                    if(QueueData[Index + 0].Dist > QueueData[Index + 1].Dist)
                    {
                        Utility::Swap(QueueData[Index + 0], QueueData[Index + 1]);
                    }
                }
            }
            _FarthestDist = _Queue.Last().Dist;
        }
    }

    const Vector<NeighborInfo>& Queue()
    {
        return _Queue;
    }

private:
    T _FarthestDist;
    Vector<NeighborInfo> _Queue;
};

template<class LearnerInput>
class MulticlassClassifierNearestNeighborBruteForce : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierNearestNeighborBruteForce()
    {
        _Configured = false;
    }

    MulticlassClassifierNearestNeighborBruteForce(const NearestNeighborConfiguration &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypeNearestNeighborBruteForce;
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
        _TrainingSet = Examples;

        _NeighborQueue.Init(_Config.KNearest);
        _ClassVotesStorage.Allocate(_ClassCount);

        Console::WriteLine("done.");
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        const UINT ExampleCount = _TrainingSet.Entries().Length();
        const UINT DimensionCount = Input.Length();
        
        _NeighborQueue.Clear(numeric_limits<double>::max());
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            double CurPointDistance = 0.0;
            const LearnerInput &TrainingInput = _TrainingSet.Entries()[ExampleIndex].Input;
            for(UINT DimensionIndex = 0; DimensionIndex < DimensionCount; DimensionIndex++)
            {
                double Diff = Input[DimensionIndex] - TrainingInput[DimensionIndex];
                CurPointDistance += Diff * Diff;
            }
            _NeighborQueue.Insert(KNearestNeighborQueue<double>::NeighborInfo(ExampleIndex, CurPointDistance));
        }

        _ClassVotesStorage.Clear(0);
        for(UINT NeighborIndex = 0; NeighborIndex < _Config.KNearest; NeighborIndex++)
        {
            UINT NeighborClass = _TrainingSet.Entries()[_NeighborQueue.Queue()[NeighborIndex].Index].Class;
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

    UINT _ClassCount;
    Dataset _TrainingSet;
    mutable KNearestNeighborQueue<double> _NeighborQueue;
    mutable Vector<UINT> _ClassVotesStorage;
};

template<class LearnerInput>
class MulticlassClassifierFactoryNearestNeighborBruteForce : public MulticlassClassifierFactory<LearnerInput>
{
public:
    MulticlassClassifierFactoryNearestNeighborBruteForce() {}
    MulticlassClassifierFactoryNearestNeighborBruteForce(const NearestNeighborConfiguration &Config)
    {
        _Config = Config;
    }
    void Configure(const NearestNeighborConfiguration &Config)
    {
        _Config = Config;
    }

    MulticlassClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifier<LearnerInput> *Result = new MulticlassClassifierNearestNeighborBruteForce<LearnerInput>(_Config);
        return Result;
    }

private:
    NearestNeighborConfiguration _Config;
};
