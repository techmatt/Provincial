XMLNode::XMLNode(const String &S)
{
    Init(S);
}

XMLNode::~XMLNode()
{
    FreeMemory();
}

void XMLNode::FreeMemory()
{
    Attributes.FreeMemory();
    
    for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
    {
        delete Children[ChildIndex];
    }
    Children.FreeMemory();
}

void XMLNode::Save(ofstream &File)
{
    if(Name == "@DELETED")
    {
        return;
    }
    if(Name == "Root")
    {
        for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
        {
            Children[ChildIndex]->Save(File);
        }
    }
    else
    {
        File << "<" << Name;
        for(UINT AttributeIndex = 0; AttributeIndex < Attributes.Length(); AttributeIndex++)
        {
            const XMLAttribute &CurAttribute = Attributes[AttributeIndex];
            File << ' ' << CurAttribute.Name << "=\"" << CurAttribute.Value << "\"";
        }
        if(Children.Length() == 0)
        {
            File << "/>" << endl;
        }
        else
        {
            File << '>' << endl;
            for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
            {
                Children[ChildIndex]->Save(File);
            }
            File << "</" << Name << '>' << endl;
        }
    }
}

void XMLNode::Init(const String &S)
{
    if(S[0] == '?')
    {
        Name = "?xml";
    }
    else
    {
        //<image id="material20-image" name="material20-image">
        Vector<String> Words;
        S.Partition(' ', Words);
        if(Words.Length() == 0 || Words[0].Length() == 0)
        {
            SignalError("Invalid node header");
        }
        Name = Words[0];

        //svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="127pt" height="104pt" viewBox="0 0 127 104" version="1.1"

        if(Words.Length() >= 2)
        {
            S.Partition('\"', Words, true);

            if(Words.Last() == " ")
            {
                Words.PopEnd();
            }

            if(Words.Length() % 2 != 0)
            {
                SignalError("Invalid node header");
            }

            String P0, P1;
            Words[0].PartitionAboutIndex(Words[0].FindFirstIndex(' '), P0, P1);
            Assert(P0 == Name, "Invalid node header");
            Words[0] = P1;

            const UINT AttributeCount = Words.Length() / 2;
            Attributes.Allocate(AttributeCount);
            for(UINT AttributeIndex = 0; AttributeIndex < AttributeCount; AttributeIndex++)
            {
                XMLAttribute &CurAttribute = Attributes[AttributeIndex];

                CurAttribute.Name = Words[AttributeIndex * 2 + 0];
                Assert(CurAttribute.Name.Last() == '=', "Invalid node header");
                CurAttribute.Name.PopEnd();

                while(CurAttribute.Name[0] == ' ')
                {
                    CurAttribute.Name.PopFront();
                }
                
                CurAttribute.Value = Words[AttributeIndex * 2 + 1];
            }

            /*Attributes.Allocate(Words.Length() - 1);
            for(UINT WordIndex = 1; WordIndex < Words.Length(); WordIndex++)
            {
                const String &CurWord = Words[WordIndex];
                Vector<String> EqPartition;
                CurWord.Partition(String("=\""), EqPartition);
                if(EqPartition.Length() != 2)
                {
                    SignalError("Invalid node header");
                }
                if(EqPartition[1].Last() != '\"' && EqPartition[1].Last() != '?')
                {
                    const String &DD = EqPartition[1];
                    SignalError("Invalid node header");
                }
                EqPartition[1].PopEnd();
                
                XMLAttribute &CurAttribute = Attributes[WordIndex - 1];
                CurAttribute.Name = EqPartition[0];
                CurAttribute.Value = EqPartition[1];
            }*/
        }
    }
}

void XMLNode::MakeChildEntryList(Vector<XMLSchemaChildEntry> &List) const
{
    List.FreeMemory();
    for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
    {
        const XMLNode &CurChild = *(Children[ChildIndex]);
        String CStyleChildName = XMLNameToCStyleName(CurChild.Name);
        bool ChildNameFound = false;
        for(UINT ListIndex = 0; ListIndex < List.Length() && !ChildNameFound; ListIndex++)
        {
            if(List[ListIndex].Name == CStyleChildName)
            {
                List[ListIndex].Count++;
                ChildNameFound = true;
            }
        }
        if(!ChildNameFound)
        {
            List.PushEnd(XMLSchemaChildEntry(CStyleChildName));
        }
    }
}

void XMLNode::GetChildrenByName(const String &Name, Vector<XMLNode*> &Result) const
{
    Result.FreeMemory();
    for(UINT ChildIndex = 0; ChildIndex < Children.Length(); ChildIndex++)
    {
        if(Children[ChildIndex]->Name == Name)
        {
            Result.PushEnd(Children[ChildIndex]);
        }
    }
}

String XMLNode::GetAttributeByName(const String &Name) const
{
    for(UINT AttributeIndex = 0; AttributeIndex < Attributes.Length(); AttributeIndex++)
    {
        if(Attributes[AttributeIndex].Name == Name)
        {
            return Attributes[AttributeIndex].Value;
        }
    }
    return String();
}
