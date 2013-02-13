struct NaiveBayesFeatureEntry
{
    NaiveBayesFeatureEntry()
    {
        Class0Occurences = 1;
        Class1Occurences = 1;
    }
    UINT Class0Occurences;
    UINT Class1Occurences;
};

class NaiveBayesFeature
{
public:
    void RecordExample(UINT Value, bool IsClass0)
    {
        if(_Entries.Length() <= Value)
        {
            _Entries.ReSize(Value + 1);
        }
        if(IsClass0)
        {
            _Entries[Value].Class0Occurences++;
        }
        else
        {
            _Entries[Value].Class1Occurences++;
        }
    }
    __forceinline const Vector<NaiveBayesFeatureEntry>& Entries() const
    {
        return _Entries;
    }
    double NonzeroFeatureProbabilityGivenClass0() const
    {
        UINT TotalOccurences = 0;
        for(UINT EntryIndex = 0; EntryIndex < _Entries.Length(); EntryIndex++)
        {
            const NaiveBayesFeatureEntry &CurEntry = _Entries[EntryIndex];
            TotalOccurences += CurEntry.Class0Occurences;
        }
        return double(TotalOccurences - _Entries[0].Class0Occurences) / TotalOccurences;
    }
    double NonzeroFeatureProbabilityGivenClass1() const
    {
        UINT TotalOccurences = 0;
        for(UINT EntryIndex = 0; EntryIndex < _Entries.Length(); EntryIndex++)
        {
            const NaiveBayesFeatureEntry &CurEntry = _Entries[EntryIndex];
            TotalOccurences += CurEntry.Class1Occurences;
        }
        return double(TotalOccurences - _Entries[0].Class1Occurences) / TotalOccurences;
    }
    __forceinline UINT& Index()
    {
        return _Index;
    }
    __forceinline UINT Index() const
    {
        return _Index;
    }


private:
    UINT _Index;
    Vector<NaiveBayesFeatureEntry> _Entries;
};

__forceinline bool operator < (const NaiveBayesFeature &L, const NaiveBayesFeature &R)
{
    double LeftProbability = L.NonzeroFeatureProbabilityGivenClass1() / L.NonzeroFeatureProbabilityGivenClass0();
    double RightProbability = R.NonzeroFeatureProbabilityGivenClass1() / R.NonzeroFeatureProbabilityGivenClass0();
    return (LeftProbability > RightProbability);
}

template<class LearnerInput, class LearnerOutput>
class BinaryClassifierNaiveBayes : public BinaryClassifier<LearnerInput>
{
public:
    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        const UINT FeatureCount = Examples.Entries()[0].Input.Length();
        
        _Features.Allocate(FeatureCount);
        for(UINT FeatureIndex = 0; FeatureIndex < FeatureCount; FeatureIndex++)
        {
            _Features[FeatureIndex].Index() = FeatureIndex;
        }

        _Class0Occurences = 0;
        _Class1Occurences = 0;
        _TotalTrainingExamples = Examples.Entries().Length();
        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            if(CurExample.Class == Class0Index)
            {
                _PositiveClassOccurences++;
            }
            else
            {
                _NegativeClassOccurences++;
            }
            for(UINT FeatureIndex = 0; FeatureIndex < FeatureCount; FeatureIndex++)
            {
                NaiveBayesFeature &CurFeature = _Features[FeatureIndex];
                CurFeature.RecordExample(CurExample.Input[FeatureIndex], CurExample.Output);
            }
        }
        _Features.Sort();
    }

    void Evaluate(const LearnerInput &Input, LearnerOutput &Result, double &ProbabilityClass0) const
    {
        const UINT FeatureCount = _Features.Length();
        double PositiveClassProbability = log(_PositiveClassOccurences / _TotalTrainingExamples);
        double NegativeClassProbability = log(_NegativeClassOccurences / _TotalTrainingExamples);
        for(UINT FeatureIndex = 0; FeatureIndex < FeatureCount; FeatureIndex++)
        {
            const NaiveBayesFeature &CurFeature = _Features[FeatureIndex];
            const UINT EntryCount = CurFeature.Entries().Length();
            const UINT EntryIndex = Input[FeatureIndex];
            if(EntryIndex < EntryCount)
            {
                const NaiveBayesFeatureEntry &CurEntry = CurFeature.Entries()[EntryIndex];
                PositiveClassProbability += log(CurEntry.PositiveClassOccurences / (_PositiveClassOccurences + FeatureCount));
                NegativeClassProbability += log(CurEntry.NegativeClassOccurences / (_NegativeClassOccurences + FeatureCount));
            }
        }
        if(PositiveClassProbability - NegativeClassProbability > 0.0)
        {
            ProbabilityPositiveClass = PositiveClassProbability;
            Result = 1;
        }
        else
        {
            ProbabilityPositiveClass = 1.0 - NegativeClassProbability;
            Result = -1;
        }
    }
    const Vector<NaiveBayesFeature>& Features()
    {
        return _Features;
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
    Vector<NaiveBayesFeature> _Features;
    double _PositiveClassOccurences;
    double _NegativeClassOccurences;
    double _TotalTrainingExamples;
};