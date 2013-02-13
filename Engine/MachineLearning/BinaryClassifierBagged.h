template<class LearnerInput>
struct BaggedConfiguration
{
    BaggedConfiguration() {}
    BaggedConfiguration(UINT _BagCount, UINT _BagSize, BinaryClassifierFactory<LearnerInput> *_Factory)
    {
        BagCount = _BagCount;
        BagSize = _BagSize;
        Factory = _Factory;
    }

    UINT BagCount, BagSize;
    BinaryClassifierFactory<LearnerInput> *Factory;
};

template<class LearnerInput>
class BinaryClassifierBagged : public BinaryClassifier<LearnerInput>
{
public:
    BinaryClassifierBagged()
    {
        _Configured = false;
    }
    BinaryClassifierBagged(const BaggedConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }
    void Configure(const BaggedConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        Console::WriteLine(String("Training bagged classifier, ") + String(Examples.Entries().Length()) + String(" examples, ") + String(_Config.BagCount) + String(" bags"));
        
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;
        
        Dataset NewDataset;
        _BaseClassifiers.Allocate(_Config.BagCount);
        for(UINT ClassifierIndex = 0; ClassifierIndex < _Config.BagCount; ClassifierIndex++)
        {
            NewDataset.SampleFromDataset(Examples, _Config.BagSize);
            BinaryClassifier<LearnerInput> *NewClassifier = _Config.Factory->MakeClassifier();
            NewClassifier->Train(NewDataset, Class0Index, Class1Index);
            _BaseClassifiers[ClassifierIndex] = NewClassifier;
        }
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        const UINT ClassCount = 2;
        UINT ClassVotes[ClassCount] = {0, 0};

        for(UINT ClassifierIndex = 0; ClassifierIndex < _Config.BagCount; ClassifierIndex++)
        {
            const BinaryClassifier<LearnerInput> &CurClassifier = *(_BaseClassifiers[ClassifierIndex]);
            UINT LocalClass;
            double LocalProbabilityClass0;
            CurClassifier.Evaluate(Input, LocalClass, LocalProbabilityClass0);
            if(LocalProbabilityClass0 > 0.5)
            {
                ClassVotes[0]++;
            }
            else
            {
                ClassVotes[1]++;
            }
        }

        ProbabilityClass0 = double(ClassVotes[0]) / _Config.BagCount;
        if(ProbabilityClass0 > 0.5)
        {
            Class = _Class0Index;
        }
        else
        {
            Class = _Class1Index;
        }
    }

    UINT Class0Index() const
    {
        return _Class0Index;
    }

    UINT Class1Index() const
    {
        return _Class1Index;
    }

private:
    UINT _Class0Index, _Class1Index;
    
    Vector<BinaryClassifier<LearnerInput>*> _BaseClassifiers;

    bool _Configured;
    BaggedConfiguration<LearnerInput> _Config;
};

template<class LearnerInput>
class BinaryClassifierFactoryBagged : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactoryBagged(const BaggedConfiguration<LearnerInput> &Config)
    {
        _Config = Config;
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        BinaryClassifierBagged<LearnerInput> *Result = new BinaryClassifierBagged<LearnerInput>(_Config);
        return Result;
    }

private:
    BaggedConfiguration<LearnerInput> _Config;
};
