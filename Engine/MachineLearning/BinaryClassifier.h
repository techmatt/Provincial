template<class LearnerInput>
struct ClassifierExample
{
    ClassifierExample()
    {
        Weight = 1.0;
    }
    String ToString() const
    {
        String Result = "{";
        for(UINT Index = 0; Index < Input.Length(); Index++)
        {
            Result += String(Input[Index]);
            if(Index != Input.Length())
            {
                Result += String(" ");
            }
        }
        Result += String("} = ") + String(Class);
        return Result;
    }
    __forceinline int PositiveNegativeClass(UINT Class0Index) const
    {
        if(Class == Class0Index)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    __forceinline int ZeroOneClass(UINT Class0Index) const
    {
        if(Class == Class0Index)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    LearnerInput Input;
    UINT Class;
    double Weight;
};

template<class LearnerInput>
struct ClassifierDataset
{
public:
    ClassifierDataset()
    {
        _ClassCount = 0;
    }
    ClassifierDataset(UINT ClassCount)
    {
        _ClassCount = ClassCount;
    }

    void DescribePerFeaturePerClassDistribtion(ostream &os)
    {

    }

    void PartitionDataset(ClassifierDataset<LearnerInput> &OutputSet0, ClassifierDataset<LearnerInput> &OutputSet1, double Set0ToSet1Ratio, bool randomizePartition = true) const
    {
        OutputSet0._Entries.FreeMemory();
        OutputSet0._ClassCount = _ClassCount;
        OutputSet1._Entries.FreeMemory();
        OutputSet1._ClassCount = _ClassCount;
        for(UINT Index = 0; Index < _Entries.Length(); Index++)
        {
            if(randomizePartition)
            {
                if(rnd() < Set0ToSet1Ratio) OutputSet0._Entries.PushEnd(_Entries[Index]);
                else OutputSet1._Entries.PushEnd(_Entries[Index]);
            }
            else
            {
                if(double(Index) / double(_Entries.Length()) < Set0ToSet1Ratio) OutputSet0._Entries.PushEnd(_Entries[Index]);
                else OutputSet1._Entries.PushEnd(_Entries[Index]);
            }
        }
    }

    void SampleFromDataset(const ClassifierDataset<LearnerInput> &InputSet, UINT SamplesToTake)
    {
        _Entries.FreeMemory();
        _ClassCount = InputSet._ClassCount;
        for(UINT Index = 0; Index < SamplesToTake; Index++)
        {
            _Entries.PushEnd(InputSet.Entries().RandomElement());
        }
    }

    void SampleFromDataset(const ClassifierDataset<LearnerInput> &InputSet, UINT SamplesToTake, const Vector<double> &Weights)
    {
        _Entries.FreeMemory();
        _ClassCount = InputSet._ClassCount;

        Vector<double> Probabilities(SamplesToTake);
        double ProbabilitySum = 0.0, WeightSum = Weights.Sum();
        for (UINT SampleIndex = 0; SampleIndex < SamplesToTake; SampleIndex++)
        {
            ProbabilitySum += rnd();
            Probabilities[SampleIndex] = ProbabilitySum;
        }
        for(UINT ProbabilityIndex = 0; ProbabilityIndex < SamplesToTake; ProbabilityIndex++)
        {
            Probabilities[ProbabilityIndex] *= WeightSum / ProbabilitySum;
        }

        Probabilities.Last() = WeightSum;
        UINT NewSampleIndex = 0;
        UINT ActiveInputSample = 0;
        ProbabilitySum = 0.0;
        while (NewSampleIndex < SamplesToTake && ActiveInputSample < InputSet.Entries().Length())
        {
            ProbabilitySum += Weights[ActiveInputSample];
            while (NewSampleIndex < SamplesToTake &&
                   Probabilities[NewSampleIndex] <= ProbabilitySum)
            {
                _Entries.PushEnd(InputSet._Entries[ActiveInputSample]);
                NewSampleIndex++;
            }
            ActiveInputSample++;
        }
    }

    void SubclassFromDataset(const ClassifierDataset<LearnerInput> &InputSet, UINT Class0Index, UINT Class1Index)
    {
        _Entries.FreeMemory();
        _ClassCount = InputSet._ClassCount;
        for(UINT ExampleIndex = 0; ExampleIndex < InputSet.Entries().Length(); ExampleIndex++)
        {
            const ClassifierExample<LearnerInput> &CurExample = InputSet.Entries()[ExampleIndex];
            if(CurExample.Class == Class0Index || CurExample.Class == Class1Index)
            {
                AddEntry(CurExample);
            }
        }
    }

    UINT CountExamplesOfClass(UINT Class)
    {
        UINT Result = 0;
        for(UINT ExampleIndex = 0; ExampleIndex < _Entries.Length(); ExampleIndex++)
        {
            const ClassifierExample<LearnerInput> &CurExample = _Entries[ExampleIndex];
            if(CurExample.Class == Class)
            {
                Result++;
            }
        }
        return Result;
    }
    __forceinline UINT AttributeCount() const
    {
        return _Entries[0].Input.Length();
    }
    __forceinline UINT& ClassCount()
    {
        return _ClassCount;
    }
    __forceinline UINT ClassCount() const
    {
        return _ClassCount;
    }
    __forceinline void AddEntry(const ClassifierExample<LearnerInput> &NewEntry)
    {
        _Entries.PushEnd(NewEntry);
    }
    __forceinline const Vector<ClassifierExample<LearnerInput> >& Entries() const
    {
        return _Entries;
    }
    __forceinline Vector<ClassifierExample<LearnerInput> >& Entries()
    {
        return _Entries;
    }

private:
    UINT _ClassCount;
    Vector<ClassifierExample<LearnerInput> > _Entries;
};

template<class LearnerInput>
class BinaryClassifier
{
public:
    typedef ClassifierDataset<LearnerInput> Dataset;
    typedef ClassifierExample<LearnerInput> Example;

    virtual void Train(const Dataset &Examples, UINT Class0Index, UINT Class1Index) = 0;
    virtual void Evaluate(const LearnerInput &Input, UINT &Class, double &ProbabilityClass0) const = 0;
    virtual UINT Class0Index() const = 0;
    virtual UINT Class1Index() const = 0;

    __forceinline void Evaluate(const LearnerInput &Input, UINT &Class) const
    {
        double ProbabilityFirstClass;
        Evaluate(Input, Class, ProbabilityFirstClass);
    }

    int ClassificationError(const Example &E) const
    {
        UINT Result;
        Evaluate(E.Input, Result);
        if(Result == E.Class)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    double DatasetClassificationError(const Dataset &Examples) const
    {
        double ErrorSum = 0.0;
        for(UINT exampleIndex = 0; exampleIndex < Examples.Entries().Length(); exampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[exampleIndex];
            ErrorSum += ClassificationError(CurExample);
        }
        return ErrorSum / Examples.Entries().Length();
    }

    double DescribeDatasetClassificationError(const Dataset &Examples, ostream &os) const
    {
        double ErrorSum = 0.0;
        os << "Real Class\tPredicted Class\tConfidence" << endl;
        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];

            UINT Result;
            double ProbabilityClass0;
            Evaluate(CurExample.Input, Result, ProbabilityClass0);
            if(Result != Class0Index())
            {
                ProbabilityClass0 = 1.0 - ProbabilityClass0;
            }

            /*for(UINT inputIndex = 0; inputIndex < CurExample.Input.Length(); inputIndex++)
            {
                os << CurExample.Input[inputIndex] << '\t';
            }*/
            os << CurExample.Class << '\t' << Result << '\t' << ProbabilityClass0 << endl;

            ErrorSum += Math::Abs(double(Result) - double(CurExample.Class));
        }
        return ErrorSum / Examples.Entries().Length();
    }

    void MakeROCCurve(const Dataset &Examples, ostream &os, UINT ClassIndex) const
    {
        struct ClassificationResult
        {
            UINT TrueClass;
            UINT PredictedClass;
            double ProbabilityClassN;
        };
        const UINT ExampleCount = Examples.Entries().Length();
        Vector<ClassificationResult> Results(ExampleCount);
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            ClassificationResult NewResult;
            NewResult.TrueClass = CurExample.Class;
            double ProbabilityClass0;
            Evaluate(CurExample.Input, NewResult.PredictedClass, ProbabilityClass0);
            if(ClassIndex == Class0Index())
            {
                NewResult.ProbabilityClassN = ProbabilityClass0;
            }
            else
            {
                NewResult.ProbabilityClassN = 1.0 - ProbabilityClass0;
            }
            Results[ExampleIndex] = NewResult;
        }

        const UINT ProbabilityDivisionCount = 100;
        os << "Probability Threshold\tProbability classification correct\tPercentage positives found" << endl;
        for(UINT ProbabilityDivision = 0; ProbabilityDivision < ProbabilityDivisionCount; ProbabilityDivision++)
        {
            double Threshold = Math::LinearMap(0.0, ProbabilityDivisionCount - 1.0, 0.0, 1.0, double(ProbabilityDivision));
            UINT ElementsPassingThreshold = 0, ElementsInClassPassingThreshold = 0, ElementsInClassNotPassingThreshold = 0;
            for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
            {
                const ClassificationResult &CurResult = Results[ExampleIndex];
                if(CurResult.ProbabilityClassN >= Threshold)
                {
                    ElementsPassingThreshold++;
                    if(CurResult.TrueClass == ClassIndex)
                    {
                        ElementsInClassPassingThreshold++;
                    }
                }
                else
                {
                    if(CurResult.TrueClass == ClassIndex)
                    {
                        ElementsInClassNotPassingThreshold++;
                    }
                }
            }
            double ProbabilityClassificationCorrect = double(ElementsInClassPassingThreshold) / double(ElementsPassingThreshold);
            double PercentagePositivesFound = double(ElementsInClassPassingThreshold) / double(ElementsInClassNotPassingThreshold + ElementsInClassPassingThreshold);
            if(ElementsPassingThreshold == 0.0)
            {
                ProbabilityClassificationCorrect = 1.0;
            }
            if(ElementsInClassNotPassingThreshold + ElementsInClassPassingThreshold == 0.0)
            {
                PercentagePositivesFound = 0.0;
            }
            os << Threshold << '\t' << ProbabilityClassificationCorrect << '\t' << PercentagePositivesFound << endl;
        }
    }

