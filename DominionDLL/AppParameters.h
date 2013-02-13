struct AppParameters
{
    void Init(const String &parameterFilename)
    {
        ParameterFile file(parameterFilename);
        useCustomCards = file.GetBoolean("useCustomCards");
    }

    bool useCustomCards;
};