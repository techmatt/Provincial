
class CardGrandCourt : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventPlayActionNTimes(c, 4));
    }
};

class CardGambler : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(logging) s.Log("gets $3 from " + c->PrettyName());
        p.money += 3;
        p.buys++;

        int cardsToTrash = Math::Min(2, (int)p.hand.Length());
        if(cardsToTrash == 0)
        {
            if(logging) s.Log("has no cards to trash to " + c->PrettyName());
        }
        else
        {
            s.stack.PushEnd(new EventChooseCardsToTrash(c, cardsToTrash, cardsToTrash));
        }
    }
};

class CardFurnace : public CardEffect
{
public:
    //
    // Furnace is handled explicitly in State.cpp, since it's "start of buy phase" effect is (thusfar) unique.
    //
};

class CardEvangelist : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        s.stack.PushEnd(new EventChooseCardsToTrash(c, 0, 1));
    }
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventChooseCardsToTrash(c, 0, 1));
    }
};

class CardPromisedLand : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(logging) s.Log("gets 1 VP from " + c->PrettyName());
        p.VPTokens++;
    }
};

class CardChampion : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(logging) s.Log("gets $1 from " + c->PrettyName());
        p.money++;
        p.actions++;
    }
};

class CardCursedLand : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        s.stack.PushEnd(new EventDrawCard(s.player));
        p.VPTokens--;
        if(logging) s.Log("loses 1 VP from " + c->PrettyName());
    }
};

class CardPalace : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(logging) s.Log("gets $1 from " + c->PrettyName());
        p.money++;
    }
};

class CardFloatingCity : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(logging) s.Log("gets an extra buy from " + c->PrettyName());
        p.buys++;
    }
};

class CardPauper : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        PlayerState &p = s.players[s.player];
        s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.copper, false, false, GainToHand));
    }
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.copper, false, false, GainToHand));
    }
};

class CardAcolyte : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        int handDeficit = Math::Max(5 - (int)p.hand.Length(), 0);
        if(handDeficit == 0 && logging) s.LogIndent(1, "already has 5 cards in hand");
        for(int cardIndex = 0; cardIndex < handDeficit; cardIndex++) s.stack.PushEnd(new EventDrawCard(s.player));
    }
};

class CardAqueduct : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].VictoryCount() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Select a victory card to trash:";
            for(Card *c : s.players[s.player].hand)
            {
                if(c->isVictory) s.decision.AddUniqueCard(c);
            }
            s.stack.PushEnd(new EventAqueduct(c));
        }
        else
        {
            if(logging) s.Log("has no victory cards to trash");
        }
    }
};

class CardArchitect : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        int cardsToPutBack = Math::Min(3, (int)s.players[s.player].hand.Length());
        if(cardsToPutBack > 0)
        {
            s.decision.SelectCards(c, cardsToPutBack, cardsToPutBack);
            if(decisionText) s.decision.text = "Choose " + String(cardsToPutBack) + " cards to put on top of your deck:";
            s.decision.cardChoices = s.players[s.player].hand;
        }
        else
        {
            if(logging) s.LogIndent(1, "has no cards in hand");
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        for(Card *c : response.cards)
        {
            PlayerState &p = s.players[s.player];
            p.hand.RemoveSwap(p.hand.FindFirstIndex(c));
            p.deck.PushEnd(c);
            if(logging) s.LogIndent(1, s.decision.controllingPlayer, "puts " + c->PrettyName() + " on their deck");
        }
    }
};

class CardSquire : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
        }
        else
        {
            s.stack.PushEnd(new EventChooseCardsToTrash(c, 1, 1));
        }
    }
};

class CardHauntedVillage : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventGainCard(p.index, s.data->baseCards.curse, false, true, GainToDiscard));
            }
        }
    }
};

class CardBetrayers : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.curse, false, false, GainToDiscard));
    }
};

class CardMeadow : public CardEffect
{
public:
    int VictoryPoints(const State &s, UINT playerIndex) const
    {
        CardCounter counter(s.players[playerIndex]);
        return counter.Count(s.data->baseCards.estate) / 2;
    }
};

class CardRuins : public CardEffect
{
public:
    int VictoryPoints(const State &s, UINT playerIndex) const
    {
        return s.SupplyCount(c);
    }
};

