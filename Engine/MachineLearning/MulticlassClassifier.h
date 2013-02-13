enum MulticlassClassifierType
{
    MulticlassClassifierTypeAdaBoostM1,
    MulticlassClassifierTypeDecisionTree,
    MulticlassClassifierTypeNearestNeighborANN,
    MulticlassClassifierTypeNearestNeighborBruteForce,
    MulticlassClassifierTypeOneVsAll,
    MulticlassClassifierTypePairwiseCoupling,
};

template<class LearnerInput>
MulticlassClassifier<LearnerInput>* MakeMulticlassClassifier(MulticlassClassifierType Type);

template<class LearnerInput>
class MulticlassClassifier
{
public:
    typedef ClassifierDataset<LearnerInput> Dataset;
    typedef ClassifierExample<LearnerInput> Example;

    virtual void Train(const Dataset &Examples) = 0;
    //virtual void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const = 0;

    virtual void Evaluate(const LearnerInput &Input, UINT &Class, Vector<double> &ClassProbabilities) const = 0;
    virtual void SaveToBinaryStream(OutputDataStream &Stream) const = 0;
    virtual void LoadFromBinaryStream(InputDataStream &Stream) = 0;
    virtual MulticlassClassifierType Type() const = 0;

    __forceinline void Evaluate(const LearnerInput &Input, UINT &Class) const
    {
        Vector<double> ClassProbabilities;
        Evaluate(Input, Class, ClassProbabilities);
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
        Vector<double> ClassProbabilities;
        for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            ClassificationResult NewResult;
            NewResult.TrueClass = CurExample.Class;
            Evaluate(CurExample.Input, NewResult.PredictedClass, ClassProbabilities);
            NewResult.ProbabilityClassN = ClassProbabilities[ClassIndex];
            Results[ExampleIndex] = NewResult;
        }

