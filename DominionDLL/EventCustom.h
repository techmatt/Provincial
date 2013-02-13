class EventAqueduct : public Event
{
public:
    EventAqueduct(Card *_source)
    {
        source = _source;
        trashedCard = NULL;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;
        s.decision.GainVictoryFromSupply(s, source, 0, s.SupplyCost(trashedCard) + 3);
        if(s.decision.cardChoices.Length() == 0)
        {
            s.decision.type = DecisionNone;
            if(logging) s.Log("cannot gain any cards");
            return true;
        }
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(trashedCard == NULL)
        {
            trashedCard = response.cards[0];
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, trashedCard));
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, response.cards[0], false, false, GainToHand));
            done = true;
        }
    }

    Card *source;
    Card *trashedCard;
    bool done;
};

class EventKnight : public Event
{
public:
    EventKnight(Card *_source)
    {
        source = _source;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;
        s.decision.MakeDiscreteChoice(source, 6);
        if(decisionText) s.decision.text = "Choose one:|+1 Card, +1 Action|+1 Card, +1 Buy|+1 Card, +1 Coin|+1 Action, +1 Buy|+1 Action, +1 Coin|+1 Coin, +1 Buy";
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(response.choice == 0)
        {
            if(logging) s.LogIndent(1, "chooses to draw a card and gain an action");
            s.stack.PushEnd(new EventDrawCard(s.player));
            p.actions++;
        }
        else if(response.choice == 1)
        {
            if(logging) s.LogIndent(1, "chooses to draw a card and gain a buy");
            s.stack.PushEnd(new EventDrawCard(s.player));
            p.buys++;
        }
        else if(response.choice == 2)
        {
            if(logging) s.LogIndent(1, "chooses to draw a card and gain a coin");
            s.stack.PushEnd(new EventDrawCard(s.player));
            p.money++;
        }
        else if(response.choice == 3)
        {
            if(logging) s.LogIndent(1, "chooses to gain an action and a buy");
            p.actions++;
            p.buys++;
        }
        else if(response.choice == 4)
        {
            if(logging) s.LogIndent(1, "chooses to gain an action and a coin");
            p.actions++;
            p.money++;
        }
        else if(response.choice == 5)
        {
            if(logging) s.LogIndent(1, "chooses to gain a coin and a buy");
            p.money++;
            p.buys++;
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventSepulcher : public Event
{
public:
    EventSepulcher(Card *_source)
    {
        source = _source;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[s.player];
        
        if(p.VictoryCount() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
            return true;
        }

        s.decision.SelectCards(source, 1, 1);
        if(decisionText) s.decision.text = "Choose a card to trash:";

        for(Card *c : p.hand) if(c->isVictory) s.decision.AddUniqueCard(c);
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, c));
            int VPValue = s.SupplyCost(c);
            s.players[s.player].VPTokens += VPValue;
            if(logging) s.LogIndent(1, "gains " + String(VPValue) + " VP");
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventWager : public Event
{
public:
    EventWager(Card *_source)
    {
        source = _source;
    }

    bool Advance(State &s)
    {
        PlayerState &p = s.players[s.player];
        
        Card *revealedCard = NULL;
        if(p.deck.Length() == 0) s.Shuffle(s.player);
        if(p.deck.Length() == 0)
        {
            if(logging) s.Log("has no cards to reveal");
            return true;
        }

        Card *topCard = p.deck.Last();
        p.deck.PopEnd();
        if(logging) s.LogIndent(1, "reveals " + topCard->PrettyName());
        if(topCard->isTreasure)
        {
            if(logging) s.LogIndent(1, "plays " + topCard->PrettyName());
            p.playArea.PushEnd(CardPlayInfo(topCard, 0));
            s.ProcessTreasure(topCard);
        }
        else
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, topCard, DiscardFromSideZone));
        }

        return true;
    }

    Card *source;
};