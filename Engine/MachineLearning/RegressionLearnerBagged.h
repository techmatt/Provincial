template<class LearnerInput, class LearnerOutput>
class BaggedNeighborLearner : public RegressionLearner<LearnerInput, LearnerOutput>
{
public:
    BaggedNeighborLearner(RegressionLearnerFactory<LearnerInput, LearnerOutput> *Factory, UINT BagCount)
    {
        _Factory = Factory;
        _BagCount = BagCount;
    }
    ~BaggedNeighborLearner()
    {
        FreeLearners();
        if(_Factory)
        {
            delete _Factory;
        }
    }
    void FreeLearners()
    {
        while(_BaseLearners.Length() > 0)
        {
            delete _BaseLearners.Last();
            _BaseLearners.PopEnd();
        }
    }

    void Train(const Dataset &Examples)
    {
        FreeLearners();

        Console::WriteLine(String("Bagged learner training on ") + String(Examples.Entries().Length()) + String(" samples..."));
        Console::AdvanceLine();
        _BaseLearners.Allocate(_BagCount);

        Dataset NewDataset;
        for(UINT learnerIndex = 0; learnerIndex < _BagCount; learnerIndex++)
        {
            Console::OverwriteLine(String("Bag ") + String(learnerIndex + 1) + String(" / ") + String(_BagCount));
            NewDataset.SampleFromDataset(Examples, Examples.Entries().Length());
            RegressionLearner<LearnerInput, LearnerOutput> *NewLearner = _Factory->MakeLearner();
            NewLearner->Train(NewDataset);
            _BaseLearners[learnerIndex] = NewLearner;
        }
    }
    void Evaluate(const LearnerInput &Input, LearnerOutput &Result) const
    {
        Result = 0.0;
        for(UINT learnerIndex = 0; learnerIndex < _BagCount; learnerIndex++)
        {
            LearnerOutput curResult;
            _BaseLearners[learnerIndex]->Evaluate(Input, curResult);
            Result += curResult;
        }
        Result *= LearnerOutput(1.0 / double(_BagCount));
    }

private:
    RegressionLearnerFactory<LearnerInput, LearnerOutput> *_Factory;
    UINT _BagCount;
    Vector<RegressionLearner<LearnerInput, LearnerOutput>*> _BaseLearners;
};