template<class LearnerInput> class MulticlassClassifier;
template<class LearnerInput> class MulticlassClassifierFactory;

template<class LearnerInput>
class BinaryClassifierMulticlass : public BinaryClassifier<LearnerInput>
{
public:
    BinaryClassifierMulticlass(MulticlassClassifier<LearnerInput> &Learner)
    {
        _Learner = &Learner;
    }

    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;
        _Learner->Train(Examples);
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        _Learner->Evaluate(Input, Class, _ClassProbabilitiesStorage);
        ProbabilityClass0 = _ClassProbabilitiesStorage[_Class0Index];
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
    mutable Vector<double> _ClassProbabilitiesStorage;
    MulticlassClassifier<LearnerInput> *_Learner;
};

template<class LearnerInput>
class BinaryClassifierFactoryMulticlass : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactoryMulticlass() {}
    BinaryClassifierFactoryMulticlass(const MulticlassClassifierFactory<LearnerInput> *MulticlassFactory)
    {
        _MulticlassFactory = MulticlassFactory;
    }
    void Configure(const MulticlassClassifierFactory<LearnerInput> *MulticlassFactory)
    {
        _MulticlassFactory =  MulticlassFactory;
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        MulticlassClassifier<LearnerInput> *MulticlassLearner = _MulticlassFactory->MakeClassifier();
        BinaryClassifierMulticlass<LearnerInput> *BinaryLearner = new BinaryClassifierMulticlass<LearnerInput>(*MulticlassLearner);
        return BinaryLearner;
    }

private:
    const MulticlassClassifierFactory<LearnerInput> *_MulticlassFactory;
};
