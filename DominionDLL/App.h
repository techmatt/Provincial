class App
{
public:
    void Init();
    
    UINT32 ProcessCommand(const String &command);
    int QueryIntegerByName(const String &s);
    const char *QueryStringByName(const String &s);
    double QueryDoubleByName(const String &s);

private:
    
    AppParameters _parameters;
    
    DominionGame _game;
    CardDatabase _cards;
    GameOptions _options;

    MetaTestChamber _chamber;
    
    String _queryString;
    double _AITestResult;
};
