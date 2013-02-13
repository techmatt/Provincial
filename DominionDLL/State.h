
class Event;
struct State;

struct CardPlayInfo
{
    CardPlayInfo() {}
    explicit CardPlayInfo(Card *c, int _turnsLeft)
    {
        card = c;
        copies = 1;
        turnsLeft = _turnsLeft;
    }
    Card *card;

    //
    // This is only used by throne room and king's court. We can't use "bound to" because of throne rooming throne rooms, the 2nd time a TR is played, the throne room would have to be rebound.
    //
    int copies;

    //
    // These are used by durations
    //
    int turnsLeft;
};

struct PlayerState
{
    void NewGame(const GameData &data);
    int ActionCount() const;
    int TreasureCount() const;
    int VictoryCount() const;
    int TotalCards() const;
    int MoneyTotal() const;
    bool CardInPlay(Card *c) const
    {
        for(const CardPlayInfo &play : playArea) if(play.card == c) return true;
        return false;
    }
    //map<Card*,int> CardCounts() const;

    Vector<Card*> hand;
    Vector<Card*> deck;
    Vector<Card*> discard;
    Vector<CardPlayInfo> playArea;
    Vector<Card*> islandZone;
    int actions;
    int buys;
    int money;
    int VPTokens;
    int turns;
};

struct DecisionResponse
{
    DecisionResponse()
    {
        choice = -1;
        singleCard = NULL;
    }

    UINT choice;
    Card *singleCard;
    Vector<Card*> cards;
};

class DecisionState
{
public:
    bool IsTrivial() const;
    DecisionResponse TrivialResponse() const;
    
    void SelectCards(Card *c, UINT minCards, UINT maxCards)
    {
        activeCard = c;
        type = DecisionSelectCards;
        minimumCards = minCards;
        maximumCards = maxCards;
        controllingPlayer = -1;
        cardChoices.FreeMemory();
    }
    void MakeDiscreteChoice(Card *c, UINT optionCount)
    {
        activeCard = c;
        type = DecisionDiscreteChoice;
        minimumCards = optionCount;
        maximumCards = optionCount;
        controllingPlayer = -1;
    }
    void GainCardFromSupply(State &s, Card *c, int minCost, int maxCost, CardFilter filter = FilterAny);
    void GainTreasureFromSupply(State &s, Card *c, int minCost, int maxCost);
    void GainVictoryFromSupply(State &s, Card *c, int minCost, int maxCost);
    __forceinline void AddUniqueCard(Card *c)
    {
        if(!cardChoices.Contains(c)) cardChoices.PushEnd(c);
    }
    __forceinline void AddUniqueCards(Vector<Card*> cards)
    {
        for(Card *c : cards) AddUniqueCard(c);
    }

    DecisionType type;
    Card *activeCard;

    Vector<Card*> cardChoices;
    UINT minimumCards;
    UINT maximumCards;

    //
    // if controllingPlayer is -1, the active player is the one making the decision.
    //
    int controllingPlayer;

    //
    // Options are separated by |
    //
    String text;
};

struct SupplyEntry
{
    UINT count;
    UINT tradeRouteToken;
};

struct State
{
    static const UINT playerMaximum = 2;
    static const UINT maxSupply = 20;

    //
    // Game Control
    //
    void NewGame(const GameData &_data);
    void AdvanceToNextDecision(UINT recursionDepth);
    void AdvancePhase();
    void ProcessDecision(const DecisionResponse &response);

    //
    // Utility
    //
    void DrawCards(UINT playerIndex, UINT cardCount);
    Card* DrawCard(UINT playerIndex);
    void DiscardCard(UINT playerIndex, Card *c);
    void PlayCard(UINT playerIndex, Card *c);
    void Shuffle(UINT playerIndex);
    void ProcessAction(Card *c);
    void ProcessTreasure(Card *c);
    void CheckEndConditions();
    void ReorderDeck(Card *source, UINT playerIndex, UINT cardCount);
    
    //
    // Query
    //
    bool GameDone() const;
    int PlayerScore(UINT playerIndex) const;
    UINT SupplyCost(int supplyIndex) const;
    UINT SupplyCost(Card *c) const;
    UINT SupplyCount(Card *c) const;
    UINT EmptySupplyPiles() const;
    UINT ActionsPlayedThisTurn() const;
    Vector<int> WinningPlayers() const;

    //
    // Logging
    //
    void Log(UINT playerIndex, const String &s);
    void Log(const String &s);
    void LogIndent(UINT indentLevel, UINT playerIndex, const String &s);
    void LogIndent(UINT indentLevel, const String &s);

    //
    // Internal triggers
    //
    void BuyPhaseStart();

    //
    // State
    //
    const GameData *data;
    UINT player;
    Phase phase;
    DecisionState decision;
    PlayerState players[playerMaximum];

    //
    // Events are entities that either require a decision from the player, or may trigger a response from the user.
    // If stack is empty, then the active decision is determined by phase.
    // If stack is non-empty, then the active decision is determined by the top of the stack.
    //
    Vector<Event*> stack;

    //
    // List of cards gained this turn
    //
    Vector<Card*> gainList;

    //
    // List of cards gained by the previous player
    //
    Vector<Card*> prevGainList;

    int tradeRouteValue;

    SupplyEntry supply[maxSupply];
};
