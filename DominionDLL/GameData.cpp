#include "Main.h"

String GameOptions::ToString() const
{
    String s;
    for(Card *c : supplyPiles) s += c->name + ",";
    s.PopEnd();

    if(startingCondition == StartingCondition25Split) s += "@25Split";
    else if(startingCondition == StartingCondition34Split) s += "@34Split";
    else if(startingCondition == StartingConditionRandom) s += "@RandomSplit";

    if(prosperity) s += "@Prosperity";
    else s += "@NoProsperity";

    return s;
}

Card* GameOptions::RandomSupplyCard(const CardDatabase &cards) const
{
    int baseSupplyCount = 7;
    if(prosperity) baseSupplyCount += 2;
    int supplyPileCount = supplyPiles.Length() + baseSupplyCount;
    
    UINT supplyIndex = rand() % supplyPileCount;
    if(supplyIndex < supplyPiles.Length())
    {
        return supplyPiles[supplyIndex];
    }
    else if(prosperity)
    {
        int baseSupplyIndex = supplyIndex - supplyPiles.Length();
        if(baseSupplyIndex == 0) return cards.GetCard("copper");
        if(baseSupplyIndex == 1) return cards.GetCard("silver");
        if(baseSupplyIndex == 2) return cards.GetCard("gold");
        if(baseSupplyIndex == 3) return cards.GetCard("platinum");
        if(baseSupplyIndex == 4) return cards.GetCard("estate");
        if(baseSupplyIndex == 5) return cards.GetCard("duchy");
        if(baseSupplyIndex == 6) return cards.GetCard("province");
        if(baseSupplyIndex == 7) return cards.GetCard("colony");
        if(baseSupplyIndex == 8) return cards.GetCard("curse");
    }
    else
    {
        int baseSupplyIndex = supplyIndex - supplyPiles.Length();
        if(baseSupplyIndex == 0) return cards.GetCard("copper");
        if(baseSupplyIndex == 1) return cards.GetCard("silver");
        if(baseSupplyIndex == 2) return cards.GetCard("gold");
        if(baseSupplyIndex == 3) return cards.GetCard("estate");
        if(baseSupplyIndex == 4) return cards.GetCard("duchy");
        if(baseSupplyIndex == 5) return cards.GetCard("province");
        if(baseSupplyIndex == 6) return cards.GetCard("curse");
    }
    SignalError("Card not found");
    return NULL;
}

void GameOptions::SetupGame(const CardDatabase &cards, const String &options)
{
    Vector<String> parts = options.Partition("|");
    
    Vector<String> requiredCards = parts[0].Partition(",");
    for(String &s : requiredCards)
    {
        while(s.StartsWith(" ") && s.Length() > 0) s.PopFront();
        while(s.EndsWith(" ") && s.Length() > 0) s.PopEnd();
    }

    if(parts[1] == "34Split") startingCondition = StartingCondition34Split;
    if(parts[1] == "25Split") startingCondition = StartingCondition25Split;

    prosperity = (parts[2] == "Prosperity");

    RandomizeSupplyPiles(cards, requiredCards);
}

void GameOptions::RandomizeSupplyPiles(const CardDatabase &cards)
{
    //
    // Standard dominion rules suggest that prosperity should be N * 10%, where N is the # of prosperity cards
    // I just choose it to be 33% of the time.
    //
    prosperity = (rand() % 3 == 0);

    if(rand() % 6 == 0) startingCondition = StartingCondition25Split;
    else startingCondition = StartingCondition34Split;

    Vector<String> requiredCards;
    RandomizeSupplyPiles(cards, requiredCards);
}

void GameOptions::RandomizeSupplyPiles(const CardDatabase &cards, const Vector<String> &requiredCards)
{
    for(UINT cardIndex = 0; cardIndex < requiredCards.Length() && cardIndex < 10; cardIndex++)
    {
        Card *requiredCard = cards.GetCard(requiredCards[cardIndex]);
        if(requiredCard != NULL) supplyPiles.PushEnd(requiredCard);
    }
    while(supplyPiles.Length() < 10)
    {
        Card *newCard;
        do
        {
            newCard = cards.RandomSupplyCard();
        } while(supplyPiles.Contains(newCard));
        supplyPiles.PushEnd(newCard);
    }
    supplyPiles.Sort([](const Card *a, const Card *b)
    { 
        if(a->cost < b->cost) return true;
        if(a->cost > b->cost) return false;
        return (a->name < b->name);
    });
}

