template<class LearnerInput>
struct AdaBoostConfiguration
{
    AdaBoostConfiguration() {}
    AdaBoostConfiguration(UINT _BoostCount, UINT _BagSize, bool _UseWeightedClassifiers, BinaryClassifierFactory<LearnerInput> *_Factory)
    {
        BoostCount = _BoostCount;
        BagSize = _BagSize;
        Factory = _Factory;
        UseWeightedClassifiers = _UseWeightedClassifiers;
    }

    UINT BoostCount, BagSize;
    bool UseWeightedClassifiers;
    BinaryClassifierFactory<LearnerInput> *Factory;
};

template<class LearnerInput>
class BinaryClassifierAdaBoost : public BinaryClassifier<LearnerInput>
{
public:
    BinaryClassifierAdaBoost()
    {
        _Configured = false;
    }
    BinaryClassifierAdaBoost(const AdaBoostConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }
    void Configure(const AdaBoostConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        Console::WriteLine(String("Training boosted classifier, ") + String(Examples.Entries().Length()) + String(" examples, ") + String(_Config.BoostCount) + String(" classifiers"));

        if(_Config.BagSize > Examples.Entries().Length())
        {
            _Config.BagSize = Examples.Entries().Length();
        }
        
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;
        
        Dataset NewDataset;
        _BaseClassifiers.Allocate(_Config.BoostCount);
        const UINT SampleCount = Examples.Entries().Length();

        Vector<double> SampleWeights(SampleCount);
        Vector<UINT> ClassificationError(SampleCount);

        SampleWeights.Clear(1.0 / double(SampleCount));

        if(_Config.UseWeightedClassifiers)
        {
            NewDataset = Examples;
        }

        double HypothesisWeightSum = 0.0;
        bool Done = false;
        for(UINT ClassifierIndex = 0; ClassifierIndex < _Config.BoostCount && !Done; ClassifierIndex++)
        {
            AdaBoostClassifierInfo &CurInfo = _BaseClassifiers[ClassifierIndex];

            if(_Config.UseWeightedClassifiers)
            {
                for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
                {
                    NewDataset.Entries()[SampleIndex].Weight = SampleWeights[SampleIndex];
                }
            }
            else
            {
                NewDataset.SampleFromDataset(Examples, _Config.BagSize, SampleWeights);
            }

            CurInfo.Classifier = _Config.Factory->MakeClassifier();
            CurInfo.Classifier->Train(NewDataset, Class0Index, Class1Index);

            double Epsilon = 0.0;
            for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
            {
                const Example &CurExample = Examples.Entries()[SampleIndex];
                UINT PredictedClass;
                double PredictedClass0Probability;
                CurInfo.Classifier->Evaluate(CurExample.Input, PredictedClass, PredictedClass0Probability);
                if(PredictedClass == CurExample.Class)
                {
                    ClassificationError[SampleIndex] = 0;
                }
                else
                {
                    ClassificationError[SampleIndex] = 1;
                    Epsilon += SampleWeights[SampleIndex];
                }
            }

            double Alpha = 0.5 * log((1.0 - Epsilon) / Epsilon);
            double CorrectFactor = exp(-Alpha);
            double IncorrectFactor = exp(Alpha);

            Console::WriteLine(String("Epsilon=") + String(Epsilon) + String(", Alpha=") + String(Alpha));

            if(Epsilon <= 1e-20 || Epsilon >= 0.5)
            {
                //_Config.Factory->MakeClassifier()->Train(NewDataset, Class0Index, Class1Index);
                Console::WriteLine("Aborting...");
                Done = true;
                continue;
            }

            CurInfo.Weight = Alpha;
            HypothesisWeightSum += Alpha;

            Vector<double> NewSampleWeights(SampleCount);
            for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
            {
                if(ClassificationError[SampleIndex] == 0)
                {
                    NewSampleWeights[SampleIndex] = SampleWeights[SampleIndex] * CorrectFactor;
                }
                else
                {
                    NewSampleWeights[SampleIndex] = SampleWeights[SampleIndex] * IncorrectFactor;
                }
            }
            double NewSampleWeightsSum = NewSampleWeights.Sum();
            for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
            {
                SampleWeights[SampleIndex] = NewSampleWeights[SampleIndex] / NewSampleWeightsSum;
            }
        }

        if(HypothesisWeightSum == 0.0)
        {
            HypothesisWeightSum = 1.0;
            _BaseClassifiers[0].Weight = 1.0;
        }

        Console::WriteString("Final weights: ");
        for(UINT ClassifierIndex = 0; ClassifierIndex < _Config.BoostCount; ClassifierIndex++)
        {
            _BaseClassifiers[ClassifierIndex].Weight /= HypothesisWeightSum;
            Console::WriteString(String(_BaseClassifiers[ClassifierIndex].Weight) + String(" "));
        }
        Console::AdvanceLine();
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        double BoostedHypothesis = 0.0;
        for(UINT ClassifierIndex = 0; ClassifierIndex < _Config.BoostCount; ClassifierIndex++)
        {
            const AdaBoostClassifierInfo &CurInfo = _BaseClassifiers[ClassifierIndex];
            UINT LocalClass;
            double LocalProbabilityClass0;
            if(CurInfo.Classifier != NULL)
            {
                CurInfo.Classifier->Evaluate(Input, LocalClass, LocalProbabilityClass0);
                if(LocalClass == _Class0Index)
                {
                    BoostedHypothesis += CurInfo.Weight;
                }
            }
        }
        ProbabilityClass0 = BoostedHypothesis;
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
    
    struct AdaBoostClassifierInfo
    {
        AdaBoostClassifierInfo()
        {
            Classifier = NULL;
            Weight = 0.0;
        }
        BinaryClassifier<LearnerInput> *Classifier;
        double Weight;
    };
    Vector<AdaBoostClassifierInfo> _BaseClassifiers;

    bool _Configured;
    AdaBoostConfiguration<LearnerInput> _Config;
};

template<class LearnerInput>
class BinaryClassifierFactoryAdaBoost : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactoryAdaBoost(const AdaBoostConfiguration<LearnerInput> &Config)
    {
        _Config = Config;
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        BinaryClassifierAdaBoost<LearnerInput> *Result = new BinaryClassifierAdaBoost<LearnerInput>(_Config);
        return Result;
    }

private:
    AdaBoostConfiguration<LearnerInput> _Config;
};
