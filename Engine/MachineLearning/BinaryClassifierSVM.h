template<class LearnerInput>
class SVMKernel
{
public:
    virtual double InnerProduct(const LearnerInput &A, const LearnerInput &B) = 0;
};

template<class LearnerInput>
class SVMKernelLinear : public SVMKernel<LearnerInput>
{
public:
    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 0.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            Sum += A[Index] * B[Index];
        }
        return Sum;
    }
};

template<class LearnerInput>
class SVMKernelQuadratic : public SVMKernel<LearnerInput>
{
public:
    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 1.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            Sum += A[Index] * B[Index];
        }

        return Sum * Sum;
    }
};

template<class LearnerInput>
class SVMKernelCubic : public SVMKernel<LearnerInput>
{
public:
    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 1.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            Sum += A[Index] * B[Index];
        }

        return Sum * Sum * Sum;
    }
};

template<class LearnerInput>
class SVMKernelQuartic : public SVMKernel<LearnerInput>
{
public:
    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 1.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            Sum += A[Index] * B[Index];
        }

        double Sum2 = Sum * Sum;
        return Sum2 * Sum2;
    }
};

template<class LearnerInput>
class SVMKernelQuintic : public SVMKernel<LearnerInput>
{
public:
    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 1.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            Sum += A[Index] * B[Index];
        }

        double Sum2 = Sum * Sum;
        return Sum2 * Sum2 * Sum;
    }
};

template<class LearnerInput>
class SVMKernelGaussian : public SVMKernel<LearnerInput>
{
public:
    SVMKernelGaussian()
    {
        sigma = 0.25f;
    }

    double InnerProduct(const LearnerInput &A, const LearnerInput &B)
    {
        const UINT Length = A.Length();
        double Sum = 0.0;
        for(UINT Index = 0; Index < Length; Index++)
        {
            double Diff = B[Index] - A[Index];
            Sum += Diff * Diff;
        }

        return exp(-sigma * sqrt(Sum));
    }

    double sigma;
};

template<class LearnerInput>
struct SVMConfiguration
{
    SVMConfiguration()
    {
        C = 1.0;
        Tolerance = 0.01;
        MaxPasses = 10;
        AbsoluteMaxPasses = 20000;
    }

    SVMConfiguration(double _C, SVMKernel<LearnerInput> *_Kernel)
    {
        C = _C;
        Kernel = _Kernel;
        Tolerance = 0.01;
        MaxPasses = 10;
        AbsoluteMaxPasses = 20000;
    }


    double C; //regularization parameter
    double Tolerance;
    UINT MaxPasses;
    UINT AbsoluteMaxPasses;
    SVMKernel<LearnerInput> *Kernel;
};

struct SVMOutputEntry
{
    __forceinline UINT Length() const
    {
        return 1;
    }
    __forceinline double operator[] (int k) const
    {
        return Margin;
    }
    double Margin;
};

template<class LearnerInput>
class BinaryClassifierSVM : public BinaryClassifier<LearnerInput>
{
public:
    BinaryClassifierSVM()
    {
        _Configured = false;
    }

