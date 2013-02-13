/*
ParameterFile.cpp
Written by Matthew Fisher

the Parameter class loads a paramater file as a simple set of "Parameter=Option" lines.
*/

ParameterFile::ParameterFile(const String &filename)
{
    Vector<String> baseDirectories;
    AddFile(filename, baseDirectories);
}

void ParameterFile::AddFile(const String &filename)
{
    Vector<String> baseDirectories;
    AddFile(filename, baseDirectories);
}

void ParameterFile::AddFile(const String &filename, const Vector<String> &baseDirectories)
{
    String finalFilename = filename;
    bool fileFound = Utility::FileExists(finalFilename);
    for(UINT baseDirectoryIndex = 0; baseDirectoryIndex < baseDirectories.Length() && !fileFound; baseDirectoryIndex++)
    {
        const String &curDirectory = baseDirectories[baseDirectoryIndex];
        finalFilename = curDirectory + filename;
        fileFound = Utility::FileExists(finalFilename);
    }
    PersistentAssert(fileFound, "Parameter file not found");
    
    Vector<String> childBaseDirectories = baseDirectories;
    if(filename.Contains("/") || filename.Contains("\\"))
    {
        Vector<String> parts = filename.PartitionAboutIndex(filename.FindAndReplace("\\", "/").FindLastIndex('/'));
        childBaseDirectories.PushEnd(parts[0] + "/");
    }

    ifstream File(finalFilename.CString());
    
    Vector<String> Lines;
    Utility::GetFileLines(File, Lines);
    for(UINT i = 0; i < Lines.Length(); i++)
    {
        String &CurLine = Lines[i];
        if(CurLine.StartsWith("#include "))
        {
            Vector<String> Partition = CurLine.Partition("#include ");
            PersistentAssert(Partition.Length() == 1, String("Invalid line in parameter file: ") + CurLine);
            String IncludeFilename = Partition[0];
            IncludeFilename.Partition("\"", Partition);
            PersistentAssert(Partition.Length() == 1, String("Invalid line in parameter file: ") + CurLine);
            AddFile(Partition[0], childBaseDirectories);
        }
        else if(CurLine.Length() > 2 && CurLine[0] != '#')
        {
            Vector<String> Partition = CurLine.Partition('=');
            PersistentAssert(Partition.Length() == 2, String("Invalid line in parameter file: ") + CurLine);
            Parameters.PushEnd(ParameterEntry(Partition[0], Partition[1]));
        }
    }
}

ParameterEntry* ParameterFile::FindParameter(const String &ParameterName)
{
    for(auto ParameterIterator = Parameters.begin(); ParameterIterator != Parameters.end(); ParameterIterator++)
    {
        auto &CurEntry = *ParameterIterator;
        if(CurEntry.Name == ParameterName)
        {
            return &CurEntry;
        }
    }
    return NULL;
}

const ParameterEntry* ParameterFile::FindParameter(const String &ParameterName) const
{
    for(auto ParameterIterator = Parameters.begin(); ParameterIterator != Parameters.end(); ParameterIterator++)
    {
        auto &CurEntry = *ParameterIterator;
        if(CurEntry.Name == ParameterName)
        {
            return &CurEntry;
        }
    }
    return NULL;
}

void ParameterFile::OutputAllParameters(ostream &os, const String &EndLine) const
{
    for(UINT parameterIndex = 0; parameterIndex < Parameters.Length(); parameterIndex++)
    {
        const ParameterEntry &curParameter = Parameters[parameterIndex];
        os << curParameter.Name << " = " << curParameter.Value << EndLine;
    }
}

String ParameterFile::GetRequiredString(const String &ParameterName) const
{
    for(UINT i = 0; i < Parameters.Length(); i++)
    {
        if(Parameters[i].Name.MakeLowercase() == ParameterName.MakeLowercase())
        {
            return Parameters[i].Value;
        }
    }
    PersistentSignalError(String("Parameter not found: ") + ParameterName);
    return String("Parameter not found");
}

String ParameterFile::GetOptionalString(const String &ParameterName) const
{
    for(UINT i = 0; i < Parameters.Length(); i++)
    {
        if(Parameters[i].Name.MakeLowercase() == ParameterName.MakeLowercase())
        {
            return Parameters[i].Value;
        }
    }
    return String();
}

bool ParameterFile::GetBoolean(const String &ParameterName) const
{
    String Str = GetRequiredString(ParameterName);
    Str = Str.MakeLowercase();
    if(Str == "true")
    {
        return true;
    }
    else if(Str == "false")
    {
        return false;
    }
    else
    {
        SignalError(String("Invalid boolean value: ") + Str);
        return false;
    }
}

