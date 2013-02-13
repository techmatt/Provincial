enum XMLDataType
{
    XMLDataNone,
    XMLDataString,
    XMLDataCount
};

struct XMLSchemaChildEntry
{
    XMLSchemaChildEntry() {}
    XMLSchemaChildEntry(const String &_Name)
    {
        Name = _Name;
        Count = 1;
    }
    String Name;
    UINT Count;
};

struct XMLSchemaAttributeEntry
{
    XMLSchemaAttributeEntry() {}
    XMLSchemaAttributeEntry(const String &_Name)
    {
        Name = _Name;
        Type = XMLDataString;
    }
    String Name;
    XMLDataType Type;
};

struct XMLSchemaEntry
{
    XMLSchemaEntry(const String &_Name)
    {
        Name = _Name;
        Type = XMLDataCount;
    }

    void ObserveNode(const XMLNode &Node);
    
    String Name;
    XMLDataType Type;
    Vector<XMLSchemaChildEntry> Children;
    Vector<XMLSchemaAttributeEntry> Attributes;

private:
    void ObserveAttribute(const XMLAttribute &Attribute);
    void ObserveChild(const XMLSchemaChildEntry &Child);
};

class XMLSchema
{
public:
    void ObserveNode(const XMLNode &Node);
    void SaveSourceFile(const String &SourceFilename, const String &Namespace);
    void SaveHeaderFile(const String &HeaderFilename, const String &Namespace);

private:
    map<String, XMLSchemaEntry*, String::LexicographicComparison> _StringToSchemaMap;
};

class XMLFile
{
public:
    XMLFile();
    ~XMLFile();
    void FreeMemory();

    __forceinline XMLNode* Root()
    {
        return _Root;
    }

    void Load(const String &Filename);
    void Save(const String &Filename);
    void SaveSchema(const String &SourceFilename, const String &HeaderFilename, const String &Namespace);

private:
    XMLNode *_Root;
};

String XMLNameToCStyleName(const String &S);