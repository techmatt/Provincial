template<class LearnerInput, class LearnerOutput>
struct RegressionLearnerExample
{
    RegressionLearnerExample()
    {
        Weight = 1.0f;
    }

    LearnerInput Input;
    LearnerOutput Output;
    float Weight;
};

template<class LearnerInput, class LearnerOutput>
struct RegressionLearnerDataset
{
    void PartitionDataset(RegressionLearnerDataset<LearnerInput, LearnerOutput> &OutputSet0, RegressionLearnerDataset<LearnerInput, LearnerOutput> &OutputSet1, double Set0ToSet1Ratio) const
    {
        OutputSet0.Entries.FreeMemory();
        OutputSet1.Entries.FreeMemory();
        for(UINT Index = 0; Index < Entries.Length(); Index++)
        {
            if(rnd() < Set0ToSet1Ratio)
            {
                OutputSet0.Entries.PushEnd(Entries[Index]);
            }
            else
            {
                OutputSet1.Entries.PushEnd(Entries[Index]);
            }
        }
    }

    void SampleFromDataset(const RegressionLearnerDataset<LearnerInput, LearnerOutput> &InputSet, UINT SamplesToTake)
    {
        Entries.FreeMemory();
        for(UINT Index = 0; Index < SamplesToTake; Index++)
        {
            Entries.PushEnd(InputSet.Entries().RandomElement());
        }
    }

    Vector<RegressionLearnerExample<LearnerInput, LearnerOutput> > Entries;
};

template<class LearnerInput, class LearnerOutput>
class RegressionLearner
{
public:
    typedef RegressionLearnerDataset<LearnerInput, LearnerOutput> Dataset;
    typedef RegressionLearnerExample<LearnerInput, LearnerOutput> Example;

    virtual void Train(const Dataset &Examples) = 0;
    virtual void Evaluate(const LearnerInput &Input, LearnerOutput &Result) const = 0;

    double EstimationError(const Example &E) const
    {
        LearnerOutput Result;
        Evaluate(E.Input, Result);
        return Math::Abs(Result - E.Output);
    }

    double DatasetEstimationError(const Dataset &Examples) const
    {
        LearnerOutput Result;
        double ErrorSum = 0.0;
        for(UINT exampleIndex = 0; exampleIndex < Examples.Entries().Length(); exampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[exampleIndex];
            Evaluate(CurExample.Input, Result);
            ErrorSum += Math::Abs(Result - CurExample.Output);
        }
        return ErrorSum / Examples.Entries().Length();
    }

    double DescribeDatasetEstimationError(const Dataset &Examples, ostream &os) const
    {
        LearnerOutput Result;
        double ErrorSum = 0.0;
        for(UINT exampleIndex = 0; exampleIndex < Examples.Entries.Length(); exampleIndex++)
        {
            const Example &CurExample = Examples.Entries[exampleIndex];
            Evaluate(CurExample.Input, Result);

            for(UINT inputIndex = 0; inputIndex < CurExample.Input.Length(); inputIndex++)
            {
                os << CurExample.Input[inputIndex] << '\t';
            }
            os << CurExample.Output << '\t' << Result << endl;

            ErrorSum += fabs(Result - CurExample.Output);
        }
        return ErrorSum / Examples.Entries.Length();
    }
};

template<class LearnerInput, class LearnerOutput>
class RegressionLearnerFactory
{
public:
    virtual RegressionLearner<LearnerInput, LearnerOutput>* MakeLearner() = 0;
};