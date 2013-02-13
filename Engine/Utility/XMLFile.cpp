String XMLNameToCStyleName(const String &S)
{
    String Result = S;
    for(UINT Index = 0; Index < Result.Length(); Index++)
    {
        char &C = Result[Index];
        if(C == ':' || C == '-')
        {
            C = '_';
        }
    }
    return Result;
}

XMLDataType XMLDataTypeFromString(const String &S)
{
    if(S.Length() == 0)
    {
        return XMLDataNone;
    }
    else
    {
        return XMLDataString;
    }
}

String StructNameFromXMLDataType(XMLDataType T)
{
    switch(T)
    {
    case XMLDataNone:
        return "void";
    case XMLDataString:
        return "String";
    default:
        SignalError("Invalid data type");
        return "Invalid";
    }
}

void XMLSchemaEntry::ObserveNode(const XMLNode &Node)
{
    Assert(Name == Node.Name, "Node name incosistent");

    XMLDataType NodeType = XMLDataTypeFromString(Node.Value);
    if(Type == XMLDataCount)
    {
        Type = NodeType;
    }
    else
    {
        if(Type == XMLDataNone)
        {
            Type = NodeType;
        }
        else
        {
            Assert(NodeType == XMLDataNone || Type == NodeType, "Node type inconsistent");
        }
    }

    for(UINT AttributeIndex = 0; AttributeIndex < Node.Attributes.Length(); AttributeIndex++)
    {
        ObserveAttribute(Node.Attributes[AttributeIndex]);
    }

    Vector<XMLSchemaChildEntry> List;
    Node.MakeChildEntryList(List);
    for(UINT ListIndex = 0; ListIndex < List.Length(); ListIndex++)
    {
        ObserveChild(List[ListIndex]);
    }
}

void XMLSchemaEntry::ObserveAttribute(const XMLAttribute &Attribute)
{
    String CStyleAttributeName = XMLNameToCStyleName(Attribute.Name);
    for(UINT AttributeIndex = 0; AttributeIndex < Attributes.Length(); AttributeIndex++)
    {
        XMLSchemaAttributeEntry &CurAttribute = Attributes[AttributeIndex];
        if(CurAttribute.Name == CStyleAttributeName)
        {
            return;
        }
    }
    Attributes.PushEnd(XMLSchemaAttributeEntry(CStyleAttributeName));
}

void XMLSchemaEntry::ObserveChild(const XMLSchemaChildEntry &Child)
{
    String CStyleChildName = XMLNameToCStyleName(Child.Name);
    for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
    {
        XMLSchemaChildEntry &CurChild = Children[ChildIndex];
        if(CurChild.Name == CStyleChildName)
        {
            CurChild.Count = Math::Max(CurChild.Count, Child.Count);
            return;
        }
    }
    Children.PushEnd(Child);
}

void XMLSchema::ObserveNode(const XMLNode &Node)
{
    map<String, XMLSchemaEntry*, String::LexicographicComparison>::iterator Iterator = _StringToSchemaMap.find(Node.Name);
    XMLSchemaEntry *CurSchemaEntry;
    if(Iterator == _StringToSchemaMap.end())
    {
        CurSchemaEntry = new XMLSchemaEntry(Node.Name);
        _StringToSchemaMap[Node.Name] = CurSchemaEntry;
    }
    else
    {
        CurSchemaEntry = Iterator->second;
    }
    CurSchemaEntry->ObserveNode(Node);

    for(UINT ChildIndex = 0; ChildIndex < Node.Children.Length(); ChildIndex++)
    {
        ObserveNode(*(Node.Children[ChildIndex]));
    }
}

void XMLSchema::SaveHeaderFile(const String &HeaderFilename, const String &Namespace)
{
    ofstream File(HeaderFilename.CString());
    File << "namespace " << Namespace << endl;
    File << "{" << endl;
    
    for(map<String, XMLSchemaEntry*, String::LexicographicComparison>::iterator Iterator = _StringToSchemaMap.begin();
        Iterator != _StringToSchemaMap.end(); Iterator++)
    {
        const XMLSchemaEntry &CurEntry = *(Iterator->second);
        File << "\tstruct S_" << CurEntry.Name << ";" << endl;
    }
    File << endl;

    for(map<String, XMLSchemaEntry*, String::LexicographicComparison>::iterator Iterator = _StringToSchemaMap.begin();
        Iterator != _StringToSchemaMap.end(); Iterator++)
    {
        const XMLSchemaEntry &CurEntry = *(Iterator->second);
        File << "\tstruct S_" << CurEntry.Name << endl;
        File << "\t{" << endl;

        File << "\t\tS_" << CurEntry.Name << "(XMLNode *Node);" << endl;
        File << "\t\t~S_" << CurEntry.Name << "();" << endl;

        if(CurEntry.Type != XMLDataNone)
        {
            File << "\t\t" << StructNameFromXMLDataType(CurEntry.Type) << " Data;" << endl;
        }
        for(UINT AttributeIndex = 0; AttributeIndex < CurEntry.Attributes.Length(); AttributeIndex++)
        {
            const XMLSchemaAttributeEntry &CurAttribute = CurEntry.Attributes[AttributeIndex];
            File << "\t\t" << StructNameFromXMLDataType(CurAttribute.Type) << " A_" << CurAttribute.Name << ";" << endl;
        }
        for(UINT ChildIndex = 0; ChildIndex < CurEntry.Children.Length(); ChildIndex++)
        {
            const XMLSchemaChildEntry &CurChild = CurEntry.Children[ChildIndex];
            if(CurChild.Count == 1)
            {
                File << "\t\tS_" << CurChild.Name << " *I_" << CurChild.Name << ";" << endl;
            }
            else
            {
                File << "\t\tVector<S_" << CurChild.Name << "*> L_" << CurChild.Name << ";" << endl;
            }
        }
        File << "\t};" << endl;
        File << endl;
    }

    File << "}" << endl;
}

