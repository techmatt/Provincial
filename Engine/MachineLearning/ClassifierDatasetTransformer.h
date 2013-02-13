template<class LearnerInput>
struct ClassifierDatasetTransformer
{
public:
    typedef ClassifierDataset<LearnerInput> Dataset;
    typedef ClassifierExample<LearnerInput> Example;

    virtual void Init(const Dataset &Examples) = 0;

    virtual UINT DatasetCount() = 0;
    virtual void MakeDataset(UINT Index, ClassifierDataset<Vector<double> > &Result) = 0;
};
