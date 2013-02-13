/*
ParameterFile.h
Written by Matthew Fisher

the Parameter class loads a paramater file as a simple set of "Parameter=Option" lines.
*/

struct ParameterEntry
{
    ParameterEntry()
    {

    }
    ParameterEntry(const String &_Name, const String &_Value)
    {
        Name = _Name;
        Value = _Value;
        if(Name.Last() == ' ')
        {
            Name.PopEnd();
        }
    }
    String Name;
    String Value;
};

class ParameterFile
{
public:
    ParameterFile() {}
    ParameterFile(const String &Filename);
    
    String GetRequiredString(const String &ParameterName) const;
    String GetOptionalString(const String &ParameterName) const;
    
    int GetInteger(const String &ParameterName) const;
    bool GetBoolean(const String &ParameterName) const;
    UINT GetUnsignedInteger(const String &ParameterName) const;
    double GetDouble(const String &ParameterName) const;
    float GetFloat(const String &ParameterName) const;
    Vec3f GetVec3(const String &ParameterName) const;
    const void* GetPointer(const String &ParameterName, const String &Type) const;

    String GetString(const String &ParameterName, const String &Default) const;
    int GetInteger(const String &ParameterName, int Default) const;
    bool GetBoolean(const String &ParameterName, bool Default) const;
    UINT GetUnsignedInteger(const String &ParameterName, UINT Default) const;
    double GetDouble(const String &ParameterName, double Default) const;
    float GetFloat(const String &ParameterName, float Default) const;
    Vec3f GetVec3(const String &ParameterName, const Vec3f &Default) const;
    const void* GetPointer(const String &ParameterName, const String &Type, const void *Default) const;

    void OutputAllParameters(ostream &os, const String &EndLine) const;

    void SetInteger(const String &ParameterName, UINT Value);
    void SetPointer(const String &ParameterName, const void *Pointer, const String &Type);

    void AddFile(const String &filename);    
    
private:
    void AddFile(const String &filename, const Vector<String> &baseDirectories);

    ParameterEntry* FindParameter(const String &ParameterName);
    const ParameterEntry* FindParameter(const String &ParameterName) const;

    Vector<ParameterEntry> Parameters;
};
