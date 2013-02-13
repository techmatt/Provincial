//
// This include will be ignored by double-inclusion detection in Main.h --- it is here to appease Intellisense since this is a *.cpp file and not a *.inl file.
//
#include "Main.h"

void BayesNode::SaveBinary(OutputDataStream &stream) const
{
    Vector<UINT> parentIndices(parents.Length());
    for(UINT parentIndex = 0; parentIndex < parents.Length(); parentIndex++)
    {
        parentIndices[parentIndex] = parents[parentIndex]->index;
    }

    stream << index << cardinality << parentIndices << table;
}

void BayesNode::LoadBinary(InputDataStream &stream, const Vector<BayesNode*> &allNodes)
{
    Vector<UINT> parentIndices;
    stream >> index >> cardinality >> parentIndices >> table;

    parents.Allocate(parentIndices.Length());
    for(UINT parentIndex = 0; parentIndex < parentIndices.Length(); parentIndex++)
    {
        parents[parentIndex] = allNodes[parentIndices[parentIndex]];
    }
}

void BayesNode::SetTableProbabilities(const Grid<UINT> &observations)
{
    const UINT parentCount = parents.Length();
    const UINT observationCount = observations.Rows();
    
    //
    // Resize the table in necessary
    //
    bool tableSizeCorrect = (table.Dimensions().Length() == parentCount + 1) && (table.Dimensions()[0] == cardinality);
    for(unsigned int parentIndex = 0; tableSizeCorrect && parentIndex < parents.Length(); parentIndex++)
    {
        if(table.Dimensions()[parentIndex + 1] != parents[parentIndex]->cardinality)
        {
            tableSizeCorrect = false;
        }
    }
    if(!tableSizeCorrect)
    {
        Vector<UINT> tableDimensions(parents.Length() + 1);
        tableDimensions[0] = cardinality;
        for(unsigned int parentIndex = 0; parentIndex < parents.Length(); parentIndex++)
        {
            tableDimensions[parentIndex + 1] = parents[parentIndex]->cardinality;
        }
        table.Allocate(tableDimensions);
    }

    const UINT totalEntries = table.TotalEntries();
    const UINT dimensionCount = table.Dimensions().Length();
    Vector<UINT> indices(dimensionCount, 0);
    for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        UINT positiveCount = 0;
        for(UINT observationIndex = 0; observationIndex < observationCount; observationIndex++)
        {
            bool observationIsPositive = (observations(observationIndex, index) == indices[0]);
            for(UINT parentIndex = 0; parentIndex < parentCount; parentIndex++)
            {
                if(observations(observationIndex, parents[parentIndex]->index) != indices[parentIndex + 1])
                {
                    observationIsPositive = false;
                }
            }
            if(observationIsPositive)
            {
                positiveCount++;
            }
        }
        table(indices) = positiveCount;

        table.IncrementIndices(indices);
    }
}

bool BayesNode::TryToSample(Vector<UINT> &assignments) const
{
    const UINT parentCount = parents.Length();
    for(UINT parentIndex = 0; parentIndex < parentCount; parentIndex++)
    {
        if(assignments[parents[parentIndex]->index] == 0xFFFFFFFF)
        {
            return false;
        }
    }

    for(UINT parentIndex = 0; parentIndex < parentCount; parentIndex++)
    {
        tableIndicesStorage[parentIndex + 1] = assignments[parents[parentIndex]->index];
    }

    UINT total = 0;
    for(UINT cardinalityIndex = 0; cardinalityIndex < cardinality; cardinalityIndex++)
    {
        tableIndicesStorage[0] = cardinalityIndex;
        UINT curProbability = table(tableIndicesStorage);
        total += curProbability;
        tableProbabilityStorage[cardinalityIndex] = double(curProbability);
    }

    if(total == 0)
    {
        //
        // This is expected in the case of the required edges not completely covering the CPT table
        //
        //Console::WriteLine("total==0!!!\n");
        assignments[index] = 0;
        return true;
    }

    double s = rnd() * total;
    for(UINT cardinalityIndex = 0; cardinalityIndex < cardinality; cardinalityIndex++)
    {
        double curProbability = tableProbabilityStorage[cardinalityIndex];
        if(s <= curProbability)
        {
            assignments[index] = cardinalityIndex;
            return true;
        }
        s -= curProbability;
    }
    Console::WriteLine("invalid sampling!\n");
    return false;
}

