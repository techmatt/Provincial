#include "Main.h"

void BuyMenu::RecordItem(Card *c)
{
    for(BuyMenuEntry &e : entries)
    {
        if(e.c == c && e.count > 0)
        {
            e.count--;
            return;
        }
    }
}

void BuyMenu::Cleanup()
{
    for(int i = 0; i < int(entries.Length()); i++)
    {
        if(entries.Length() > 1 && entries[i].count <= 0)
        {
            entries.RemoveSlow(i);
            i--;
        }
    }
}

void BuyMenu::Merge()
{
    for(int i = 0; i < int(entries.Length()) - 1; i++)
    {
        if(entries[i].c == entries[i+1].c)
        {
            entries[i].count = Math::Min(entries[i].count + entries[i+1].count, 99);
            entries.RemoveSlow(i+1);
            i--;
        }
    }
    Bound();
}

void BuyMenu::Bound()
{
    for(BuyMenuEntry &e : entries)
    {
        if(e.count != 99)
        {
            if(e.c->isVictory) e.count = Math::Min(8, e.count);
            else e.count = Math::Min(e.c->supply, e.count);
        }
    }
}

Card* BuyAgendaBigMoney::Buy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    const BaseCards &base = s.data->baseCards;
    if(choices.Contains(base.colony)) return base.colony;
    if(choices.Contains(base.platinum)) return base.platinum;
    if(choices.Contains(base.province)) return base.province;
    if(choices.Contains(base.duchy) && s.SupplyCount(base.province) <= 4) return base.duchy;
    if(choices.Contains(base.estate) && s.SupplyCount(base.province) <= 2) return base.estate;
    if(choices.Contains(base.gold)) return base.gold;
    if(choices.Contains(base.silver)) return base.silver;
    return NULL;
}

Card* BuyAgendaBigMoney::ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    Card *result = Buy(s, player, choices);
    if(result != NULL) return result;
    
    auto scoringFunction = [](Card *c)
    {
        double score = c->cost;
        if(c->isVictory) score -= 0.5;
        return score;
    };
    return AIUtility::BestCard(choices, scoringFunction, -100.0);
}

BuyAgendaMenu::BuyAgendaMenu(BuyMenu &m)
{
    _m = m;
}

BuyAgendaMenu::BuyAgendaMenu(const CardDatabase &cards, const GameOptions &options, Card *a, Card *b)
{
    InitMenu(cards, options);
}

BuyAgendaMenu::BuyAgendaMenu(const CardDatabase &cards, const String &s)
{
    //e4-d5-p8-gold@99|festival@10|ironworks@1|warehouse@1|silver@99
    Vector<String> p = s.Partition("-");
    _m.estateBuyThreshold = p[0].FindAndReplace("e","").ConvertToInteger();
    _m.duchyBuyThreshold = p[1].FindAndReplace("d","").ConvertToInteger();
    _m.provinceBuyThreshold = p[2].FindAndReplace("p","").ConvertToInteger();

    Vector<String> c = p[3].Partition("|");
    for(String &part : c)
    {
        Card *card = cards.GetCard(part.Partition("@")[0]);
        if(card == NULL) card = cards.GetCard("colony");

        _m.entries.PushEnd(BuyMenuEntry(card, part.Partition("@")[1].ConvertToInteger(), 0, 11));
    }
}

BuyAgendaMenu::BuyAgendaMenu(const CardDatabase &cards, const GameOptions &options)
{
    InitMenu(cards, options);
}

void BuyAgendaMenu::InitMenu(const CardDatabase &cards, const GameOptions &options)
{
    const bool bareboneMenuEvolution = false;
    if(bareboneMenuEvolution)
    {
        _m.provinceBuyThreshold = 4;
        _m.duchyBuyThreshold = 3;
        _m.estateBuyThreshold = 2;

        for(int entryIndex = 0; entryIndex < 13; entryIndex++)
            _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 11));
    }
    else if(options.prosperity)
    {
        _m.provinceBuyThreshold = 4;
        _m.duchyBuyThreshold = 3;
        _m.estateBuyThreshold = 2;

        //_m.entries.PushEnd(BuyMenuEntry(cards.GetCard("platinum"), 2, 9, 9));
        //_m.entries.PushEnd(BuyMenuEntry(cards.GetCard("colony"), 8, 11, 11));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("platinum"), 0, 7, 11));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("platinum"), 99, 9, 9));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 0, 5, 8));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 0, 5, 8));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 0, 5, 8));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 99, 6, 6));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 3, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 3, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("silver"), 99, 3, 3));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 2));
    }
    else
    {
        _m.provinceBuyThreshold = 8;
        _m.duchyBuyThreshold = 4;
        _m.estateBuyThreshold = 2;
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 0, 5, 11));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 0, 5, 11));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 99, 6, 6));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 3, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 3, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 5));
        _m.entries.PushEnd(BuyMenuEntry(cards.GetCard("silver"), 99, 3, 3));
        _m.entries.PushEnd(BuyMenuEntry(options.supplyPiles.RandomElement(), rand() % 2, 0, 2));
    }
}

