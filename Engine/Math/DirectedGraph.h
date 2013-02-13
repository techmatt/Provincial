struct NoEdgeData
{
    UINT unused;
};

template<class NodeData, class EdgeData>
class DirectedGraph
{
public:
    DirectedGraph();
    ~DirectedGraph();
    void FreeMemory();

    struct Node;
    struct Edge
    {
        Edge(UINT _index)
        {
            index = _index;
        }
        Edge(UINT _index, const EdgeData &_data)
        {
            index = _index;
            data = _data;
        }
        Node *fromNode;
        Node *toNode;
        UINT index;
        EdgeData data;
    };

    struct Node
    {
    public:
        Node(UINT _index, const NodeData &_data)
        {
            index = _index;
            data = _data;
        }

        //
        // Query
        //
        bool IsTreeCheck(Vector<Node*> &observedNodes);
        Vector<Node*> FindAnyPath(Node *target)
        {
            Vector<Node*> path;
            FindAnyPath(target, path);
            return path;
        }
        Vector<Node*> FindShortestPath(Node *target);

        Vector<Edge*> outgoingEdges;
        Vector<Edge*> incomingEdges;
        UINT index;
        NodeData data;

    private:
        bool FindAnyPath(Node *target, Vector<Node*> &path);
    };

    //
    // Modifiers
    //
    void AddNode(const NodeData &data);
    void AddEdge(const EdgeData &data, Node *fromNode, Node *toNode);
    void AddEdge(Node *fromNode, Node *toNode);

    bool IsTree(Node &root);

    //
    // Accessors
    //
    __forceinline const Vector<Edge*>& edges() const
    {
        return _edges;
    }
    __forceinline const Vector<Node*>& nodes() const
    {
        return _nodes;
    }

private:
    Vector<Edge*> _edges;
    Vector<Node*> _nodes;
};

template<class NodeData, class EdgeData>
DirectedGraph<NodeData, EdgeData>::DirectedGraph()
{

}

template<class NodeData, class EdgeData>
DirectedGraph<NodeData, EdgeData>::~DirectedGraph()
{
    FreeMemory();
}

template<class NodeData, class EdgeData>
void DirectedGraph<NodeData, EdgeData>::FreeMemory()
{
    _edges.DeleteMemory();
    _nodes.DeleteMemory();
}

template<class NodeData, class EdgeData>
void DirectedGraph<NodeData, EdgeData>::AddNode(const NodeData &data)
{
    _nodes.PushEnd(new Node(_nodes.Length(), data));
}

template<class NodeData, class EdgeData>
void DirectedGraph<NodeData, EdgeData>::AddEdge(const EdgeData &data, Node *fromNode, Node *toNode)
{
    Edge *newEdge = new Edge(_edges.Length(), data);
    newEdge->fromNode = fromNode;
    newEdge->toNode = toNode;
    fromNode->outgoingEdges.PushEnd(newEdge);
    toNode->incomingEdges.PushEnd(newEdge);
    _edges.PushEnd(newEdge);
}

template<class NodeData, class EdgeData>
void DirectedGraph<NodeData, EdgeData>::AddEdge(Node *fromNode, Node *toNode)
{
    Edge *newEdge = new Edge(_edges.Length());
    newEdge->fromNode = fromNode;
    newEdge->toNode = toNode;
    fromNode->outgoingEdges.PushEnd(newEdge);
    toNode->incomingEdges.PushEnd(newEdge);
    _edges.PushEnd(newEdge);
}

template<class NodeData, class EdgeData>
bool DirectedGraph<NodeData, EdgeData>::IsTree(Node &root)
{
    if(root.incomingEdges.Length() > 0) return false;
    Vector<Node*> observedNodes;
    root.IsTreeCheck(observedNodes);
    return (observedNodes.Length() == _nodes.Length());
}

template<class NodeData, class EdgeData>
bool DirectedGraph<NodeData, EdgeData>::Node::IsTreeCheck(Vector<Node*> &observedNodes)
{
    if(observedNodes.Contains(this)) return false;
    observedNodes.PushEnd(this);
    for(UINT edgeIndex = 0; edgeIndex < outgoingEdges.Length(); edgeIndex++)
    {
        Edge *curEdge = outgoingEdges[edgeIndex];
        if(!curEdge->toNode->IsTreeCheck(observedNodes)) return false;
    }
    return true;
}

template<class NodeData, class EdgeData>
bool DirectedGraph<NodeData, EdgeData>::Node::FindAnyPath(Node *target, Vector<Node*> &path)
{
    if(this == target)
    {
        path.PushEnd(this);
        return true;
    }
    path.PushEnd(this);
    for(UINT edgeIndex = 0; edgeIndex < outgoingEdges.Length(); edgeIndex++)
    {
        Edge *curEdge = outgoingEdges[edgeIndex];
        if(curEdge->toNode->FindAnyPath(target, path)) return true;
    }
    path.PopEnd();
    return false;
}