double BayesNode::ComputeAssignmentProbability(const Vector<UINT> &assignments) const
{
    const UINT parentCount = parents.Length();
    for(UINT parentIndex = 0; parentIndex < parentCount; parentIndex++)
    {
        tableIndicesStorage[parentIndex + 1] = assignments[parents[parentIndex]->index];
    }

    UINT total = 0;
    for(UINT cardinalityIndex = 0; cardinalityIndex < cardinality; cardinalityIndex++)
    {
        tableIndicesStorage[0] = cardinalityIndex;
        UINT curProbability = table(tableIndicesStorage);
        total += curProbability;
        tableProbabilityStorage[cardinalityIndex] = double(curProbability);
    }

    double result = tableProbabilityStorage[assignments[index]] / double(total);
    return result;
}

BayesNet::BayesNet(const Vector<UINT> &variableCardinalities, const Vector< pair<UINT, UINT> > &directedEdges, const Grid<UINT> &observations, const String &description)
{
    _description = description;
    InitInternal(variableCardinalities, directedEdges, observations);
}

BayesNet::BayesNet(const BayesNet &b)
{
    PersistentSignalError("Copy constructor not implemented for BayesNet");
}

BayesNet::BayesNet()
{
    _description = "NULL";
}

void BayesNet::InitInternal(const Vector<UINT> &variableCardinalities, const Vector< pair<UINT, UINT> > &directedEdges, const Grid<UINT> &observations)
{
    const UINT variableCount = variableCardinalities.Length();
    _nodes.Allocate(variableCount);

    for(UINT nodeIndex = 0; nodeIndex < variableCount; nodeIndex++)
    {
        BayesNode *newNode = new BayesNode;
        newNode->cardinality = variableCardinalities[nodeIndex];
        newNode->index = nodeIndex;
        _nodes[nodeIndex] = newNode;
    }

    for(UINT edgeIndex = 0; edgeIndex < directedEdges.Length(); edgeIndex++)
    {
        const pair<UINT, UINT> &curEdge = directedEdges[edgeIndex];
        
        BayesNode &parentNode = *_nodes[curEdge.first];
        BayesNode &childNode = *_nodes[curEdge.second];

        //parentNode.children.PushEnd(&childNode);
        childNode.parents.PushEnd(&parentNode);
    }

    for(UINT nodeIndex = 0; nodeIndex < variableCount; nodeIndex++)
    {
        BayesNode &curNode = *_nodes[nodeIndex];
        curNode.tableIndicesStorage.Allocate(curNode.parents.Length() + 1);
        curNode.tableProbabilityStorage.Allocate(curNode.cardinality);
    }
    
    SetTableProbabilities(observations);
}

BayesNet::~BayesNet()
{
    _nodes.DeleteMemory();
}

void BayesNet::SetTableProbabilities(const Grid<UINT> &observations)
{
    for(UINT nodeIndex = 0; nodeIndex < _nodes.Length(); nodeIndex++)
    {
        _nodes[nodeIndex]->SetTableProbabilities(observations);
    }
}

void BayesNet::Sample(Vector<UINT> &result) const
{
    const UINT variableCount = _nodes.Length();
    if(result.Length() != variableCount)
    {
        result.Allocate(variableCount);
    }
    result.Clear(0xFFFFFFFF);

    UINT variablesAssigned = 0;
    while(variablesAssigned < variableCount)
    {
        for(UINT variableIndex = 0; variableIndex < variableCount; variableIndex++)
        {
            UINT &curVariable = result[variableIndex];
            if(curVariable == 0xFFFFFFFF && _nodes[variableIndex]->TryToSample(result))
            {
                variablesAssigned++;
            }
        }
    }
}

