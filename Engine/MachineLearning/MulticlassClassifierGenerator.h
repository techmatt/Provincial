template<class LearnerInput>
MulticlassClassifier<LearnerInput>* MakeMulticlassClassifier(MulticlassClassifierType Type)
{
    switch(Type)
    {
    /*case MulticlassClassifierTypeAdaBoostM1:
        return new MulticlassClassifierAdaBoostM1<LearnerInput>;
    case MulticlassClassifierTypeDecisionTree:
        return new MulticlassClassifierDecisionTree<LearnerInput>;
    case MulticlassClassifierTypeNearestNeighborBruteForce:
        return new MulticlassClassifierNearestNeighborBruteForce<LearnerInput>;
    case MulticlassClassifierTypeNearestNeighborANN:
        return new MulticlassClassifierNearestNeighborANN<LearnerInput>;
    case MulticlassClassifierTypeOneVsAll:
        return new MulticlassClassifierOneVsAll<LearnerInput>;
    case MulticlassClassifierTypePairwiseCoupling:
        return new MulticlassClassifierPairwiseCoupling<LearnerInput>;*/
    default:
        SignalError("Invalid classifier type");
        return NULL;
    }
}

template<class LearnerInput>
MulticlassClassifier<LearnerInput>* MakeMulticlassClassifierFromStream(InputDataStream &Stream)
{
    /*MulticlassClassifierType Type;
    Stream.ReadData(Type);
    MulticlassClassifier<LearnerInput> *Result = MakeMulticlassClassifier<LearnerInput>(Type);
    Result->LoadFromBinaryStream(Stream);
    return Result;*/
    return NULL;
}
