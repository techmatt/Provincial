Graph::Graph()
{

}

Graph::Graph(const Vector<GraphEdge> &Edges)
{
    LoadEdges(Edges);
}

void Graph::LoadEdges(const Vector<GraphEdge> &Edges)
{
    _Edges = Edges;
    GenerateAdjacencyList();
}

UINT Graph::HighestVertexIndex()
{
    UINT Result = 0;
    for(UINT EdgeIndex = 0; EdgeIndex < _Edges.Length(); EdgeIndex++)
    {
        const GraphEdge &curEdge = _Edges[EdgeIndex];
        Result = Math::Max(Result, curEdge.V[0]);
        Result = Math::Max(Result, curEdge.V[1]);
    }
    return Result;
}

void Graph::GenerateAdjacencyList()
{
    UINT VertexCount = HighestVertexIndex() + 1;
    _Vertices.Allocate(VertexCount);
    for(UINT EdgeIndex = 0; EdgeIndex < _Edges.Length(); EdgeIndex++)
    {
        const GraphEdge &curEdge = _Edges[EdgeIndex];
        _Vertices[curEdge.V[0]].Edges.PushEnd(EdgeIndex);
        _Vertices[curEdge.V[1]].Edges.PushEnd(EdgeIndex);
    }
}

struct DijkstraQueueElement
{
    DijkstraQueueElement() {}
    DijkstraQueueElement(UINT _VertexIndex, float _Value)
    {
        VertexIndex = _VertexIndex;
        Value = _Value;
    }
    UINT VertexIndex;
    float Value;
};

bool operator < (const DijkstraQueueElement &A, const DijkstraQueueElement &B)
{
    return A.Value > B.Value;
}

void Graph::ShortestPath(UINT EndVertex, UINT StartVertex, Vector<UINT> &PathEdgeIndices)
{
    //
    // StartVertex & EndVertex are flipped intentionally
    //
    PathEdgeIndices.FreeMemory();
    if(StartVertex == EndVertex)
    {
        return;
    }

    for(UINT VertexIndex = 0; VertexIndex < _Vertices.Length(); VertexIndex++)
    {
        GraphVertex &curVertex = _Vertices[VertexIndex];
        curVertex.Dist = 1e10f;
        curVertex.PredecessorEdgeIndex = -1;
        curVertex.Visited = false;
    }

    GraphVertex &sourceVertex = _Vertices[StartVertex];

    sourceVertex.Dist = 0.0f;

    priority_queue<DijkstraQueueElement> Queue;
    Queue.push(DijkstraQueueElement(StartVertex, 0.0f));
    
    while(!Queue.empty())
    {
        DijkstraQueueElement QueueTop = Queue.top();
        Queue.pop();

        GraphVertex &curVertex = _Vertices[QueueTop.VertexIndex];
        if(!curVertex.Visited)
        {
            curVertex.Visited = true;
            for(UINT AdjacentEdgeIndex = 0; AdjacentEdgeIndex < curVertex.Edges.Length(); AdjacentEdgeIndex++)
            {
                GraphEdge &curEdge = _Edges[curVertex.Edges[AdjacentEdgeIndex]];
                UINT otherVertexIndex = curEdge.GetOtherVertex(QueueTop.VertexIndex);
                GraphVertex &otherVertex = _Vertices[otherVertexIndex];
                float NewDist = curVertex.Dist + curEdge.Weight;
                if(NewDist < otherVertex.Dist)
                {
                    otherVertex.Dist = NewDist;
                    otherVertex.PredecessorEdgeIndex = curVertex.Edges[AdjacentEdgeIndex];
                    Queue.push(DijkstraQueueElement(otherVertexIndex, otherVertex.Dist));
                }
            }
        }
    }

    if(_Vertices[EndVertex].PredecessorEdgeIndex == -1)
    {
        Console::WriteLine("No path found");
    }
    else
    {
        bool Done = false;
        UINT CurVertex = EndVertex;
        while(!Done)
        {
            UINT PredecessorEdgeIndex = _Vertices[CurVertex].PredecessorEdgeIndex;
            if(PredecessorEdgeIndex == -1)
            {
                Done = true;
            }
            else
            {
                PathEdgeIndices.PushEnd(PredecessorEdgeIndex);
                CurVertex = _Edges[PredecessorEdgeIndex].GetOtherVertex(CurVertex);
            }
        }
    }
}

/*void Graph::LoadFromEdgeMesh(const EdgeMesh &M)
{
    Vector<GraphEdge> Edges(M.FullEdges().Length());
    for(UINT EdgeIndex = 0; EdgeIndex < Edges.Length(); EdgeIndex++)
    {
        GraphEdge &curEdge = Edges[EdgeIndex];
        FullEdge &curMeshEdge = *(M.FullEdges()[EdgeIndex]);
        curEdge.V[0] = curMeshEdge.GetVertex(0).Index();
        curEdge.V[1] = curMeshEdge.GetVertex(1).Index();
        curEdge.Weight = Vec3::Dist(curMeshEdge.GetVertex(0).Pos(), curMeshEdge.GetVertex(1).Pos());
    }
    LoadEdges(Edges);
}*/