double BayesNet::ComputeAssignmentProbability(const Vector<UINT> &assignment) const
{
    double result = 1.0;
    for(UINT variableIndex = 0; variableIndex < _nodes.Length(); variableIndex++)
    {
        const BayesNode &curNode = *_nodes[variableIndex];
        result *= curNode.ComputeAssignmentProbability(assignment);
    }
    return result;
}

double BayesNet::MDLScore(const Grid<UINT> &observations) const
{
    //
    // http://jmlr.csail.mit.edu/papers/volume7/decampos06a/decampos06a.pdf
    // The "complexity term" is kind of bad because it punishes cardinalities that are never used.
    //
    const UINT observationCount = observations.Rows();

    double logLikelihood = 0.0;
    double networkComplexity = 0.0;

    for(UINT variableIndex = 0; variableIndex < _nodes.Length(); variableIndex++)
    {
        const BayesNode &curNode = *_nodes[variableIndex];

        const UINT totalEntries = curNode.table.TotalEntries();
        const UINT parentCount = curNode.parents.Length();
        
        networkComplexity += (curNode.cardinality - 1) * totalEntries;

        Vector<UINT> indices(curNode.table.Dimensions().Length(), 0);
        for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
        {
            UINT N_ijk = 0;
            UINT N_ij = 0;

            for(UINT observationIndex = 0; observationIndex < observationCount; observationIndex++)
            {
                bool observationMatches = true;
                for(UINT parentIndex = 0; parentIndex < parentCount; parentIndex++)
                {
                    if(observations(observationIndex, curNode.parents[parentIndex]->index) != indices[parentIndex + 1])
                    {
                        observationMatches = false;
                    }
                }
                if(observationMatches)
                {
                    N_ij++;
                    if(observations(observationIndex, curNode.index) == indices[0])
                    {
                        N_ijk++;
                    }
                }
            }

            if(N_ijk != 0)
            {
                logLikelihood += double(N_ijk) * log( double(N_ijk) / double(N_ij) );
            }

            curNode.table.IncrementIndices(indices);
        }
    }

    return (logLikelihood - networkComplexity * 0.5 * log(double(observationCount)));
    //return logLikelihood;
}

void BayesNet::GenerateChildren(Vector<BayesNet*> &children, const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges) const
{
    const UINT nodeCount = _nodes.Length();
    Vector<UINT> variableCardinalities(nodeCount);
    Vector< pair<UINT, UINT> > myEdges;
    
    for(UINT nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        const BayesNode &curNode = *_nodes[nodeIndex];
        variableCardinalities[nodeIndex] = curNode.cardinality;
        for(UINT parentIndex = 0; parentIndex < curNode.parents.Length(); parentIndex++)
        {
            myEdges.PushEnd( make_pair( curNode.parents[parentIndex]->index, nodeIndex ) );
        }
    }

    //
    // Try all possible edge flips
    //
    for(UINT edgeIndex = 0; edgeIndex < myEdges.Length(); edgeIndex++)
    {
        if(!requiredEdges.Contains(myEdges[edgeIndex]))
        {
            Vector< pair<UINT, UINT> > childEdges = myEdges;
            Utility::Swap(childEdges[edgeIndex].first, childEdges[edgeIndex].second);
            if(EdgeSetIsValid(childEdges, nodeCount))
            {
                children.PushEnd(new BayesNet(variableCardinalities, childEdges, observations, "flip" + String(edgeIndex)));
            }
        }
    }

    //
    // Try adding all possible edges
    //
    for(UINT parentIndex = 0; parentIndex < nodeCount; parentIndex++)
    {
        for(UINT childIndex = 0; childIndex < nodeCount; childIndex++)
        {
            pair<UINT, UINT> curEdge(parentIndex, childIndex);
            if(parentIndex != childIndex && !myEdges.Contains(curEdge))
            {
                Vector< pair<UINT, UINT> > childEdges = myEdges;
                childEdges.PushEnd(curEdge);
                if(EdgeSetIsValid(childEdges, nodeCount))
                {
                    children.PushEnd(new BayesNet(variableCardinalities, childEdges, observations, "add" + String(curEdge.first) + "-" + String(curEdge.second) ));
                }
            }
        }
    }

    //
    // Try deleting all possible edges
    //
    for(UINT edgeIndex = 0; edgeIndex < myEdges.Length(); edgeIndex++)
    {
        if(!requiredEdges.Contains(myEdges[edgeIndex]))
        {
            Vector< pair<UINT, UINT> > childEdges = myEdges;
            childEdges.RemoveSwap(edgeIndex);
            if(EdgeSetIsValid(childEdges, nodeCount))
            {
                children.PushEnd(new BayesNet(variableCardinalities, childEdges, observations, "delete" + String(edgeIndex)));
            }
        }
    }

    Console::WriteLine("Children generated: " + String(children.Length()));
}