void XMLSchema::SaveSourceFile(const String &HeaderFilename, const String &Namespace)
{
    ofstream File(HeaderFilename.CString());

    File << "#include \"Main.h\"" << endl << endl;
    File << "#ifdef USE_" << Namespace << endl << endl;
    File << "namespace " << Namespace << endl;
    File << "{" << endl;
    File << "\tstatic const bool ReportXMLMismatches = true;" << endl << endl;
    
    for(map<String, XMLSchemaEntry*, String::LexicographicComparison>::iterator Iterator = _StringToSchemaMap.begin();
        Iterator != _StringToSchemaMap.end(); Iterator++)
    {
        const XMLSchemaEntry &CurEntry = *(Iterator->second);
        
        /*S_COLLADA::S_COLLADA(XMLNode *Node)
        {
            Vector<XMLNode*> Result;
            
            Node->GetChildrenByName("library_images", Result);
            if(Result.Length() == 0)
            {
                I_library_images = NULL;
            }
            else if(Result.Length() == 1)
            {
                I_library_images = new S_library_images(Result[0]);
            }
            else if(ReportXMLMismatches)
            {
                Console::File() << "Vector Mismatch: S_COLLADA.I_library_images\n";
            }

            for(UINT ResultIndex = 0; ResultIndex < Result.Length(); ResultIndex++)
            {
                L_library_images.PushEnd(new S_library_images(Result[ResultIndex]));
            }
        }*/

        File << "\tS_" << CurEntry.Name << "::S_" << CurEntry.Name << "(XMLNode *Node)" << endl;
        File << "\t{" << endl;
        
        if(CurEntry.Type == XMLDataString)
        {
            File << "\t\tData = Node->Value;" << endl << endl;
        }

        if(CurEntry.Attributes.Length() > 0)
        {
            for(UINT AttributeIndex = 0; AttributeIndex < CurEntry.Attributes.Length(); AttributeIndex++)
            {
                const XMLSchemaAttributeEntry &CurAttribute = CurEntry.Attributes[AttributeIndex];
                File << "\t\tA_" << CurAttribute.Name << " = Node->GetAttributeByName(\"" << CurAttribute.Name << "\");" << endl;
            }
            File << endl;
        }

        if(CurEntry.Children.Length() > 0)
        {
            File << "\t\tVector<XMLNode*> Result;" << endl << endl;
            for(UINT ChildIndex = 0; ChildIndex < CurEntry.Children.Length(); ChildIndex++)
            {
                const XMLSchemaChildEntry &CurChild = CurEntry.Children[ChildIndex];
                File << "\t\tNode->GetChildrenByName(\"" << CurChild.Name << "\", Result);" << endl;
                if(CurChild.Count == 1)
                {
                    File << "\t\tif(Result.Length() == 0)" << endl;
                    File << "\t\t{" << endl;
                    File << "\t\t\tI_" << CurChild.Name << " = NULL;" << endl;
                    File << "\t\t}" << endl;
                    File << "\t\telse if(Result.Length() == 1)" << endl;
                    File << "\t\t{" << endl;
                    File << "\t\t\tI_" << CurChild.Name << " = new S_" << CurChild.Name << "(Result[0]);" << endl;
                    File << "\t\t}" << endl;
                    File << "\t\telse if(ReportXMLMismatches)" << endl;
                    File << "\t\t{" << endl;
                    File << "\t\t\tSignalError(\"Vector mismatch: S_" << CurEntry.Name << ".I_" << CurChild.Name << "\\n\");" << endl;
                    File << "\t\t}" << endl;
                }
                else
                {
                    File << "\t\tfor(UINT ResultIndex = 0; ResultIndex < Result.Length(); ResultIndex++)" << endl;
                    File << "\t\t{" << endl;
                    File << "\t\t\tL_" << CurChild.Name << ".PushEnd(new S_" << CurChild.Name << "(Result[ResultIndex]));" << endl;
                    File << "\t\t}" << endl;
                }
                File << endl;
            }
        }

        File << "\t}" << endl;
        File << endl;

        /*S_COLLADA::~S_COLLADA()
        {   
            Node->GetChildrenByName("library_images", Result);
            if(Result.Length() == 0)
            {
                I_library_images = NULL;
            }
            else if(Result.Length() == 1)
            {
                I_library_images = new S_library_images(Result[0]);
            }
            else if(ReportXMLMismatches)
            {
                Console::File() << "Vector Mismatch: S_COLLADA.I_library_images\n";
            }

            for(UINT ResultIndex = 0; ResultIndex < Result.Length(); ResultIndex++)
            {
                L_library_images.PushEnd(new S_library_images(Result[ResultIndex]));
            }
        }*/

        File << "\tS_" << CurEntry.Name << "::~S_" << CurEntry.Name << "()" << endl;
        File << "\t{" << endl;
        
        if(CurEntry.Children.Length() > 0)
        {
            for(UINT ChildIndex = 0; ChildIndex < CurEntry.Children.Length(); ChildIndex++)
            {
                const XMLSchemaChildEntry &CurChild = CurEntry.Children[ChildIndex];
                if(CurChild.Count == 1)
                {
                    File << "\t\tif(I_" << CurChild.Name << " != NULL)" << endl;
                    File << "\t\t{" << endl;
                    File << "\t\t\tdelete I_" << CurChild.Name << ";" << endl;
                    File << "\t\t}" << endl;
                }
                else
                {
                    File << "\t\tL_" << CurChild.Name << ".DeleteMemory();" << endl;
                }
            }
        }

        File << "\t}" << endl;
        File << endl;
    }

    File << "}" << endl << endl;
    File << "#endif" << endl;
}

