struct XMLSchemaChildEntry;

struct XMLAttribute
{
    String Name;
    String Value;
};

struct XMLNode
{
    XMLNode(const String &S);
    ~XMLNode();
    void FreeMemory();

    void Init(const String &S);
    void Save(ofstream &File);

    __forceinline void AddChild(XMLNode *Child)
    {
        Children.PushEnd(Child);
    }

    void MakeChildEntryList(Vector<XMLSchemaChildEntry> &List) const;
    void GetChildrenByName(const String &Name, Vector<XMLNode*> &Result) const;
    String GetAttributeByName(const String &Name) const;

    String Name;
    String Value;
    Vector<XMLAttribute> Attributes;
    Vector<XMLNode*> Children;
};