BayesNet* BayesNet::LearnBayesNet(const Vector<UINT> &variableCardinalities, const Grid<UINT> &observations, const Vector< pair<UINT, UINT> > &requiredEdges, int maxIterations)
{
    BayesNet *activeNetwork = new BayesNet(variableCardinalities, requiredEdges, observations, "base");
    double activeNetworkScore = activeNetwork->MDLScore(observations);

    Console::WriteLine("Base score: " + String(activeNetworkScore));

    UINT iterationsLeft = 0xFFFFFFFF;
    if(maxIterations != -1) iterationsLeft = maxIterations;

    bool activeNetworkChanged = true;
    while(activeNetworkChanged && iterationsLeft > 0)
    {
        iterationsLeft--;
        activeNetworkChanged = false;
        Vector<BayesNet*> children;
        activeNetwork->GenerateChildren(children, observations, requiredEdges);
        for(UINT childIndex = 0; childIndex < children.Length(); childIndex++)
        {
            BayesNet *curNetwork = children[childIndex];
            double curNetworkScore = curNetwork->MDLScore(observations);
            if(curNetworkScore > activeNetworkScore)
            {
                activeNetwork = curNetwork;
                activeNetworkScore = curNetworkScore;
                Console::WriteLine("New score: " + String(curNetworkScore) + "(" + curNetwork->_description + ")");
                activeNetworkChanged = true;
            }
        }
        for(UINT childIndex = 0; childIndex < children.Length(); childIndex++)
        {
            if(children[childIndex] != activeNetwork)
            {
                delete children[childIndex];
            }
        }
    }

    Console::WriteLine("Best score: " + String(activeNetworkScore));
    return activeNetwork;
}

bool BayesNet::EdgeSetIsValid(const Vector< pair<UINT, UINT> > &directedEdges, UINT variableCount)
{
    Vector<UINT> nodeState(variableCount, 0);
    Vector< Vector<UINT> > nodeParents(variableCount);
    for(UINT edgeIndex = 0; edgeIndex < directedEdges.Length(); edgeIndex++)
    {
        nodeParents[directedEdges[edgeIndex].second].PushEnd(directedEdges[edgeIndex].first);
    }

    bool variablesChanged = true;
    UINT nodesReached = 0;
    while(variablesChanged)
    {
        variablesChanged = false;
        for(UINT nodeIndex = 0; nodeIndex < variableCount; nodeIndex++)
        {
            if(nodeState[nodeIndex] == 0)
            {
                bool nodeReady = true;
                const Vector<UINT> &parents = nodeParents[nodeIndex];
                for(UINT parentIndex = 0; parentIndex < parents.Length(); parentIndex++)
                {
                    if(nodeState[parents[parentIndex]] == 0)
                    {
                        nodeReady = false;
                    }
                }
                if(nodeReady)
                {
                    nodeState[nodeIndex] = 1;
                    nodesReached++;
                    variablesChanged = true;
                }
            }
        }
    }
    return (nodesReached == variableCount);
}

