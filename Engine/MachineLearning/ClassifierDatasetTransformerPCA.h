template<class LearnerInput>
struct ClassifierDatasetTransformerPCA : public ClassifierDatasetTransformer<LearnerInput>
{
public:
    void Init(const Dataset &Examples)
    {
        _Dataset = Examples;
        const UINT EntryCount = _Dataset.Entries().Length();
        const UINT AttributeCount = _Dataset.AttributeCount();
        
        Vector<const double*> Points(EntryCount);
        for(UINT EntryIndex = 0; EntryIndex < EntryCount; EntryIndex++)
        {
            const Example &CurExample = _Dataset.Entries()[EntryIndex];
            double *CurPoint = new double[AttributeCount];
            for(UINT AttributeIndex = 0; AttributeIndex < AttributeCount; AttributeIndex++)
            {
                CurPoint[AttributeIndex] = CurExample.Input[AttributeIndex];
            }
            Points[EntryIndex] = CurPoint;
        }

        _PCA.InitFromDensePoints(Points, AttributeCount);

        for(UINT EntryIndex = 0; EntryIndex < EntryCount; EntryIndex++)
        {
            delete[] Points[EntryIndex];
        }
    }

    virtual UINT DatasetCount()
    {
        return _Dataset.AttributeCount();
    }

    virtual void MakeDataset(UINT Index, ClassifierDataset<Vector<double> > &Result)
    {
        const UINT EntryCount = _Dataset.Entries().Length();
        const UINT AttributeCount = _Dataset.AttributeCount();

        Result.ClassCount() = _Dataset.ClassCount();
        Result.Entries().Allocate(EntryCount);

        Vector<double> UntransformedPoint(AttributeCount);
        for(UINT EntryIndex = 0; EntryIndex < EntryCount; EntryIndex++)
        {
            const Example &CurExample = _Dataset.Entries()[EntryIndex];
            Result.Entries()[EntryIndex].Class = CurExample.Class;
            Result.Entries()[EntryIndex].Weight = CurExample.Weight;
            for(UINT AttributeIndex = 0; AttributeIndex < AttributeCount; AttributeIndex++)
            {
                UntransformedPoint[AttributeIndex] = CurExample.Input[AttributeIndex];
            }
            _PCA.Transform(Result.Entries()[EntryIndex].Input, UntransformedPoint, Index + 1);
        }
    }

private:
    ClassifierDataset<LearnerInput> _Dataset;
    PCA<double> _PCA;
};