BuyAgenda* BuyAgendaMenu::Mutate(const CardDatabase &cards, const GameOptions &options) const
{
    PersistentAssert(_m.entries.Length() > 0, "Empty menu");
    BuyMenu m = _m;
    
    if(rnd() <= 0.1) m.estateBuyThreshold = Utility::Bound(m.estateBuyThreshold + AIUtility::Delta(), 0, 8);
    if(rnd() <= 0.1) m.duchyBuyThreshold =  Utility::Bound(m.duchyBuyThreshold + AIUtility::Delta(), 0, 8);
    if(options.prosperity && rnd() <= 0.1) m.provinceBuyThreshold =  Utility::Bound(m.provinceBuyThreshold + AIUtility::Delta(), 0, 8);
    
    if(rnd() <= 0.7)
    {
        //
        // Mutate existing entry
        //
        BuyMenuEntry &curEntry = m.entries.RandomElement();
        if(curEntry.count != 99)
        {
            double r = rnd();
            if(r <= 0.2)
            {
                curEntry.count = 0;
            }
            else if(r <= 0.7)
            {
                curEntry.count = Utility::Bound(curEntry.count + AIUtility::Delta(2), 0, 12);
            }
            else
            {
                curEntry.count = Utility::Bound(curEntry.count + AIUtility::Delta(12), 0, 12);
            }
        }
    }

    for(UINT mutationIndex = 0; mutationIndex < 2; mutationIndex++)
    {
        if(rnd() <= 0.6)
        {
            BuyMenuEntry &curEntry = m.entries.RandomElement();
            if(curEntry.count != 99)
            {
                Card *newCard = options.RandomSupplyCard(cards);
                for(int retryIndex = 0; retryIndex < 20; retryIndex++) if(!curEntry.CardValidInSlot(newCard)) newCard = options.RandomSupplyCard(cards);

                if(curEntry.CardValidInSlot(newCard) && newCard->name != "curse" && newCard->name != "copper" && newCard->name != "estate" && newCard->name != "duchy" && newCard->name != "province" && newCard->name != "colony")
                {
                    curEntry.c = newCard;
                    if(rnd() <= 0.5) curEntry.count = 1;
                }
            }
        }
    }

    if(rnd() <= 0.5)
    {
        BuyMenuEntry &entry1 = m.entries.RandomElement();
        BuyMenuEntry &entry2 = m.entries.RandomElement();
        if(entry1.minCost == entry2.minCost && entry1.maxCost == entry2.maxCost && entry1.count != 99 && entry2.count != 99) Utility::Swap(entry1, entry2);
    }

    return new BuyAgendaMenu(m);
}

Card* BuyAgendaMenu::Buy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    const BaseCards &base = s.data->baseCards;
    const PlayerState &p = s.players[player];

    //
    // If we are at the "end game", aggressively buy provinces, duchies, and estates.
    //
    if(s.data->options.prosperity)
    {
        if(choices.Contains(base.colony)) return base.colony;
        int victoryStackSize = Math::Min(s.SupplyCount(base.province), s.SupplyCount(base.colony));
        if(choices.Contains(base.province) && victoryStackSize <= _m.provinceBuyThreshold)  return base.province;
        if(choices.Contains(base.duchy) && victoryStackSize <= _m.duchyBuyThreshold) return base.duchy;
        if(choices.Contains(base.estate) && victoryStackSize <= _m.estateBuyThreshold) return base.estate;
    }
    else
    {
        if(choices.Contains(base.province)) return base.province;
        if(choices.Contains(base.duchy) && int(s.SupplyCount(base.province)) <= _m.duchyBuyThreshold) return base.duchy;
        if(choices.Contains(base.estate) && int(s.SupplyCount(base.province)) <= _m.estateBuyThreshold) return base.estate;
    }

    BuyMenu menuCopy = _m;

    CardCounter counts(p);
    //map<Card*,int> counts = p.CardCounts();
    
    for(auto &c : counts.counts)
    {
        for(int i = 0; i < c.second; i++)
        {
            menuCopy.RecordItem(c.first);
        }
    }

    for(auto &e : menuCopy.entries)
    {
        if(e.count > 0 && choices.Contains(e.c))
        {
            return e.c;
        }
    }

    return NULL;
}

Card* BuyAgendaMenu::ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    Card *result = Buy(s, player, choices);
    if(result != NULL) return result;
    
    auto scoringFunction = [](Card *c)
    {
        double score = c->cost;
        if(c->isVictory) score -= 0.5;
        return score;
    };
    return AIUtility::BestCard(choices, scoringFunction, -100.0);
}

Card* BuyAgendaExpensiveNovelties::Buy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    const BaseCards &base = s.data->baseCards;
    const PlayerState &p = s.players[player];

    //
    // If we are at the "end game", aggressively buy duchys and estates.
    //
    if(choices.Contains(base.province)) return base.province;
    if(choices.Contains(base.duchy) && s.SupplyCount(base.province) <= 3) return base.duchy;
    if(choices.Contains(base.estate) && s.SupplyCount(base.province) <= 1) return base.estate;

    CardCounter counts(p);
    //map<Card*,int> counts = p.CardCounts();
    
    auto scoringFunction = [&counts](Card *c)
    {
        double score = c->cost;
        if(c->name == "feast") score = 0.0;
        if(c->name == "thief") score = 0.0; //NYI
        if(!c->isTreasure) score -= 0.01 * counts.Count(c);
        score += c->actions * 0.01;
        if(c->isVictory) score -= 0.5;
        return score;
    };
    return AIUtility::BestCard(choices, scoringFunction, 2.0);
}

Card* BuyAgendaExpensiveNovelties::ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const
{
    Card *result = Buy(s, player, choices);
    if(result != NULL) return result;
    
    auto scoringFunction = [](Card *c)
    {
        double score = c->cost;
        if(c->isVictory) score -= 0.5;
        return score;
    };
    return AIUtility::BestCard(choices, scoringFunction, -100.0);
}