int ParameterFile::GetInteger(const String &ParameterName) const
{
    return GetRequiredString(ParameterName).ConvertToInteger();
}

UINT ParameterFile::GetUnsignedInteger(const String &ParameterName) const
{
    return GetRequiredString(ParameterName).ConvertToUnsignedInteger();
}

double ParameterFile::GetDouble(const String &ParameterName) const
{
    return GetRequiredString(ParameterName).ConvertToDouble();
}

float ParameterFile::GetFloat(const String &ParameterName) const
{
    return GetRequiredString(ParameterName).ConvertToFloat();
}

Vec3f ParameterFile::GetVec3(const String &ParameterName) const
{
    String VecString = GetRequiredString(ParameterName);
    Vector<String> Elements;
    VecString.Partition(' ', Elements);
    Assert(Elements.Length() == 3, "Vector with invalid element count");
    Vec3f Result;
    for(UINT i = 0; i < 3; i++)
    {
        Result[i] = Elements[i].ConvertToFloat();
    }
    return Result;
}

String ParameterFile::GetString(const String &ParameterName, const String &Default) const
{
    String Result = GetOptionalString(ParameterName);
    if(Result.Length() == 0)
    {
        return Default;
    }
    else
    {
        return Result;
    }
}

bool ParameterFile::GetBoolean(const String &ParameterName, bool Default) const
{
    String Str = GetOptionalString(ParameterName);
    Str = Str.MakeLowercase();
    if(Str == "true")
    {
        return true;
    }
    else if(Str == "false")
    {
        return false;
    }
    else
    {
        return Default;
    }
}

int ParameterFile::GetInteger(const String &ParameterName, int Default) const
{
    return GetString(ParameterName, String(Default)).ConvertToInteger();
}

UINT ParameterFile::GetUnsignedInteger(const String &ParameterName, UINT Default) const
{
    return GetString(ParameterName, String(Default)).ConvertToUnsignedInteger();
}

double ParameterFile::GetDouble(const String &ParameterName, double Default) const
{
    return GetString(ParameterName, String(Default)).ConvertToDouble();
}

float ParameterFile::GetFloat(const String &ParameterName, float Default) const
{
    return GetString(ParameterName, String(Default)).ConvertToFloat();
}

Vec3f ParameterFile::GetVec3(const String &ParameterName, const Vec3f &Default) const
{
    String VecString = GetOptionalString(ParameterName);
    if(VecString.Length() == 0)
    {
        return Default;
    }
    Vector<String> Elements;
    VecString.Partition(' ', Elements);
    Assert(Elements.Length() == 3, "Vector with invalid element count");
    Vec3f Result;
    for(UINT i = 0; i < 3; i++)
    {
        Result[i] = Elements[i].ConvertToFloat();
    }
    return Result;
}

const void* ParameterFile::GetPointer(const String &ParameterName, const String &Type, const void *Default) const
{
    const ParameterEntry *CurEntry = FindParameter(ParameterName);
    if(CurEntry == NULL)
    {
        return Default;
    }
    else
    {
        return GetPointer(ParameterName, Type);
    }
}

const void* ParameterFile::GetPointer(const String &ParameterName, const String &Type) const
{
    const String &Value = GetRequiredString(ParameterName);
    Vector<String> Words;
    Value.Partition('@', Words);
    Assert(Words.Length() == 2, "Invalid pointer parameter");
    Assert(Words[0] == Type, "Pointer type mismatch");
    return (const void *)(Words[1].ConvertToUnsignedInteger());
}

void ParameterFile::SetInteger(const String &ParameterName, UINT Value)
{
    ParameterEntry *CurEntry = FindParameter(ParameterName);
    if(CurEntry == NULL)
    {
        Parameters.PushEnd(ParameterEntry(ParameterName, String(Value)));
    }
    else
    {
        CurEntry->Value = String(Value);
    }
}

void ParameterFile::SetPointer(const String &ParameterName, const void *Pointer, const String &Type)
{
    ParameterEntry *CurEntry = FindParameter(ParameterName);
    String EncodedPointer = Type + String('@') + String((UINT)Pointer);
    if(CurEntry == NULL)
    {
        Parameters.PushEnd(ParameterEntry(ParameterName, EncodedPointer));
    }
    else
    {
        CurEntry->Value = EncodedPointer;
    }
}