    BinaryClassifierSVM(const SVMConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Configure(const SVMConfiguration<LearnerInput> &Config)
    {
        _Configured = true;
        _Config = Config;
    }

    void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index)
    {
        Console::WriteLine(String("Training SVM classifier, ") + String(Examples.Entries().Length()) + String(" examples"));
        PersistentAssert(_Configured, "Classifier not configured");
        _Class0Index = Class0Index;
        _Class1Index = Class1Index;
        
        const UINT ExampleCount = Examples.Entries().Length();
        _TrainingSet = Examples;
        _LagrangeMultipliers.Allocate(ExampleCount);
        _LagrangeMultipliers.Clear(0.0);
        _Threshold = 0.0;

        //
        // Precompute all inner products
        //
        _AllInnerProducts.Allocate(ExampleCount, ExampleCount);
        for(UINT RowIndex = 0; RowIndex < ExampleCount; RowIndex++)
        {
            for(UINT ColIndex = 0; ColIndex < ExampleCount; ColIndex++)
            {
                const Example &RowExample = _TrainingSet.Entries()[RowIndex];
                const Example &ColExample = _TrainingSet.Entries()[ColIndex];
                _AllInnerProducts(RowIndex, ColIndex) = _Config.Kernel->InnerProduct(RowExample.Input, ColExample.Input);
            }
        }

        Console::AdvanceLine();
        UINT PassesWithoutChange = 0;
        UINT PassIndex = 0;
        while(PassesWithoutChange < _Config.MaxPasses && PassIndex < _Config.AbsoluteMaxPasses)
        {
            PassIndex++;
            UINT AlphasChangedThisPass = 0, InvalidAlphas = 0;
            
            for(UINT ExampleAIndex = 0; ExampleAIndex < ExampleCount; ExampleAIndex++)
            {
                UINT ExampleBIndex = rand() % ExampleCount;
                if(ExampleBIndex == ExampleAIndex)
                {
                    ExampleBIndex = (ExampleBIndex + 1) % ExampleCount;
                }
                bool ExampleAViolatesKKT, AlphasChanged;
                OptimizeVariablePair(ExampleAIndex, ExampleBIndex, ExampleAViolatesKKT, AlphasChanged);
                if(AlphasChanged)
                {
                    AlphasChangedThisPass++;
                }
                if(ExampleAViolatesKKT)
                {
                    InvalidAlphas++;
                }
            }

            if(PassIndex % 100 == 0 || InvalidAlphas == 0)
            {
                Console::OverwriteLine(String("Alphas changed=") + String(AlphasChangedThisPass) + String(", Invalid alphas=") + String(InvalidAlphas) + String(", Pass=") + String(PassIndex));
            }

            if(AlphasChangedThisPass == 0)
            {
                PassesWithoutChange++;
            }
            else
            {
                PassesWithoutChange = 0;
            }
            if(InvalidAlphas == 0)
            {
                PassesWithoutChange = _Config.MaxPasses;
            }
        }
        UINT SupportVectorCount = 0;
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            if(_LagrangeMultipliers[ExampleIndex] > 1e-10)
            {
                SupportVectorCount++;
            }
        }
        Console::WriteLine(String("Support vectors: ") + String(SupportVectorCount));