void BayesNet::SaveASCII(const String &filename) const
{
    ofstream file(filename.CString());
    file << "nodeCount " << _nodes.Length() << endl;
    for(UINT nodeIndex = 0; nodeIndex < _nodes.Length(); nodeIndex++)
    {
        const BayesNode &curNode = *_nodes[nodeIndex];
        file << "node " << nodeIndex << endl;
        file << "parents";
        for(UINT parentIndex = 0; parentIndex < curNode.parents.Length(); parentIndex++)
        {
            file << ' ' << curNode.parents[parentIndex]->index;
        }
        file << endl;
    }
}

void BayesNet::SaveGraphViz(const String &filename, const Vector<String> &labels) const
{
    ofstream file(filename.CString());
    file << "digraph BayesNet {" << endl;
    file << "  size = \"30, 30\";" << endl;
    file << "  layout = \"fdp\";" << endl;
    file << "  node [shape = box];" << endl << endl;

    for(UINT nodeIndex = 0; nodeIndex < _nodes.Length(); nodeIndex++)
    {
        const BayesNode &curNode = *_nodes[nodeIndex];
        file << "  n" << nodeIndex << " [label=\"" << labels[nodeIndex] << "\"];" << endl;
    }
    for(UINT nodeIndex = 0; nodeIndex < _nodes.Length(); nodeIndex++)
    {
        const BayesNode &curNode = *_nodes[nodeIndex];
        file << "  n" << nodeIndex << " -> { ";
        for(UINT parentIndex = 0; parentIndex < curNode.parents.Length(); parentIndex++)
        {
            file << "n" << curNode.parents[parentIndex]->index << " ";
        }
        file << "};" << endl;
    }
    file << "}" << endl;
}

double BayesNet::KLDivergence(const Grid<UINT> &observations) const
{
    double result = 0.0;
    const UINT observationCount = observations.Rows();
    const double basePi = 1.0 / double(observationCount);

    set<UINT> usedObservations;

    for(UINT observationIndexOuter = 0; observationIndexOuter < observationCount; observationIndexOuter++)
    {
        if(usedObservations.count(observationIndexOuter) == 0)
        {
            UINT duplicity = 1;
            Vector<UINT> curObservation = observations.ExtractRow(observationIndexOuter);
            for(UINT observationIndexInner = observationIndexOuter + 1; observationIndexInner < observationCount; observationIndexInner++)
            {
                Vector<UINT> innerObservation = observations.ExtractRow(observationIndexInner);
                bool identical = true;
                for(UINT index = 0; index < curObservation.Length(); index++)
                {
                    if(curObservation[index] != innerObservation[index])
                    {
                        identical = false;
                    }
                }
                if(identical)
                {
                    duplicity++;
                    usedObservations.insert(observationIndexInner);
                }
            }

            const double Pi = duplicity * basePi;
            const double Qi = ComputeAssignmentProbability(curObservation);
            result += Pi * log ( Pi / Qi );
        }
    }
    return result;
}

void BayesNet::SaveBinary(OutputDataStream &stream) const
{
    stream << _description;

    const UINT nodeCount = _nodes.Length();
    stream << nodeCount;
    for(UINT nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        _nodes[nodeIndex]->SaveBinary(stream);
    }
}

void BayesNet::LoadBinary(InputDataStream &stream)
{
    stream >> _description;

    UINT nodeCount;
    stream >> nodeCount;

    _nodes.Allocate(nodeCount);

    for(UINT nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        _nodes[nodeIndex] = new BayesNode;
    }

    for(UINT nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        _nodes[nodeIndex]->LoadBinary(stream, _nodes);
    }

    for(UINT nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        BayesNode &curNode = *_nodes[nodeIndex];
        curNode.tableIndicesStorage.Allocate(curNode.parents.Length() + 1);
        curNode.tableProbabilityStorage.Allocate(curNode.cardinality);
    }
}
