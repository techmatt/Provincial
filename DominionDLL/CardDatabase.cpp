#include "Main.h"

void CardDatabase::Init(bool useCustomCards)
{
    _skipList = Utility::GetFileLines("data/skipList.txt");

    LoadCardList("data/core.txt", "core");
    LoadCardList("data/base.txt", "base");
    LoadCardList("data/intrigue.txt", "intrigue");
    LoadCardList("data/seaside.txt", "seaside");
    LoadCardList("data/alchemy.txt", "alchemy");
    LoadCardList("data/prosperity.txt", "prosperity");
    if(useCustomCards) LoadCardList("data/custom.txt", "custom");

    LoadPriorityList();
    LoadStrengthList();

    RegisterCardEffects(useCustomCards);
}

void CardDatabase::RegisterCard(const String &cardName, CardEffect *effect)
{
    Card *c = _cards[cardName];
    effect->c = c;
    c->effect = effect;
}

void CardDatabase::RegisterCardEffects(bool useCustomCards)
{
    //
    // Base cards
    //
    RegisterCard("chapel", new CardChapel);
    RegisterCard("cellar", new CardCellar);
    RegisterCard("moneylender", new CardMoneylender);
    RegisterCard("workshop", new CardWorkshop);
    RegisterCard("chancellor", new CardChancellor);
    RegisterCard("militia", new CardMilitia);
    RegisterCard("council room", new CardCouncilRoom);
    RegisterCard("feast", new CardFeast);
    RegisterCard("remodel", new CardRemodel);
    RegisterCard("library", new CardLibrary);
    RegisterCard("witch", new CardWitch);
    RegisterCard("mine", new CardMine);
    RegisterCard("gardens", new CardGardens);
    RegisterCard("adventurer", new CardAdventurer);
    RegisterCard("bureaucrat", new CardBureaucrat);
    RegisterCard("spy", new CardSpy);
    RegisterCard("throne room", new CardThroneRoom);

    //
    // Intrigue cards
    //
    RegisterCard("pawn", new CardPawn);
    RegisterCard("shanty town", new CardShantyTown);
    RegisterCard("courtyard", new CardCourtyard);
    RegisterCard("steward", new CardSteward);
    RegisterCard("wishing well", new CardWishingWell);
    RegisterCard("baron", new CardBaron);
    RegisterCard("nobles", new CardNobles);
    RegisterCard("trading post", new CardTradingPost);
    RegisterCard("scout", new CardScout);
    RegisterCard("duke", new CardDuke);
    RegisterCard("conspirator", new CardConspirator);
    RegisterCard("ironworks", new CardIronworks);
    RegisterCard("swindler", new CardSwindler);
    RegisterCard("minion", new CardMinion);
    RegisterCard("saboteur", new CardSaboteur);
    RegisterCard("torturer", new CardTorturer);
    RegisterCard("upgrade", new CardUpgrade);
    RegisterCard("mining village", new CardMiningVillage);
    RegisterCard("tribute", new CardTribute);

    //
    // Seaside cards
    //
    RegisterCard("caravan", new CardCaravan);
    RegisterCard("fishing village", new CardFishingVillage);
    RegisterCard("merchant ship", new CardMerchantShip);
    RegisterCard("wharf", new CardWharf);
    RegisterCard("tactician", new CardTactician);
    RegisterCard("lighthouse", new CardLighthouse);
    RegisterCard("warehouse", new CardWarehouse);
    RegisterCard("salvager", new CardSalvager);
    RegisterCard("lookout", new CardLookout);
    RegisterCard("pearl diver", new CardPearlDiver);
    RegisterCard("navigator", new CardNavigator);
    RegisterCard("ambassador", new CardAmbassador);
    RegisterCard("sea hag", new CardSeaHag);
    RegisterCard("cutpurse", new CardCutpurse);
    RegisterCard("ghost ship", new CardGhostShip);
    RegisterCard("treasure map", new CardTreasureMap);
    RegisterCard("smugglers", new CardSmugglers);
    RegisterCard("island", new CardIsland);
    RegisterCard("explorer", new CardExplorer);

    //
    // Prosperity cards
    //
    RegisterCard("king's court", new CardKingsCourt);
    RegisterCard("rabble", new CardRabble);
    RegisterCard("mountebank", new CardMountebank);
    RegisterCard("goons", new CardGoons);
    RegisterCard("city", new CardCity);
    RegisterCard("counting house", new CardCountingHouse);
    RegisterCard("loan", new CardLoan);
    RegisterCard("venture", new CardVenture);
    RegisterCard("bishop", new CardBishop);
    RegisterCard("watchtower", new CardWatchtower);
    RegisterCard("expand", new CardExpand);
    RegisterCard("trade route", new CardTradeRoute);
    RegisterCard("mint", new CardMint);
    RegisterCard("vault", new CardVault);

    //
    // Custom cards
    //
    if(useCustomCards)
    {
        RegisterCard("architect", new CardArchitect);
        RegisterCard("acolyte", new CardAcolyte);
        RegisterCard("aqueduct", new CardAqueduct);
        RegisterCard("gambler", new CardGambler);
        RegisterCard("meadow", new CardMeadow);
        RegisterCard("ruins", new CardRuins);
        RegisterCard("squire", new CardSquire);
        RegisterCard("haunted village", new CardHauntedVillage);
        RegisterCard("betrayers", new CardBetrayers);
        RegisterCard("meadow", new CardMeadow);
        RegisterCard("ruins", new CardRuins);
        RegisterCard("aristocrat", new CardAristocrat);
        RegisterCard("cursed land", new CardCursedLand);
        RegisterCard("promised land", new CardPromisedLand);
        RegisterCard("palace", new CardPalace);
        RegisterCard("pauper", new CardPauper);
        RegisterCard("floating city", new CardFloatingCity);
        RegisterCard("grand court", new CardGrandCourt);
        RegisterCard("knight", new CardKnight);
        RegisterCard("street urchin", new CardStreetUrchin);
        RegisterCard("sepulcher", new CardSepulcher);
        RegisterCard("benefactor", new CardBenefactor);
        RegisterCard("champion", new CardChampion);
        RegisterCard("hex", new CardHex);
        RegisterCard("trailblazer", new CardTrailblazer);
        RegisterCard("wager", new CardWager);
        RegisterCard("witchdoctor", new CardWitchdoctor);
        RegisterCard("pillage", new CardPillage);
        RegisterCard("furnace", new CardFurnace);
        RegisterCard("evangelist", new CardEvangelist);
    }
}