        const UINT ProbabilityDivisionCount = 100;
        os << "Probability Threshold\tProbability classification correct\tPercentage positives found\tPercentage negatives found" << endl;
        for(UINT ProbabilityDivision = 0; ProbabilityDivision < ProbabilityDivisionCount; ProbabilityDivision++)
        {
            double Threshold = Math::LinearMap(0.0, ProbabilityDivisionCount - 1.0, 0.0, 1.0, double(ProbabilityDivision));
            UINT ElementsPassingThreshold = 0, ElementsInClassPassingThreshold = 0, ElementsInClassNotPassingThreshold = 0;
            UINT ElementsInClass = 0, ElementsNotInClass = 0;
            for(UINT ExampleIndex = 0; ExampleIndex < ExampleCount; ExampleIndex++)
            {
                const ClassificationResult &CurResult = Results[ExampleIndex];
                if(CurResult.TrueClass == ClassIndex)
                {
                    ElementsInClass++;
                }
                else
                {
                    ElementsNotInClass++;
                }
                if(CurResult.ProbabilityClassN >= Threshold)
                {
                    ElementsPassingThreshold++;
                    if(CurResult.TrueClass == ClassIndex)
                    {
                        ElementsInClassPassingThreshold++;
                    }
                }
                /*else
                {
                    if(CurResult.TrueClass == ClassIndex)
                    {
                        ElementsInClassNotPassingThreshold++;
                    }
                }*/
            }
            double ProbabilityClassificationCorrect = double(ElementsInClassPassingThreshold) / double(ElementsPassingThreshold);
            double PercentagePositivesFound = double(ElementsInClassPassingThreshold) / double(ElementsInClass);
            double PercentageNegativesFound = double(ElementsPassingThreshold - ElementsInClassPassingThreshold) / double(ElementsNotInClass);
            if(ElementsPassingThreshold == 0.0)
            {
                ProbabilityClassificationCorrect = 1.0;
            }
            if(ElementsInClass == 0.0)
            {
                PercentagePositivesFound = 0.0;
            }
            if(ElementsNotInClass == 0.0)
            {
                PercentageNegativesFound = 0.0;
            }
            os << Threshold << '\t' << ProbabilityClassificationCorrect << '\t' << PercentagePositivesFound << '\t' << PercentageNegativesFound << endl;
        }
    }

    void DescribeConfusionMatrix(const Dataset &Examples, ostream &os) const
    {
        const UINT N = Examples.Entries().Length();
        const UINT C = Examples.ClassCount();
        Vector<double> ClassProbabilities(C);

        Grid<int> confusionMatrix(C, C, 0);
        Vector<int> priorCount(C, 0);

        for(UINT ExampleIndex = 0; ExampleIndex < N; ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            UINT Result;
            Evaluate(CurExample.Input, Result, ClassProbabilities);

            confusionMatrix(CurExample.Class, Result)++;
            priorCount[CurExample.Class]++;
        }

        os << "\t";
        for(UINT ClassIndex = 0; ClassIndex < C; ClassIndex++)
        {
            os << 'c' << ClassIndex << '\t';
        }
        os << "dist" << endl;

        for(UINT classIndex0 = 0; classIndex0 < C; classIndex0++)
        {
            os << 'c' << classIndex0 << '\t';
            for(UINT classIndex1 = 0; classIndex1 < C; classIndex1++)
            {
                os << confusionMatrix(classIndex0, classIndex1) << '\t';
            }
            os << priorCount[classIndex0] << endl;
        }
    }

    void DescribeDatasetClassificationError(const Dataset &Examples, ostream &os, bool DisplayAttributes) const
    {
        Vector<double> ClassProbabilities(Examples.ClassCount());

        os << "Class\tClassification\tConfidence\t";
        for(UINT ClassIndex = 0; ClassIndex < Examples.ClassCount(); ClassIndex++)
        {
            os << 'c' << ClassIndex << '\t';
        }

        if(DisplayAttributes)
        {
            for(UINT AttributeIndex = 0; AttributeIndex < Examples.AttributeCount(); AttributeIndex++)
            {
                os << 'a' << AttributeIndex << '\t';
            }
        }

        os << endl;

        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            UINT Result;
            Evaluate(CurExample.Input, Result, ClassProbabilities);

            os << CurExample.Class << '\t' << Result << '\t' << ClassProbabilities[Result] << '\t';
            for(UINT ClassIndex = 0; ClassIndex < Examples.ClassCount(); ClassIndex++)
            {
                os << ClassProbabilities[ClassIndex] << '\t';
            }

            if(DisplayAttributes)
            {
                for(UINT AttributeIndex = 0; AttributeIndex < Examples.AttributeCount(); AttributeIndex++)
                {
                    os << CurExample.Input[AttributeIndex] << '\t';
                }
            }
            os << endl;
        }
    }

    void Draw2DClassification(const Dataset &Examples, UINT DimensionIndex0, UINT DimensionIndex1, UINT BmpSize, Bitmap &Bmp) const
    {
        KMeansClustering<Vec3f, Vec3fKMeansMetric> ColorClusters;
        Vector<Vec3f> RandomColors(1000 * Examples.ClassCount());
        for(UINT ColorIndex = 0; ColorIndex < RandomColors.Length(); ColorIndex++)
        {
            Vec3f &CurColor = RandomColors[ColorIndex];
            CurColor = Vec3f(rnd(), rnd(), rnd());
            while(CurColor.x + CurColor.y + CurColor.z < 0.75f)
            {
                CurColor = Vec3f(rnd(), rnd(), rnd());
            }
        }
        ColorClusters.Cluster(RandomColors, Examples.ClassCount());

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
                
                UINT Class;
                Vector<double> ClassProbabilities;
                Evaluate(BaseExample.Input, Class, ClassProbabilities);
                
                RGBColor ClusterColor = RGBColor(ColorClusters.ClusterCenter(Class));
                Bmp[Y][X] = RGBColor::Interpolate(RGBColor::Black, ClusterColor, float(ClassProbabilities[Class]));
            }
        }

        AliasRender R;
        for(UINT ExampleIndex = 0; ExampleIndex < Examples.Entries().Length(); ExampleIndex++)
        {
            const Example &CurExample = Examples.Entries()[ExampleIndex];
            Vec2i CurImagePos(Math::Round(Math::LinearMap(BBox.Min.x, BBox.Max.x, 0.0f, Bmp.Width() - 1.0f, float(CurExample.Input[DimensionIndex0]))),
                              Math::Round(Math::LinearMap(BBox.Min.y, BBox.Max.y, 0.0f, Bmp.Height() - 1.0f, float(CurExample.Input[DimensionIndex1]))));
            R.DrawSquare(Bmp, CurImagePos, 4, RGBColor(ColorClusters.ClusterCenter(CurExample.Class)), RGBColor::Black);
            //Evaluate(CurExample.Input, Result);
        }
    }
};

template<class LearnerInput>
class MulticlassClassifierFactory
{
public:
    virtual MulticlassClassifier<LearnerInput>* MakeClassifier() const = 0;
};
