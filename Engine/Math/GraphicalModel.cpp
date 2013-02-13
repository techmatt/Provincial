//
// This include will be ignored by double-inclusion detection in Main.h --- it is here to appease Intellisense since this is a *.cpp file and not a *.inl file.
//
#include "Main.h"

void GraphicalModelBayesNetBooleanizer::Sample(Vector<UINT> &result) const
{
    Vector<UINT> assignment;
    _bayes->Sample(assignment);
    result = BooleansToObservation(assignment);
}

void GraphicalModelBayesNetBooleanizer::Learn(const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges, int maxIterations)
{
    const UINT observationCount = observations.Rows();
    const UINT variableCount = observations.Cols();
    _cardinalities.Allocate(variableCount, 2);

    for(UINT observationIndex = 0; observationIndex < observationCount; observationIndex++)
    {
        for(UINT variableIndex = 0; variableIndex < variableCount; variableIndex++)
        {
            _cardinalities[variableIndex] = Math::Max(_cardinalities[variableIndex], observations(observationIndex, variableIndex) + 1);
        }
    }

    Vector<UINT> variableIndexToBooleanIndex(variableCount);
    for(UINT variableIndex = 0; variableIndex < variableCount; variableIndex++)
    {
        const UINT curCardinality = _cardinalities[variableIndex];
        variableIndexToBooleanIndex[variableIndex] = _booleanVariables.Length();
        for(UINT booleanIndex = 0; booleanIndex < curCardinality - 1; booleanIndex++)
        {
            BooleanVariable curBoolean;
            curBoolean.variableIndex = variableIndex;
            curBoolean.value = booleanIndex;
            _booleanVariables.PushEnd(curBoolean);
        }
    }

    const UINT booleanCount = _booleanVariables.Length();

    Grid<UINT> booleanObservations(observationCount, booleanCount);
    for(UINT observationIndex = 0; observationIndex < observationCount; observationIndex++)
    {
        booleanObservations.SetRow(observationIndex, ObservationToBooleans(observations.ExtractRow(observationIndex)));
    }

    Vector<UINT> booleanVariableCadinalities(booleanCount, 2);

    //
    // Remap variable-based requiredEdges to the boolean equivalents
    //
    Vector< pair<UINT, UINT> > booleanRequiredEdges(requiredEdges.Length());
    for(UINT requiredEdgeIndex = 0; requiredEdgeIndex < requiredEdges.Length(); requiredEdgeIndex++)
    {
        const pair<UINT, UINT> &edgeA = requiredEdges[requiredEdgeIndex];
        pair<UINT, UINT> &edgeB = booleanRequiredEdges[requiredEdgeIndex];
        edgeB.first = variableIndexToBooleanIndex[edgeA.first];
        edgeB.second = variableIndexToBooleanIndex[edgeA.second];
    }

    //
    // Boolean >N depends on >(N-1).
    //
    for(UINT booleanIndex = 0; booleanIndex < booleanCount; booleanIndex++)
    {
        const BooleanVariable &curVariable = _booleanVariables[booleanIndex];
        if(curVariable.value > 0)
        {
            booleanRequiredEdges.PushEnd(make_pair(booleanIndex - 1, booleanIndex));
        }
    }

    _bayes = BayesNet::LearnBayesNet(booleanVariableCadinalities, booleanObservations, booleanRequiredEdges, maxIterations);
}

Vector<UINT> GraphicalModelBayesNetBooleanizer::ObservationToBooleans(const Vector<UINT> &observation) const
{
    const UINT booleanCount = _booleanVariables.Length();
    Vector<UINT> result(booleanCount);
    for(UINT booleanIndex = 0; booleanIndex < booleanCount; booleanIndex++)
    {
        const BooleanVariable &curBoolean = _booleanVariables[booleanIndex];
        result[booleanIndex] = (observation[curBoolean.variableIndex] > curBoolean.value);
    }
    return result;
}

Vector<UINT> GraphicalModelBayesNetBooleanizer::BooleansToObservation(const Vector<UINT> &booleans) const
{
    const UINT variableCount = _cardinalities.Length();
    Vector<UINT> result(variableCount, 0);
    for(UINT booleanIndex = 0; booleanIndex < booleans.Length(); booleanIndex++)
    {
        if(booleans[booleanIndex])
        {
            const BooleanVariable &curBoolean = _booleanVariables[booleanIndex];
            result[curBoolean.variableIndex] = Math::Max(result[curBoolean.variableIndex], curBoolean.value + 1);
        }
    }
    return result;
}

void GraphicalModelBayesNetBooleanizer::SaveBinary(const String &filename) const
{
    OutputDataStream stream;
    stream << _cardinalities;
    stream.WriteSimpleVector(_booleanVariables);
    _bayes->SaveBinary(stream);
    stream.SaveToFile(filename);
}

void GraphicalModelBayesNetBooleanizer::LoadBinary(const String &filename)
{
    InputDataStream stream;
    stream.LoadFromFile(filename);
    stream >> _cardinalities;
    stream.ReadSimpleVector(_booleanVariables);
    _bayes = new BayesNet;
    _bayes->LoadBinary(stream);
}
