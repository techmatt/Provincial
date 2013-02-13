struct GraphEdge
{
    __forceinline UINT GetOtherVertex(UINT V0) const
    {
        if(V[0] == V0)
        {
            return V[1];
        }
        else
        {
            return V[0];
        }
    }

    UINT V[2];
    float Weight;
};

struct GraphVertex
{
    Vector<UINT> Edges;

    //
    // Internal values used by Dijkstra's algorithm
    //
    float Dist;
    int PredecessorEdgeIndex;
    bool Visited;
};

class Graph
{
public:
    Graph();
    Graph(const Vector<GraphEdge> &Edges);
    void FreeMemory();
    //void LoadFromEdgeMesh(const EdgeMesh &M);
    void LoadEdges(const Vector<GraphEdge> &Edges);

    void ShortestPath(UINT StartVertex, UINT EndVertex, Vector<UINT> &PathEdgeIndices);

    __forceinline const Vector<GraphEdge>& Edges()
    {
        return _Edges;
    }

private:
    void GenerateAdjacencyList();
    UINT HighestVertexIndex();

    Vector<GraphEdge> _Edges;
    Vector<GraphVertex> _Vertices;
};