class CardAristocrat : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(p.deck.Length() < 5)
        {
            s.Shuffle(s.player);
        }

        int cardsToReveal = Math::Min((int)p.deck.Length(), 5);
        int cardsToReorder = 0;

        if(cardsToReveal == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to reveal");
            return;
        }

        Vector<Card*> deckTop;
        for(int cardIndex = 0; cardIndex < cardsToReveal; cardIndex++)
        {
            Card *c = p.deck.Last();
            p.deck.PopEnd();
            if(c->isVictory)
            {
                if(logging)
                {
                    s.LogIndent(1, "reveals " + c->PrettyName() + ", discarding it");
                    s.LogIndent(1, "gets $1");
                }
                p.money++;
                s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
            }
            else
            {
                if(logging) s.LogIndent(1, "reveals " + c->PrettyName());
                deckTop.PushEnd(c);
            }
        }

        for(UINT deckIndex = 0; deckIndex < deckTop.Length(); deckIndex++)
        {
            p.deck.PushEnd(deckTop[deckIndex]);
        }

        for(UINT reorderIndex = 2; reorderIndex <= deckTop.Length(); reorderIndex++)
        {
            s.stack.PushEnd(new EventReorderDeck(c, s.player, reorderIndex));
        }
    }
};

class CardKnight : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventKnight(c));
    }
    void ProcessDuration(State &s) const
    {
        s.stack.PushEnd(new EventKnight(c));
    }
};

class CardStreetUrchin : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.SelectCards(c, 1, 1);
        if(decisionText) s.decision.text = "Select a card to gain:";
        for(UINT supplyIndex = 0; supplyIndex < s.data->supplyCards.Length(); supplyIndex++)
        {
            int cost = s.SupplyCost(supplyIndex);
            int count = s.supply[supplyIndex].count;
            if(count > 0 && count <= 5 && !s.data->supplyCards[supplyIndex]->isVictory)
            {
                s.decision.AddUniqueCard(s.data->supplyCards[supplyIndex]);
            }
        }
        if(s.decision.cardChoices.Length() == 0)
        {
            s.decision.type = DecisionNone;
            if(logging) s.Log("has no cards to gain");
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        s.data->players[s.decision.controllingPlayer].ledger.RecordBuy(response.cards[0]);
        s.stack.PushEnd(new EventGainCard(s.player, response.cards[0]));
    }
};

class CardSepulcher : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventSepulcher(c));
    }
};

class CardBenefactor : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(logging) for(Card *c : p.hand) s.LogIndent(1, "reveals " + c->PrettyName());

        if(p.ActionCount() == 0)
        {
            if(logging) s.LogIndent(1, "gets $2");
            p.money += 2;
        }

        if(p.VictoryCount() == 0)
        {
            s.stack.PushEnd(new EventDrawCard(s.player));
            s.stack.PushEnd(new EventDrawCard(s.player));
        }

        if(p.TreasureCount() == 0)
        {
            if(logging) s.LogIndent(1, "gains 2 VP");
            p.VPTokens += 2;
        }
    }
};

class CardHex : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.SupplyCount(c) > 0)
        {
            s.stack.PushEnd(new EventGainCard(s.player, c));
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.curse));
            
            Vector<CardPlayInfo> &playArea = s.players[s.player].playArea;
            for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
            {
                CardPlayInfo &playCard = playArea[playIndex];
                if(playCard.card == c)
                {
                    playArea.RemoveSwap(playIndex);
                    s.supply[s.data->SupplyIndex(c)].count++;
                    if(logging) s.LogIndent(1, "returns " + c->PrettyName() + " to the supply");
                    return;
                }
            }
        }
    }
};

class CardWager : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventWager(c));
    }
};

class CardPillage : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        
        if(p.deck.Length() == 0) s.Shuffle(s.player);
        if(p.deck.Length() == 0)
        {
            if(logging) s.Log("has no cards to reveal");
            return;
        }

        Card *topCard = p.deck.Last();
        p.deck.PopEnd();

        if(logging) s.LogIndent(1, "reveals " + topCard->PrettyName());
        s.stack.PushEnd(new EventChooseCardToGain(c, s.player, s.player, false, 0, s.SupplyCost(topCard) + 2, FilterTreasure, GainToHand));
    }
};

class CardTrailblazer : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventPlayActionNTimes(c, 3));
    }
};

class CardWitchdoctor : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventPlayActionNTimes(c, 2));
    }
};