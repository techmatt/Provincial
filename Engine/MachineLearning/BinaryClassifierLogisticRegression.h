template<class LearnerInput>
class BinaryClassifierLogisticRegression : public BinaryClassifier<LearnerInput>
{
public:
    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        Console::WriteLine(String("Training logistic regression classifier, ") + String(Examples.Entries().Length()) + String(" examples"));
        Console::AdvanceLine();
        _Thetas.Allocate(Examples.Entries()[0].Input.Length() + 1);
        _Thetas.Clear(0.0);
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;
        TrainBatchGradientDescent(Examples);
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        double Sum = 0.0;
        for(UINT ElementIndex = 0; ElementIndex < Input.Length(); ElementIndex++)
        {
            Sum += Input[ElementIndex] * _Thetas[ElementIndex];
        }
        Sum += _Thetas.Last();
        ProbabilityClass0 = 1.0 - Sigmoid(Sum);
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
    double Sigmoid(double x) const
    {
        return 1.0 / (1.0 + exp(-x));
    }
    double ComputeHypothesis(const LearnerInput &Input)
    {
        double Sum = 0.0;
        for(UINT ElementIndex = 0; ElementIndex < Input.Length(); ElementIndex++)
        {
            Sum += Input[ElementIndex] * _Thetas[ElementIndex];
        }
        Sum += _Thetas.Last();
        return Sigmoid(Sum);
    }

    void TrainBatchGradientDescent(const Dataset &Examples)
    {
        const UINT FeatureCount = _Thetas.Length();
        const UINT EntryCount = Examples.Entries().Length();
        Vector<double> Gradients(FeatureCount);

        bool Converged = false;
        UINT PassIndex = 0;
        double Alpha = 0.1;
        while(!Converged)
        {
            Gradients.Clear(0.0);
            PassIndex++;
            for(UINT SampleIndex = 0; SampleIndex < EntryCount; SampleIndex++)
            {
                const ClassifierExample<LearnerInput> &CurEntry = Examples.Entries()[SampleIndex];
                double Delta = double(CurEntry.ZeroOneClass(_Class0Index)) - ComputeHypothesis(CurEntry.Input);

                for(UINT FeatureIndex = 0; FeatureIndex < FeatureCount - 1; FeatureIndex++)
                {
                    Gradients[FeatureIndex] += Delta * CurEntry.Input[FeatureIndex];
                }
                Gradients[FeatureCount - 1] += Delta;
            }
            double Change = 0.0;
            for(UINT FeatureIndex = 0; FeatureIndex < FeatureCount; FeatureIndex++)
            {
                double DeltaTheta = Alpha * Gradients[FeatureIndex];
                _Thetas[FeatureIndex] += DeltaTheta;
                Change += fabs(DeltaTheta);
            }
            Alpha *= 0.995;
            if(Change < 1e-10)
            {
                Converged = true;
            }
            if(PassIndex % 1000 == 0 || Converged)
            {
                Console::OverwriteLine(String("Pass ") + String(PassIndex) + String(", Change=") + String(Change));
            }
        }
    }

    //
    // The last entry in _Thetas is the x=1 term
    //
    Vector<double> _Thetas;
    UINT _Class0Index, _Class1Index;
};

template<class LearnerInput>
class BinaryClassifierFactoryLogisticRegression : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactoryLogisticRegression()
    {
        
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        BinaryClassifierLogisticRegression<LearnerInput> *Result = new BinaryClassifierLogisticRegression<LearnerInput>;
        return Result;
    }
};