    void Draw2DClassification(const Dataset &Examples, UINT DimensionIndex0, UINT DimensionIndex1, UINT Class0Index, UINT Class1Index, UINT BmpSize, Bitmap &Bmp) const
    {
        Rectangle2f BBox;
        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            Vec2f CurFunctionPos(float(CurExample.Input[DimensionIndex0]), float(CurExample.Input[DimensionIndex1]));
            if(ExampleIndex == 0)
            {
                BBox.Min = CurFunctionPos;
                BBox.Max = CurFunctionPos;
            }
            else
            {
                BBox.Min = Vec2f::Minimize(BBox.Min, CurFunctionPos);
                BBox.Max = Vec2f::Maximize(BBox.Max, CurFunctionPos);
            }
        }
        BBox = Rectangle2f::ConstructFromCenterVariance(BBox.Center(), BBox.Dimensions() * 0.6f);

        float AspectRatio = BBox.Dimensions().y / BBox.Dimensions().x;
        if(AspectRatio > 1.0f)
        {
            Bmp.Allocate(UINT(BmpSize / AspectRatio), BmpSize);
        }
        else
        {
            Bmp.Allocate(BmpSize, UINT(BmpSize * AspectRatio));
        }

        Example BaseExample = Examples.Entries()[0];
        for(UINT Y = 0; Y < Bmp.Height(); Y++)
        {
            for(UINT X = 0; X < Bmp.Width(); X++)
            {
                Vec2f CurFunctionPos(Math::LinearMap(0.0f, Bmp.Width() - 1.0f,  BBox.Min.x, BBox.Max.x, float(X)),
                                     Math::LinearMap(0.0f, Bmp.Height() - 1.0f, BBox.Min.y, BBox.Max.y, float(Y)));
                BaseExample.Input[DimensionIndex0] = CurFunctionPos.x;
                BaseExample.Input[DimensionIndex1] = CurFunctionPos.y;
                
                UINT Result;
                double ProbabilityFirstClass = 0.0;
                Evaluate(BaseExample.Input, Result, ProbabilityFirstClass);
                
                RGBColor C;
                if(Result == Class0Index)
                {
                    C = RGBColor::Interpolate(RGBColor::White, RGBColor::Blue, float(Utility::Bound(Math::LinearMap(0.5, 1.0, 0.0, 1.0, ProbabilityFirstClass), 0.0, 1.0)));
                } 
                else
                {
                    C = RGBColor::Interpolate(RGBColor::Red, RGBColor::Black, float(Utility::Bound(Math::LinearMap(0.0, 0.5, 0.0, 1.0, ProbabilityFirstClass), 0.0, 1.0)));
                }
                Bmp[Y][X] = C;
            }
        }

        AliasRender R;
        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            Vec2i CurImagePos(Math::Round(Math::LinearMap(BBox.Min.x, BBox.Max.x, 0.0f, Bmp.Width() - 1.0f, float(CurExample.Input[DimensionIndex0]))),
                              Math::Round(Math::LinearMap(BBox.Min.y, BBox.Max.y, 0.0f, Bmp.Height() - 1.0f, float(CurExample.Input[DimensionIndex1]))));
            if(CurExample.Class == Class0Index)
            {
                R.DrawSquare(Bmp, CurImagePos, 4, RGBColor::Blue, RGBColor::Black);
            }
            else
            {
                R.DrawSquare(Bmp, CurImagePos, 4, RGBColor::Red, RGBColor::Black);
            }
        }
    }
};

template<class LearnerInput>
class BinaryClassifierFactory
{
public:
    virtual BinaryClassifier<LearnerInput>* MakeClassifier() const = 0;
};
