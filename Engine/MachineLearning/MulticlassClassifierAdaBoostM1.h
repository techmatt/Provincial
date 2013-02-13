template<class LearnerInput>
struct AdaBoostM1Configuration
{
    AdaBoostM1Configuration() {}
    AdaBoostM1Configuration(UINT _BoostCount, UINT _BagSize, bool _UseWeightedClassifiers, MulticlassClassifierFactory<LearnerInput> *_Factory)
    {
        BoostCount = _BoostCount;
        BagSize = _BagSize;
        Factory = _Factory;
        UseWeightedClassifiers = _UseWeightedClassifiers;
    }

    UINT BoostCount, BagSize;
    bool UseWeightedClassifiers;
    MulticlassClassifierFactory<LearnerInput> *Factory;
};

template<class LearnerInput>
class MulticlassClassifierAdaBoostM1 : public MulticlassClassifier<LearnerInput>
{
public:
    MulticlassClassifierAdaBoostM1()
    {
        _Configured = false;
    }

    MulticlassClassifierAdaBoostM1(const AdaBoostM1Configuration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    MulticlassClassifierType Type() const
    {
        return MulticlassClassifierTypeAdaBoostM1;
    }

    void Configure(const AdaBoostM1Configuration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void SetBaseClassifiersToUse(UINT BaseClassifiersToUse)
    {
        _BaseClassifiersToUse = BaseClassifiersToUse;
    }

    void Train(const Dataset &Examples)
    {
        Console::WriteLine(String("Training AdaBoostM1 classifier, ") + String(Examples.Entries().Length()) + String(" examples, ") + String(_Config.BoostCount) + String(" classifiers"));
        _ClassCount = Examples.ClassCount();

        if(_Config.BagSize > Examples.Entries().Length())
        {
            _Config.BagSize = Examples.Entries().Length();
        }
        
        Dataset NewDataset;
        _BaseClassifiers.Allocate(_Config.BoostCount);
        const UINT SampleCount = Examples.Entries().Length();

        Vector<double> SampleWeights(SampleCount);
        Vector<double> ClassProbabilityStorage;
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
            AdaBoostM1ClassifierInfo &CurInfo = _BaseClassifiers[ClassifierIndex];

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
            CurInfo.Classifier->Train(NewDataset);

            double Epsilon = 0.0;
            for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
            {
                const Example &CurExample = Examples.Entries()[SampleIndex];
                UINT PredictedClass;
                CurInfo.Classifier->Evaluate(CurExample.Input, PredictedClass, ClassProbabilityStorage);
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

            double Alpha = log((1.0 - Epsilon) / Epsilon) + log(double(_ClassCount) - 1.0);
            //double Alpha = log((1.0 - Epsilon) / Epsilon);
            
            Console::WriteLine(String("Classifier ") + String(ClassifierIndex) + String(" / ") + String(_Config.BoostCount) +
                               String(", Epsilon=") + String(Epsilon) + String(", Alpha=") + String(Alpha));

            if(Epsilon <= 1e-20 || Epsilon >= 0.5)
            {
                Console::WriteLine("Aborting...");
                Done = true;
                continue;
            }

            CurInfo.Weight = Alpha;
            HypothesisWeightSum += Alpha;

            double CorrectFactor = exp(-Alpha);
            double IncorrectFactor = exp(Alpha);
            
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
        _BaseClassifiersToUse = _Config.BoostCount;
        Console::AdvanceLine();
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const
    {
        if(ClassProbabilities.Length() != _ClassCount)
        {
            ClassProbabilities.Allocate(_ClassCount);
        }
        ClassProbabilities.Clear(0.0);
        
        double RescaleTerm = 1.0;
        if(_BaseClassifiersToUse < _Config.BoostCount)
        {
            RescaleTerm = 0.0;
            for(UINT ClassifierIndex = 0; ClassifierIndex < _BaseClassifiersToUse; ClassifierIndex++)
            {
                RescaleTerm += _BaseClassifiers[ClassifierIndex].Weight;
            }
            RescaleTerm = 1.0 / RescaleTerm;
        }

        for(UINT ClassifierIndex = 0; ClassifierIndex < _BaseClassifiersToUse; ClassifierIndex++)
        {
            const AdaBoostM1ClassifierInfo &CurInfo = _BaseClassifiers[ClassifierIndex];
            UINT LocalClass;
            if(CurInfo.Classifier != NULL)
            {
                CurInfo.Classifier->Evaluate(Input, LocalClass, _ClassProbabilitiesStorage);
                ClassProbabilities[LocalClass] += CurInfo.Weight * RescaleTerm;
            }
        }

        Class = ClassProbabilities.MaxIndex();
        double MaxProbability = ClassProbabilities[Class];
        for(UINT ClassIndex = 0; ClassIndex < _ClassCount; ClassIndex++)
        {
            ClassProbabilities[ClassIndex] = exp(ClassProbabilities[ClassIndex] - MaxProbability);
        }
        ClassProbabilities.Scale(1.0 / ClassProbabilities.Sum());
    }

    void SaveToBinaryStream(OutputDataStream &Stream) const
    {
        PersistentAssert(_Configured, "Classifier not configured");
        Stream << UINT(Type());
        Stream.WriteData(_Config);
        Stream << _ClassCount << _BaseClassifiers.Length() << _BaseClassifiersToUse;
        for(UINT ClassifierIndex = 0; ClassifierIndex < _BaseClassifiers.Length(); ClassifierIndex++)
        {
            const AdaBoostM1ClassifierInfo &CurClassifierInfo = _BaseClassifiers[ClassifierIndex];
            if(CurClassifierInfo.Classifier == NULL)
            {
                Stream << double(0.0);
            }
            else
            {
                Stream << CurClassifierInfo.Weight;
                CurClassifierInfo.Classifier->SaveToBinaryStream(Stream);
            }
        }
    }

    void LoadFromBinaryStream(InputDataStream &Stream)
    {
        _Configured = true;
        Stream.ReadData(_Config);
        UINT BaseClassifierCount;
        Stream >> _ClassCount >> BaseClassifierCount >> _BaseClassifiersToUse;
        _BaseClassifiers.Allocate(BaseClassifierCount);
        for(UINT ClassifierIndex = 0; ClassifierIndex < _BaseClassifiers.Length(); ClassifierIndex++)
        {
            AdaBoostM1ClassifierInfo &CurClassifierInfo = _BaseClassifiers[ClassifierIndex];
            Stream >> CurClassifierInfo.Weight;
            if(CurClassifierInfo.Weight > 0.0)
            {
                CurClassifierInfo.Classifier = MakeMulticlassClassifierFromStream<LearnerInput>(Stream);
            }
            else
            {
                CurClassifierInfo.Classifier = NULL;
            }
        }
    }

private:
    mutable Vector<double> _ClassProbabilitiesStorage;

    struct AdaBoostM1ClassifierInfo
    {
        AdaBoostM1ClassifierInfo()
        {
            Classifier = NULL;
            Weight = 0.0;
        }
        MulticlassClassifier<LearnerInput> *Classifier;
        double Weight;
    };
    Vector<AdaBoostM1ClassifierInfo> _BaseClassifiers;

    UINT _BaseClassifiersToUse;

    UINT _ClassCount;
    bool _Configured;
    AdaBoostM1Configuration<LearnerInput> _Config;
};

template<class LearnerInput>
class MulticlassClassifierFactoryAdaBoostM1 : public MulticlassClassifierFactory<LearnerInput>
{
public:
    MulticlassClassifierFactoryAdaBoostM1(const AdaBoostM1Configuration<LearnerInput> &Config)
    {
        _Config = Config;
    }

    MulticlassClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifierAdaBoostM1<LearnerInput> *Result = new MulticlassClassifierAdaBoostM1<LearnerInput>(_Config);
        return Result;
    }

private:
    AdaBoostM1Configuration<LearnerInput> _Config;
};