XMLFile::XMLFile()
{
    _Root = NULL;
}

XMLFile::~XMLFile()
{
    FreeMemory();
}

void XMLFile::FreeMemory()
{
    if(_Root)
    {
        delete _Root;
        _Root = NULL;
    }
}

void XMLFile::Load(const String &Filename)
{
    FreeMemory();

    Vector<BYTE> Data;
    Utility::GetFileData(Filename, Data);

    const UINT FileLength = Data.Length();
    
    _Root = new XMLNode("Root");
    Vector<XMLNode*> Stack;
    Stack.PushEnd(_Root);

    bool FileDone = false;
    UINT DataIndex = 0;
    while(!FileDone)
    {
        if(Data[DataIndex] == '<')
        {
            DataIndex++;

            String ActiveLine;
            bool TerminalFound = false;
            bool IsScopeClosure = false;
            if(Data[DataIndex] == '/')
            {
                IsScopeClosure = true;
                DataIndex++;
            }
            while(!TerminalFound)
            {
                char C = Data[DataIndex];
                if(C == '>')
                {
                    TerminalFound = true;
                }
                else
                {
                    if(C != 0x0D && C != 0x0A)
                    {
                        ActiveLine.PushEnd(C);
                    }
                }
                DataIndex++;
            }
            if(IsScopeClosure)
            {
                if(Stack.Last()->Name != ActiveLine)
                {
                    SignalError("Invalid scope closure");
                }
                Stack.PopEnd();
            }
            else
            {
                if(ActiveLine.Last() == '/')
                {
                    //
                    // New leaf node
                    //
                    ActiveLine.PopEnd();
                    XMLNode *NewNode = new XMLNode(ActiveLine);
                    Stack.Last()->AddChild(NewNode);
                }
                else
                {
                    XMLNode *NewNode = new XMLNode(ActiveLine);
                    if(NewNode->Name == "?xml")
                    {
                        delete NewNode;
                    }
                    else
                    {
                        Stack.Last()->AddChild(NewNode);
                        Stack.PushEnd(NewNode);
                    }
                }
            }
        }
        else
        {
            String ActiveLine;
            bool TerminalFound = false;
            while(!TerminalFound && DataIndex < FileLength)
            {
                char C = Data[DataIndex];
                if(C == '<')
                {
                    TerminalFound = true;
                }
                else
                {
                    if(C != 0x0D && C != 0x0A && (ActiveLine.Length() > 0 || C != ' '))
                    {
                        ActiveLine.PushEnd(C);
                    }
                    DataIndex++;
                }
            }
            if(ActiveLine.Length() > 0)
            {
                if(Stack.Last()->Value.Length() > 0)
                {
                    SignalError("Multiple node values");
                }
                Stack.Last()->Value = ActiveLine;
            }
        }
        FileDone = (DataIndex == FileLength);
    }
    if(Stack.Length() != 1 || Stack[0]->Name != "Root")
    {
        SignalError("Invalid stack at EOF");
    }
}

void XMLFile::Save(const String &Filename)
{
    ofstream File(Filename.CString());
    File << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    _Root->Save(File);
}

void XMLFile::SaveSchema(const String &SourceFilename, const String &HeaderFilename, const String &Namespace)
{
    XMLSchema Schema;
    Schema.ObserveNode(*_Root);
    Schema.SaveSourceFile(SourceFilename, Namespace);
    Schema.SaveHeaderFile(HeaderFilename, Namespace);
}
