
class CardKingsCourt : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventPlayActionNTimes(c, 3));
    }
};

class CardMountebank : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventMountebankAttack(c, p.index));
            }
        }
    }
};

class CardRabble : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventRabbleAttack(c, p.index));
            }
        }
    }
};

class CardGoons : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventDiscardDownToN(c, p.index, 3));
            }
        }
    }
};

class CardBishop : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players) s.stack.PushEnd(new EventBishop(c, p.index));
    }
};

class CardCity : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        int emptyPileCount = 0;
        for(UINT supplyIndex = 0; supplyIndex < s.data->supplyCards.Length(); supplyIndex++)
        {
            if(s.supply[supplyIndex].count == 0) emptyPileCount++;
        }
        if(emptyPileCount >= 1) s.stack.PushEnd(new EventDrawCard(s.player));
        if(emptyPileCount >= 2)
        {
            s.players[s.player].buys++;
            s.players[s.player].money++;
            if(logging) s.LogIndent(1, "gains $1");
        }
    }
};

class CardLoan : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventLoan(c));
    }
};

class CardVenture : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventVenture(c));
    }
};

class CardCountingHouse : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        
        int copperCount = 0;
        for(Card *c : p.discard) if(c == s.data->baseCards.copper) copperCount++;

        if(copperCount == 0)
        {
            if(logging) s.LogIndent(1, "has no coppers in their discard pile");
            return;
        }

        if(logging) s.LogIndent(1, "reveals " + String(copperCount) + " coppers");

        s.decision.MakeDiscreteChoice(c, copperCount + 1);
        if(decisionText)
        {
            s.decision.text = "Put how many coppers into your hand?";
            for(int copperIndex = 0; copperIndex <= copperCount; copperIndex++)
            {
                s.decision.text += "|" + String(copperIndex);
            }
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        if(logging) s.LogIndent(1, "puts " + String(response.choice) + " coppers into their hand");

        PlayerState &p = s.players[s.player];
        for(UINT copperIndex = 0; copperIndex < response.choice; copperIndex++)
        {
            p.discard.RemoveSlow(p.discard.FindFirstIndex(s.data->baseCards.copper));
            p.hand.PushEnd(s.data->baseCards.copper);
        }
    }
};

class CardWatchtower : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        int handDeficit = Math::Max(6 - (int)p.hand.Length(), 0);
        if(handDeficit == 0 && logging) s.LogIndent(1, "already has 6 cards in hand");
        for(int cardIndex = 0; cardIndex < handDeficit; cardIndex++) s.stack.PushEnd(new EventDrawCard(s.player));
    }
};

class CardExpand : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Select a card to trash:";
            s.decision.cardChoices = s.players[s.player].hand;
            s.stack.PushEnd(new EventRemodelExpand(c, 3));
        }
        else
        {
            if(logging) s.Log("has no cards to trash");
        }
    }
};

class CardTradeRoute : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        p.money += s.tradeRouteValue;
        if(logging) s.LogIndent(1, "gets $" + String(s.tradeRouteValue));
        s.stack.PushEnd(new EventTradeRoute(c));
    }
};

class CardMint : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventMint(c));
    }
};

class CardVault : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players) s.stack.PushEnd(new EventVault(c, p.index));
    }
};