void GameData::NewGame(const Vector<PlayerInfo> &playerList, const GameOptions &_options)
{
    PersistentAssert(playerList[0].index == 0 && playerList[1].index == 1, "Player indices are invalid");

    options = _options;
    players = playerList;

    supplyCards.FreeMemory();
    supplyCards.PushEnd(cards->GetCard("copper"));
    supplyCards.PushEnd(cards->GetCard("silver"));
    supplyCards.PushEnd(cards->GetCard("gold"));

    if(options.prosperity) supplyCards.PushEnd(cards->GetCard("platinum"));

    supplyCards.PushEnd(cards->GetCard("estate"));
    supplyCards.PushEnd(cards->GetCard("duchy"));
    supplyCards.PushEnd(cards->GetCard("province"));

    if(options.prosperity) supplyCards.PushEnd(cards->GetCard("colony"));

    supplyCards.PushEnd(cards->GetCard("curse"));

    for(Card *c : options.supplyPiles) supplyCards.PushEnd(c);

    reactionCards.FreeMemory();
    for(Card *c : supplyCards)
    {
        if(c->isReaction) reactionCards.PushEnd(c);
    }

    for(PlayerInfo &p : players) p.ledger.Reset();

    coppersmithInSupply = supplyCards.Contains(cards->GetCard("coppersmith"));
    bridgeInSupply = supplyCards.Contains(cards->GetCard("bridge"));
    quarryInSupply = supplyCards.Contains(cards->GetCard("quarry"));
    talismanInSupply = supplyCards.Contains(cards->GetCard("talisman"));
    hoardInSupply = supplyCards.Contains(cards->GetCard("hoard"));
    goonsInSupply = supplyCards.Contains(cards->GetCard("goons"));
    grandMarketInSupply = supplyCards.Contains(cards->GetCard("grand market"));
    treasuryInSupply = supplyCards.Contains(cards->GetCard("treasury"));
    watchtowerInSupply = supplyCards.Contains(cards->GetCard("watchtower"));
    royalSealInSupply = supplyCards.Contains(cards->GetCard("royal seal"));
    tradeRouteInSupply = supplyCards.Contains(cards->GetCard("trade route"));
    mintInSupply = supplyCards.Contains(cards->GetCard("mint"));
    championInSupply = supplyCards.Contains(cards->GetCard("champion"));
    gardenerInSupply = supplyCards.Contains(cards->GetCard("gardener"));
    grandCourtInSupply = supplyCards.Contains(cards->GetCard("grand court"));
    plunderInSupply = supplyCards.Contains(cards->GetCard("plunder"));
    promisedLandInSupply = supplyCards.Contains(cards->GetCard("promised land"));
    furnaceInSupply = supplyCards.Contains(cards->GetCard("furnace"));

    useGainList = supplyCards.Contains(cards->GetCard("smugglers")) || supplyCards.Contains(cards->GetCard("treasury"));
}

void GameData::InitBaseCards()
{
    baseCards.copper = cards->GetCard("copper");
    baseCards.silver = cards->GetCard("silver");
    baseCards.gold = cards->GetCard("gold");
    baseCards.platinum = cards->GetCard("platinum");
    baseCards.potion = cards->GetCard("potion");
    baseCards.estate = cards->GetCard("estate");
    baseCards.duchy = cards->GetCard("duchy");
    baseCards.province = cards->GetCard("province");
    baseCards.colony = cards->GetCard("colony");
    baseCards.curse = cards->GetCard("curse");

    baseCards.moat = cards->GetCard("moat");
    baseCards.lighthouse = cards->GetCard("lighthouse");
    baseCards.coppersmith = cards->GetCard("coppersmith");
    baseCards.bridge = cards->GetCard("bridge");
    baseCards.quarry = cards->GetCard("quarry");
    baseCards.talisman = cards->GetCard("talisman");
    baseCards.hoard = cards->GetCard("hoard");
    baseCards.goons = cards->GetCard("goons");

    baseCards.peddler = cards->GetCard("peddler");
    baseCards.bank = cards->GetCard("bank");
    baseCards.royalSeal = cards->GetCard("royal seal");
    baseCards.watchtower = cards->GetCard("watchtower");
    baseCards.treasury = cards->GetCard("treasury");
    baseCards.treasureMap = cards->GetCard("treasure map");
    baseCards.tradeRoute = cards->GetCard("trade route");
    baseCards.grandMarket = cards->GetCard("grand market");
    baseCards.mint = cards->GetCard("mint");

    baseCards.champion = cards->GetCard("champion");
    baseCards.gardener = cards->GetCard("gardener");
    baseCards.grandCourt = cards->GetCard("grand court");
    baseCards.plunder = cards->GetCard("plunder");
    baseCards.promisedLand = cards->GetCard("promised land");
    baseCards.trailblazer = cards->GetCard("trailblazer");

    baseCards.evangelist = cards->GetCard("evangelist");
    baseCards.furnace = cards->GetCard("furnace");
    baseCards.witchdoctor = cards->GetCard("witchdoctor");
}

void GameData::Init(const CardDatabase &_cards)
{
    cards = &_cards;
    InitBaseCards();
}
