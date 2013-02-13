
struct GameData;
class Player;

struct GameOptions
{
    GameOptions()
    {
        startingCondition = StartingCondition34Split;
        prosperity = false;
    }
    Card* RandomSupplyCard(const CardDatabase &cards) const;
    void SetupGame(const CardDatabase &cards, const String &options);
    void RandomizeSupplyPiles(const CardDatabase &cards);
    void RandomizeSupplyPiles(const CardDatabase &cards, const Vector<String> &requiredCards);
    String ToString() const;

    Vector<Card*> supplyPiles;
    StartingCondition startingCondition;
    bool prosperity;
};

class Log
{
public:
    void operator()(const String &s)
    {
        _events.PushEnd(s);
    }
    void Reset()
    {
        _events.FreeMemory();
    }
    const Vector<String>& Events()
    {
        return _events;
    }

private:
    Vector<String> _events;
};

//
// BuyLedger is a record of all voluntary purchases made by each player, used by TestChamber
//
struct BuyLedger
{
    void Reset()
    {
        cardsBought.FreeMemory();
    }
    void RecordBuy(Card *c)
    {
        if(!cardsBought.Contains(c)) cardsBought.PushEnd(c);
    }

    Vector<Card*> cardsBought;
};

struct PlayerInfo
{
    PlayerInfo(UINT _index, const String &_name, Player *_controller)
    {
        index = _index;
        name = _name;
        controller = _controller;
    }
    PlayerInfo()
    {

    }

    UINT index;
    String name;
    Player *controller;

    mutable BuyLedger ledger;
};

struct BaseCards
{
    Card *copper;
    Card *silver;
    Card *gold;
    Card *platinum;
    Card *potion;
    Card *estate;
    Card *duchy;
    Card *province;
    Card *colony;
    Card *curse;

    Card *moat;
    Card *bridge;
    Card *coppersmith;
    Card *quarry;
    Card *talisman;
    Card *hoard;
    Card *goons;
    Card *grandMarket;
    Card *peddler;
    Card *bank;
    Card *royalSeal;
    Card *watchtower;
    Card *lighthouse;
    Card *treasury;
    Card *treasureMap;
    Card *tradeRoute;
    Card *mint;
    Card *champion;
    Card *gardener;
    Card *grandCourt;
    Card *plunder;
    Card *promisedLand;
    Card *trailblazer;
    Card *evangelist;
    Card *furnace;
    Card *witchdoctor;
};

struct GameData
{
    void NewGame(const Vector<PlayerInfo> &playerList, const GameOptions &_options);
    void Init(const CardDatabase &_cards);
    
    __forceinline int SupplyIndex(Card *c) const
    {
        int result = supplyCards.FindFirstIndex(c);
        //Assert(result != -1, "card not found");
        return result;
    }

    mutable Log log;
    const CardDatabase *cards;
    BaseCards baseCards;
    Vector<Card*> supplyCards;
    Vector<PlayerInfo> players;

    Vector<Card*> costModifyingCards;
    Vector<Card*> reactionCards;
    GameOptions options;

    //
    // Fast "in-supply" checks for certain cards
    //
    bool useGainList;
    bool quarryInSupply;
    bool coppersmithInSupply;
    bool bridgeInSupply;
    bool talismanInSupply;
    bool hoardInSupply;
    bool goonsInSupply;
    bool grandMarketInSupply;
    bool treasuryInSupply;
    bool royalSealInSupply;
    bool watchtowerInSupply;
    bool tradeRouteInSupply;
    bool mintInSupply;
    bool championInSupply;
    bool gardenerInSupply;
    bool grandCourtInSupply;
    bool plunderInSupply;
    bool promisedLandInSupply;
    bool furnaceInSupply;

private:
    void InitBaseCards();
};
