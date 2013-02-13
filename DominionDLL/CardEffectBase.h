class CardChapel : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 0, 4);
            if(decisionText) s.decision.text = "Select up to 4 cards to trash:";
            s.decision.cardChoices = s.players[s.player].hand;
        }
        else
        {
            if(logging) s.Log("has no cards to trash");
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
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, c));
        }
    }
};

class CardCellar : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 0, s.players[s.player].hand.Length());
            if(decisionText) s.decision.text = "Select cards to discard:";
            s.decision.cardChoices = s.players[s.player].hand;
        }
        else
        {
            if(logging) s.Log("has no cards to discard");
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
            s.stack.PushEnd(new EventDrawCard(s.player));
        }
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromHand));
        }
    }
};

class CardMoneylender : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.FindFirstIndex(s.data->baseCards.copper) != -1)
        {
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, s.data->baseCards.copper));
            s.players[s.player].money += 3;
        }
        else
        {
            if(logging) s.Log("has no coppers to trash");
        }
    }
};

class CardWorkshop : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.GainCardFromSupply(s, c, 0, 4);
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

class CardFeast : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.GainCardFromSupply(s, c, 0, 5);
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
        s.stack.PushEnd(new EventTrashCardFromPlay(s.player, c));
        s.stack.PushEnd(new EventGainCard(s.player, response.cards[0]));
    }
};

class CardChancellor : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.MakeDiscreteChoice(c, 2);
        if(decisionText) s.decision.text = "Put your deck into your discard pile?|Yes|No";
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        if(response.choice == 0)
        {
            if(logging) s.Log("discards their deck");
            PlayerState &p = s.players[s.player];
            p.discard.Append(p.deck);
            p.deck.FreeMemory();
        }
        else
        {
            if(logging) s.Log("does not discard their deck");
        }
    }
};

class CardMilitia : public CardEffect
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

class CardCouncilRoom : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventDrawCard(p.index));
            }
        }
    }
};

class CardRemodel : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Select a card to trash:";
            s.decision.cardChoices = s.players[s.player].hand;
            s.stack.PushEnd(new EventRemodelExpand(c, 2));
        }
        else
        {
            if(logging) s.Log("has no cards to trash");
        }
    }
};

class CardMine : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].TreasureCount() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Select a treasure to trash:";
            for(Card *c : s.players[s.player].hand)
            {
                if(c->isTreasure) s.decision.AddUniqueCard(c);
            }
            s.stack.PushEnd(new EventMine(c));
        }
        else
        {
            if(logging) s.Log("has no treasures to trash");
        }
    }
};

class CardLibrary : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() < 7)
        {
            s.stack.PushEnd(new EventLibrary(c));
        }
        else
        {
            if(logging) s.Log("already has 7 cards in hand");
        }
    }
};

class CardAdventurer : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventAdventurer(c));
    }
};

class CardWitch : public CardEffect
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

class CardGardens : public CardEffect
{
public:
    int VictoryPoints(const State &s, UINT playerIndex) const
    {
        return s.players[playerIndex].TotalCards() / 10;
    }
};

class CardBureaucrat : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.silver, false, false, GainToDeckTop));
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventBureaucratAttack(c, p.index));
            }
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        Card *c = response.cards[0];
        PlayerState &p = s.players[s.decision.controllingPlayer];
        p.hand.RemoveSwap(p.hand.FindFirstIndex(c));
        p.deck.PushEnd(c);
        if(logging) s.LogIndent(1, s.decision.controllingPlayer, "puts " + c->PrettyName() + " on their deck");
    }
};

class CardSpy : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            s.stack.PushEnd(new EventSpy(c, s.player, p.index));
        }
    }
};

class CardThroneRoom : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventPlayActionNTimes(c, 2));
    }
};