        TrainLogisticClassifier(Examples);
    }

    void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const
    {
        const double Margin = ComputeArbitraryFx(Input);

        if(Margin > 0.0)
        {
            Class = _Class1Index;
        }
        else
        {
            Class = _Class0Index;
        }
        
        SVMOutputEntry LogisticEntry;
        LogisticEntry.Margin = Margin;
        UINT LogisticResult;
        _Logistic.Evaluate(LogisticEntry, LogisticResult, ProbabilityClass0);
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
    void TrainLogisticClassifier(const Dataset &Examples)
    {
        ClassifierDataset<SVMOutputEntry> LogisticDataset(2);
        for(UINT SampleIndex = 0; SampleIndex < Examples.Entries().Length(); SampleIndex++)
        {
            ClassifierExample<SVMOutputEntry> NewEntry;
            NewEntry.Input.Margin = ComputeArbitraryFx(Examples.Entries()[SampleIndex].Input);
            NewEntry.Class = Examples.Entries()[SampleIndex].Class;
            LogisticDataset.AddEntry(NewEntry);
        }
        _Logistic.Train(LogisticDataset, _Class0Index, _Class1Index);
    }

    void OptimizeVariablePair(UINT ExampleAIndex, UINT ExampleBIndex, bool &ExampleAViolatesKKT, bool &AlphasChanged)
    {
        const Example &ExampleA = _TrainingSet.Entries()[ExampleAIndex];
        const UINT ExampleCount = _TrainingSet.Entries().Length();
        double OldLagrangeMultiplierA = _LagrangeMultipliers[ExampleAIndex];
        double ErrorA = ComputeTrainingSetFx(ExampleAIndex) - ExampleA.PositiveNegativeClass(_Class0Index);
     
        AlphasChanged = false;
        ExampleAViolatesKKT = (ExampleA.PositiveNegativeClass(_Class0Index) * ErrorA < -_Config.Tolerance && OldLagrangeMultiplierA < _Config.C) ||
                              (ExampleA.PositiveNegativeClass(_Class0Index) * ErrorA >  _Config.Tolerance && OldLagrangeMultiplierA > 0.0);
        if(ExampleAViolatesKKT)
        {
            const Example &ExampleB = _TrainingSet.Entries()[ExampleBIndex];
            double OldLagrangeMultiplierB = _LagrangeMultipliers[ExampleBIndex];
            double ErrorB = ComputeTrainingSetFx(ExampleBIndex) - ExampleB.PositiveNegativeClass(_Class0Index);

            double L;
            double H;
            if(ExampleA.Class == ExampleB.Class)
            {
                double LagrangeMultiplierSum = OldLagrangeMultiplierB + OldLagrangeMultiplierA;
                L = Math::Max(0.0, LagrangeMultiplierSum - _Config.C);
                H = Math::Min(_Config.C, LagrangeMultiplierSum);
            }
            else
            {
                double LagrangeMultiplierDiff = OldLagrangeMultiplierB - OldLagrangeMultiplierA;
                L = Math::Max(0.0, LagrangeMultiplierDiff);
                H = Math::Min(_Config.C, _Config.C + LagrangeMultiplierDiff);
            }
            if(L != H)
            {
                //double CrossTermsProduct = _Config.Kernel->InnerProduct(ExampleA.Input, ExampleB.Input);
                //double ExampleASelfProduct = _Config.Kernel->InnerProduct(ExampleA.Input, ExampleA.Input);
                //double ExampleBSelfProduct = _Config.Kernel->InnerProduct(ExampleB.Input, ExampleB.Input);
                
                double CrossTermsProduct = _AllInnerProducts(ExampleAIndex, ExampleBIndex);
                double ExampleASelfProduct = _AllInnerProducts(ExampleAIndex, ExampleAIndex);
                double ExampleBSelfProduct = _AllInnerProducts(ExampleBIndex, ExampleBIndex);

                double Eta = 2.0 * CrossTermsProduct - ExampleASelfProduct - ExampleBSelfProduct;
                if(Eta < 0.0)
                {
                    double NewLagrangeMultiplierB = OldLagrangeMultiplierB - ExampleB.PositiveNegativeClass(_Class0Index) * (ErrorA - ErrorB) / Eta;
                    NewLagrangeMultiplierB = Utility::Bound(NewLagrangeMultiplierB, L, H);
                    //if(Math::Abs(NewLagrangeMultiplierB - OldLagrangeMultiplierB) > 1e-8)
                    {
                        double NewLagrangeMultiplierA = OldLagrangeMultiplierA + ExampleA.PositiveNegativeClass(_Class0Index) * ExampleB.PositiveNegativeClass(_Class0Index) * (OldLagrangeMultiplierB - NewLagrangeMultiplierB);

                        double LagrangeMultiplierADelta = NewLagrangeMultiplierA - OldLagrangeMultiplierA;
                        double LagrangeMultiplierBDelta = NewLagrangeMultiplierB - OldLagrangeMultiplierB;
                        double b1 = _Threshold - ErrorA - ExampleA.PositiveNegativeClass(_Class0Index) * LagrangeMultiplierADelta * ExampleASelfProduct - ExampleB.PositiveNegativeClass(_Class0Index) * LagrangeMultiplierBDelta * CrossTermsProduct;
                        double b2 = _Threshold - ErrorB - ExampleA.PositiveNegativeClass(_Class0Index) * LagrangeMultiplierADelta * CrossTermsProduct - ExampleB.PositiveNegativeClass(_Class0Index) * LagrangeMultiplierBDelta * ExampleBSelfProduct;
                        
                        double NewThreshold;
                        if(between(NewLagrangeMultiplierA, 0.0, _Config.C))
                        {
                            NewThreshold = b1;
                        }
                        else if(between(NewLagrangeMultiplierB, 0.0, _Config.C))
                        {
                            NewThreshold = b2;
                        }
                        else
                        {
                            NewThreshold = (b1 + b2) * 0.5;
                        }

                        _LagrangeMultipliers[ExampleAIndex] = NewLagrangeMultiplierA;
                        _LagrangeMultipliers[ExampleBIndex] = NewLagrangeMultiplierB;
                        _Threshold = NewThreshold;
                        AlphasChanged = true;
                    }
                }
            }
        }
    }

    double ComputeArbitraryFx(const LearnerInput &x) const
    {
        double Sum = 0.0;
        const Example *Examples = _TrainingSet.Entries().CArray();
        const UINT ExampleCount = _TrainingSet.Entries().Length();
        const double *LagrangeMultipliers = _LagrangeMultipliers.CArray();
        const UINT MultiplierCount = _NonzeroLagrangeMultipliers.Length();
        if(MultiplierCount > 0)
        {
            const std::pair<double, UINT> *NonzeroMultipliers = _NonzeroLagrangeMultipliers.CArray();
            /*for(UINT MultiplierIndex = 0; MultiplierIndex < MultiplierCount; MultiplierIndex++)
            {
                const Example &CurExample = Examples[ExampleIndex];
                int y = CurExample.PositiveNegativeClass(_Class0Index);
                Sum += LagrangeMultiplier * y * _Config.Kernel->InnerProduct(x, CurExample.Input);
            }*/
        }
        else
        {
            for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
            {
                double LagrangeMultiplier = LagrangeMultipliers[ExampleIndex];
                if(LagrangeMultiplier > 1e-10)
                {
                    const Example &CurExample = Examples[ExampleIndex];
                    int y = CurExample.PositiveNegativeClass(_Class0Index);
                    Sum += LagrangeMultiplier * y * _Config.Kernel->InnerProduct(x, CurExample.Input);
                }
            }
        }
        return Sum + _Threshold;
    }

    double ComputeTrainingSetFx(UINT DatasetIndex) const
    {
        double Sum = 0.0;
        const Example *Examples = _TrainingSet.Entries().CArray();
        const UINT ExampleCount = _TrainingSet.Entries().Length();
        const double *LagrangeMultipliers = _LagrangeMultipliers.CArray();
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            double LagrangeMultiplier = LagrangeMultipliers[ExampleIndex];
            if(LagrangeMultiplier > 1e-10)
            {
                const Example &CurExample = Examples[ExampleIndex];
                int y = CurExample.PositiveNegativeClass(_Class0Index);
                Sum += LagrangeMultiplier * y * _AllInnerProducts(DatasetIndex, ExampleIndex);
            }
        }
        return Sum + _Threshold;
    }
    
    UINT _Class0Index, _Class1Index;

    Grid<double> _AllInnerProducts;

    bool _Configured;
    SVMConfiguration<LearnerInput> _Config;

    Dataset _TrainingSet;
    Vector<double> _LagrangeMultipliers;
    double _Threshold;

    Vector<pair<double, UINT> > _NonzeroLagrangeMultipliers;

    BinaryClassifierLogisticRegression<SVMOutputEntry> _Logistic;
};

template<class LearnerInput>
class BinaryClassifierFactorySVM : public BinaryClassifierFactory<LearnerInput>
{
public:
    BinaryClassifierFactorySVM() {}
    BinaryClassifierFactorySVM(const SVMConfiguration<LearnerInput> &Config)
    {
        _Config = Config;
    }
    void Configure(const SVMConfiguration<LearnerInput> &Config)
    {
        _Config = Config;
    }

    BinaryClassifier<LearnerInput>* MakeClassifier() const
    {
        BinaryClassifierSVM<LearnerInput> *Result = new BinaryClassifierSVM<LearnerInput>(_Config);
        return Result;
    }

private:
    SVMConfiguration<LearnerInput> _Config;
};
