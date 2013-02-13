
class EventSteward : public Event
{
public:
    EventSteward(Card *_source)
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
        s.decision.MakeDiscreteChoice(source, 3);
        if(decisionText) s.decision.text = "Choose one:|+2 Cards|+2 Coins|Trash 2 cards";
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(response.choice == 0)
        {
            s.stack.PushEnd(new EventDrawCard(s.player));
            s.stack.PushEnd(new EventDrawCard(s.player));
        }
        else if(response.choice == 1)
        {
            if(logging) s.LogIndent(1, "gets $2");
            p.money += 2;
        }
        else if(response.choice == 2)
        {
            s.stack.PushEnd(new EventChooseCardsToTrash(source, 2, 2));
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventSwindlerAttack : public Event
{
public:
    EventSwindlerAttack(Card *_source, UINT _player)
    {
        source = _source;
        player = _player;
        done = false;
    }
    bool IsAttack() const
    {
        return true;
    }
    int AttackedPlayer() const
    {
        return player;
    }
    AttackAnnotations* Annotations()
    {
        return &annotations;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[player];
        if(p.deck.Length() == 0) s.Shuffle(player);

        if(p.deck.Length() == 0)
        {
            if(logging) s.LogIndent(1, player, "has no cards to trash");
            return true;
        }

        Card *c = p.deck.Last();
        if(logging) s.LogIndent(1, player, "reveals " + c->PrettyName());
        
        s.decision.GainCardFromSupply(s, source, c->cost, c->cost);
        if(s.decision.cardChoices.Length() == 0)
        {
            s.decision.type = DecisionNone;
            if(logging) s.LogIndent(1, player, "has no cards to gain");
            s.stack.PushEnd(new EventTrashCardFromDeck(player));
            return true;
        }

        if(decisionText) s.decision.text = "Choose a card for " + s.data->players[player].name + " to gain:";
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        s.stack.PushEnd(new EventGainCard(player, response.cards[0]));
        s.stack.PushEnd(new EventTrashCardFromDeck(player));
        done = true;
    }

    Card *source;
    UINT player;
    AttackAnnotations annotations;
    bool done;
};

class EventMinion : public Event
{
public:
    EventMinion(Card *_source)
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
        s.decision.MakeDiscreteChoice(source, 2);
        if(decisionText) s.decision.text = "Choose one:|+2 Coins|Discard hand, +4 Cards";
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(response.choice == 0)
        {
            s.players[s.player].money += 2;
            if(logging) s.LogIndent(1, "gets $2");
        }
        else if(response.choice == 1)
        {
            for(const PlayerInfo &p : s.data->players)
            {
                if(p.index == s.player || s.players[p.index].hand.Length() >= 5)
                {
                    for(int cardIndex = 0; cardIndex < 4; cardIndex++) s.stack.PushEnd(new EventDrawCard(p.index));
                    for(Card *c : s.players[p.index].hand) s.stack.PushEnd(new EventDiscardCard(p.index, c, DiscardFromHand));
                }
            }
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventTorturerAttack : public Event
{
public:
    EventTorturerAttack(Card *_source, UINT _player)
    {
        source = _source;
        player = _player;
        done = false;
    }
    bool IsAttack() const
    {
        return true;
    }
    int AttackedPlayer() const
    {
        return player;
    }
    AttackAnnotations* Annotations()
    {
        return &annotations;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        s.decision.MakeDiscreteChoice(source, 2);
        s.decision.controllingPlayer = player;
        if(decisionText) s.decision.text = "Choose one:|Discard 2 cards|Gain a curse in hand";

        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.choice == 0)
        {
            s.stack.PushEnd(new EventDiscardNCards(source, player, 2, 2));
        }
        else if(response.choice == 1)
        {
            s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.curse, false, false, GainToHand));
        }
        done = true;
    }

    Card *source;
    UINT player;
    AttackAnnotations annotations;
    bool done;
};

class EventSaboteurAttack : public Event
{
public:
    EventSaboteurAttack(Card *_source, UINT _player)
    {
        source = _source;
        player = _player;
    }
    bool IsAttack() const
    {
        return true;
    }
    int AttackedPlayer() const
    {
        return player;
    }
    AttackAnnotations* Annotations()
    {
        return &annotations;
    }
    bool Advance(State &s)
    {
        PlayerState &p = s.players[player];

        Vector<Card*> discardZone;
        bool deckExhausted = false, cardFound = false;
        while(!cardFound && !deckExhausted)
        {
            if(p.deck.Length() == 0) s.Shuffle(player);
            if(p.deck.Length() == 0) deckExhausted = true;
            else
            {
                Card *top = p.deck.Last();
                if(logging) s.LogIndent(1, player, "reveals " + top->PrettyName());
                int cost = s.SupplyCost(top);
                if(cost >= 3)
                {
                    cardFound = true;
                    s.stack.PushEnd(new EventChooseCardToGain(source, player, player, true, 0, cost - 2, FilterAny, GainToDiscard));
                    s.stack.PushEnd(new EventTrashCardFromDeck(player));
                }
                else
                {
                    discardZone.PushEnd(top);
                    p.deck.PopEnd();
                }
            }
        }

        for(Card *c : discardZone) s.stack.PushEnd(new EventDiscardCard(player, c, DiscardFromSideZone));

        if(deckExhausted)
        {
            if(logging) s.LogIndent(1, player, "has no cards with cost 3 or greater");
        }

        return true;
    }

    Card *source;
    UINT player;
    AttackAnnotations annotations;
};

class EventUpgrade : public Event
{
public:
    EventUpgrade(Card *_source)
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
        int cost = s.SupplyCost(trashedCard) + 1;
        s.decision.GainCardFromSupply(s, source, cost, cost);
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
            s.stack.PushEnd(new EventGainCard(s.player, response.cards[0]));
            done = true;
        }
    }

    Card *source;
    Card *trashedCard;
    bool done;
};