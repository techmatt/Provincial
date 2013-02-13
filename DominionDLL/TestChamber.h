
struct TestParameters
{
    GameOptions options;
    Player* players[2];
    UINT minGameCount, maxGameCount;
};

//
// New mutations should be granted the average rating at the time
//
struct ChamberParameters
{
    ChamberParameters()
    {
        poolSize = 100;
        leaderCount = 5;

        visualizationGameCount = 5000;
        standardGameCount = 2000;

        leaderCurveMin = 1.0;
        leaderCurveMax = 1.5;

        mutationTerminationProbability = 0.3;
    }

    UINT poolSize;
    UINT leaderCount;

    UINT visualizationGameCount;
    UINT standardGameCount;

    double leaderCurveMin;
    double leaderCurveMax;
    
    double mutationTerminationProbability;
};

struct TestResult
{
    double winRatio[2];
    Vector<double> buyRatio;
};

struct TestPlayer
{
    TestPlayer(PlayerHeuristic *_p)
    {
        p = _p;
        rating = 0.0;
    }
    TestPlayer(PlayerHeuristic *_p, double _rating)
    {
        p = _p;
        rating = _rating;
    }
    String VisualizationDescription(const Vector<Card*> &supplyCards, bool ignoreBuyID, bool ignoreCounts = false) const;
    PlayerHeuristic *p;
    double rating;
    String buyID;
};

class TestChamber
{
public:
    void FreeMemory();
    void StrategizeStart(const CardDatabase &cards, const GameOptions &options, const String &directory, const String &metaSuffix);
    void StrategizeStep(const CardDatabase &cards);
    TestResult Test(const CardDatabase &cards, const TestParameters &params, bool useConsole=true);

private:
    friend class MetaTestChamber;
    void ComputeCounters(const CardDatabase &cards, const String &filename);
    void ComputeProgression(const CardDatabase &cards, TestPlayer* leader, const Vector<TestPlayer*> &players, const String &filename);
    void ComputeLeaderboard(const CardDatabase &cards, const Vector<TestPlayer*> &players, const String &filename, UINT gameCount);
    void InitializePool(const CardDatabase &cards);
    Grid<TestResult> TestPool(const CardDatabase &cards, const String &filename);

    void AssignBuyIDs(const Grid<TestResult> &poolResults);
    void AssignNewLeaders(const CardDatabase &cards);
    void GenerateNewPool(const CardDatabase &cards);
    void SaveVisualizationFiles(const CardDatabase &cards);

    Grid<TestResult> RunAllPairsTests(const CardDatabase &cards, const Vector<TestPlayer*> &playersA, const Vector<TestPlayer*> &playersB, UINT minGameCount, UINT maxGameCount);

    Vector<TestPlayer*> _pool;
    Vector<TestPlayer*> _leaders;

    Vector<TestPlayer*> _bestLeaderHistory;
    Vector<TestPlayer*> _allLeaderHistory;
    Vector<UINT> _allLeaderHistoryHashes;
    
    Vector<double> _leaderWeights;

    ChamberParameters _parameters;
    GameOptions _gameOptions;
    Vector<Card*> _supplyCards;

    String _metaSuffix;
    int _generation;
    String _directory;
};

struct TestChamberTask : public WorkerThreadTask
{
    void Run(ThreadLocalStorage *threadLocalStorage);
    TestChamber *chamber;
    const CardDatabase *cards;
    TestParameters params;
    TestResult *result;
};