void CardDatabase::LoadPriorityList()
{
    Vector<String> lines = Utility::GetFileLines("data/priority.txt", 2);
    for(UINT lineIndex = 0; lineIndex < lines.Length(); lineIndex++)
    {
        const String &s = lines[lineIndex];
        if(!_skipList.Contains(s))
        {
            //PersistentAssert(_cards.find(s) != _cards.end(), "Card not in database");
            if(_cards.find(s) != _cards.end()) _cards[s]->priority = int(lines.Length()) - int(lineIndex);
        }
    }
}

void CardDatabase::LoadStrengthList()
{
    Vector<String> lines = Utility::GetFileLines("data/strongCards.txt", 2);
    for(UINT lineIndex = 0; lineIndex < lines.Length(); lineIndex++)
    {
        const String &s = lines[lineIndex];
        if(!_skipList.Contains(s))
        {
            //PersistentAssert(_cards.find(s) != _cards.end(), "Card not in database");
            if(_cards.find(s) != _cards.end()) _cards[s]->strength = int(lines.Length()) - int(lineIndex);
        }
    }
}

void CardDatabase::LoadCardList(const String &filename, const String &expansion)
{
    Vector<String> lines = Utility::GetFileLines(filename, 3);
    Vector<String> header = lines[0].Partition('\t');
    for(UINT lineIndex = 1; lineIndex < lines.Length(); lineIndex++)
    {
        Vector<String> words = lines[lineIndex].Partition('\t');

        if(!_skipList.Contains(words[header.FindFirstIndex("name")]))
        {
            Card *c = new Card;
            c->name = words[header.FindFirstIndex("name")];
            c->expansion = expansion;

            c->isAction    = words[header.FindFirstIndex("type")].Contains("action");
            c->isAttack    = words[header.FindFirstIndex("type")].Contains("attack");
            c->isReaction  = words[header.FindFirstIndex("type")].Contains("reaction");
            c->isTreasure  = words[header.FindFirstIndex("type")].Contains("treasure");
            c->isVictory   = words[header.FindFirstIndex("type")].Contains("victory");
            c->isDuration  = words[header.FindFirstIndex("type")].Contains("duration");
            c->isPermanent = words[header.FindFirstIndex("type")].Contains("permanent");

            c->cost          = words[header.FindFirstIndex("cost"    )].ConvertToInteger();
            c->supply        = words[header.FindFirstIndex("supply"  )].ConvertToInteger();
            c->actions       = words[header.FindFirstIndex("actions" )].ConvertToInteger();
            c->cards         = words[header.FindFirstIndex("cards"   )].ConvertToInteger();
            c->buys          = words[header.FindFirstIndex("buys"    )].ConvertToInteger();
            c->money         = words[header.FindFirstIndex("money"   )].ConvertToInteger();
            c->victoryTokens = words[header.FindFirstIndex("v tokens")].ConvertToInteger();
            c->victoryPoints = words[header.FindFirstIndex("v points")].ConvertToInteger();
            c->treasure      = words[header.FindFirstIndex("treasure")].ConvertToInteger();

            c->priority = -1;
            c->strength = -1;

            _cards[c->name] = c;
            _cardList.PushEnd(c);
        }
    }
}

Card* CardDatabase::GetCard(const String &s) const
{
    const auto it = _cards.find(s);
    if(it == _cards.end()) return NULL;
    else return it->second;
}

bool CardDatabase::IsSupplyCard(const Card *c) const
{
    return (c->expansion != "core");
}

Card* CardDatabase::RandomSupplyCard() const
{
    Card *result;
    do
    {
        result = _cardList.RandomElement();
    } while(!IsSupplyCard(result));
    return result;
}
