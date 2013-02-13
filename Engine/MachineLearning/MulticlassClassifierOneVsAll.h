template<class LearnerInput>
struct OneVsAllConfiguration
{
    OneVsAllConfiguration()
    {
        Factory = NULL;
    }
    OneVsAllConfiguration(BinaryClassifierFactory<LearnerInput> *_Factory)
    {
        Factory = _Factory;
    }

    BinaryClassifierFactory<LearnerInput> *Factory;
};

template<class LearnerInput>
class MulticlassClassifierOneVsAll : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierOneVsAll()
    {
        _Configured = false;
    }

    MulticlassClassifierOneVsAll(const OneVsAllConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypeOneVsAll;
    }

    void Configure(const OneVsAllConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples)
    {
        Console::WriteLine(String("Training multiclass one-vs-all classifier, ") + String(Examples.Entries().Length()) + String(" examples"));
        PersistentAssert(_Configured, "Classifier not configured");
        
        const UINT ExampleCount = Examples.Entries().Length();
        const UINT ClassCount = Examples.ClassCount();
        _Classifiers.Allocate(Examples.ClassCount());

        for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
        {
            _Classifiers[ClassIndex] = _Config.Factory->MakeClassifier();
            BinaryClassifier<LearnerInput> &CurClassifier = *(_Classifiers[ClassIndex]);
            
            ClassifierDataset<LearnerInput> LocalDataset = Examples;
            LocalDataset.ClassCount() = ClassCount + 1;
            for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
            {
                Example &CurExample = LocalDataset.Entries()[ExampleIndex];
                if(CurExample.Class != ClassIndex)
                {
                    CurExample.Class = ClassCount;
                }
            }

            Console::WriteLine(String("Training classfier ") + String(ClassIndex) + String("(") + String(LocalDataset.CountExamplesOfClass(ClassIndex)) + String(" positive samples)"));

            CurClassifier.Train(LocalDataset, ClassIndex, ClassCount);
        }
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        const UINT ClassCount = _Classifiers.Length();
        if(ClassProbabilities.Length() != ClassCount)
        {
            ClassProbabilities.Allocate(ClassCount);
        }
        Class = 0;
        double MaxProbability = 0.0;
        for(UINT ClassIndex = 0; ClassIndex < ClassCount; ClassIndex++)
        {
            BinaryClassifier<LearnerInput> &CurClassifier = *(_Classifiers[ClassIndex]);
            UINT LocalClass;
            CurClassifier.Evaluate(Input, LocalClass, ClassProbabilities[ClassIndex]);
            if(ClassProbabilities[ClassIndex] > MaxProbability)
            {
                MaxProbability = ClassProbabilities[ClassIndex];
                Class = ClassIndex;
            }
        }
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
    OneVsAllConfiguration<LearnerInput> _Config;
    Vector<BinaryClassifier<LearnerInput>*> _Classifiers;
};
