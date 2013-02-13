template<class LearnerInput, class LearnerOutput>
class RegressionLearnerNearestNeighbor : public RegressionLearner<LearnerInput, LearnerOutput>
{
public:
    void Train(const Dataset &Examples)
    {
        _TrainingSet = Examples;
    }
    void Evaluate(const LearnerInput &Input, LearnerOutput &Result) const
    {
        Result = _TrainingSet.Entries[0].Output;
        double ClosestDist = InputDistance(Input, _TrainingSet.Entries[0].Input);
        for(UINT trainingIndex = 0; trainingIndex < _TrainingSet.Entries.Length(); trainingIndex++)
        {
            const RegressionLearnerExample<LearnerInput, LearnerOutput> &CurEntry = _TrainingSet.Entries[trainingIndex];
            double CurDist = InputDistance(Input, CurEntry.Input);
            if(CurDist < ClosestDist)
            {
                ClosestDist = CurDist;
                Result = CurEntry.Output;
            }
        }
    }

private:
    double InputDistance(const LearnerInput &A, const LearnerInput &B) const
    {
        double Sum = 0.0;
        for(UINT Index = 0; Index < A.Length(); Index++)
        {
            double Diff = double(A[Index]) - double(B[Index]);
            Sum += Diff * Diff;
        }
        return Sum;
    }

    Dataset _TrainingSet;
};