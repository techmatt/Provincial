
class EventLibrary : public Event
{
public:
    EventLibrary(Card *_source)
    {
        source = _source;
        doneDrawing = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        PlayerState &p = s.players[s.player];
        int currentHandSize = int(p.hand.Length());
        if(currentHandSize < 7 && !doneDrawing)
        {
            //
            // We can't actually call the card draw functions or events here, since we don't want an action to enter the hand
            // if the player sets it aside.
            //
            Card *revealedCard = NULL;
            if(p.deck.Length() == 0)
            {
                s.Shuffle(s.player);
            }
            if(p.deck.Length() == 0)
            {
                if(logging) s.Log("tries to draw, but has no cards left");
                doneDrawing = true;
            }
            else
            {
                revealedCard = p.deck.Last();
                p.deck.PopEnd();

                if(revealedCard->isAction)
                {
                    decisionCard = revealedCard;
                    s.decision.MakeDiscreteChoice(source, 2);
                    if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());
                    if(decisionText) s.decision.text = "Set aside " + decisionCard->PrettyName() + "?|Yes|No";
                }
                else
                {
                    if(logging) s.LogIndent(1, "draws " + revealedCard->PrettyName());
                    p.hand.PushEnd(revealedCard);
                }
                return false;
            }
        }
        for(Card *c : libraryZone)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
        }
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(response.choice == 0)
        {
            libraryZone.PushEnd(decisionCard);
            if(logging) s.Log("sets aside " + decisionCard->PrettyName());
        }
        else
        {
            p.hand.PushEnd(decisionCard);
            if(logging) s.Log("puts " + decisionCard->PrettyName() + " into their hand");
        }
    }


    Card *source, *decisionCard;
    bool doneDrawing;
    Vector<Card*> libraryZone;
};

class EventRemodelExpand : public Event
{
public:
    EventRemodelExpand(Card *_source, int _gainedValue)
    {
        source = _source;
        gainedValue = _gainedValue;
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
        s.decision.GainCardFromSupply(s, source, 0, s.SupplyCost(trashedCard) + gainedValue);
        if(s.decision.cardChoices.Length() == 0)
        {
            s.decision.type = DecisionNone;
            if(logging) s.LogIndent(1, "cannot gain any cards");
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
    int gainedValue;
    bool done;
};

class EventMine : public Event
{
public:
    EventMine(Card *_source)
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
        s.decision.GainTreasureFromSupply(s, source, 0, s.SupplyCost(trashedCard) + 3);
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

class EventAdventurer : public Event
{
public:
    EventAdventurer(Card *_source)
    {
        source = _source;
    }
    bool Advance(State &s)
    {
        PlayerState &p = s.players[s.player];
        Vector<Card*> zone;
        int treasuresLeft = 2;
        while(treasuresLeft > 0)
        {
            Card *revealedCard = NULL;
            if(p.deck.Length() == 0)
            {
                s.Shuffle(s.player);
            }
            if(p.deck.Length() == 0)
            {
                if(logging) s.Log("tries to draw, but has no cards left");
                treasuresLeft = 0;
            }
            else
            {
                revealedCard = p.deck.Last();
                p.deck.PopEnd();

                if(revealedCard->isTreasure)
                {
                    if(logging) s.LogIndent(1, "draws " + revealedCard->PrettyName());
                    p.hand.PushEnd(revealedCard);
                    treasuresLeft--;
                }
                else
                {
                    if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());
                    zone.PushEnd(revealedCard);
                }
            }
        }
        for(Card *c : zone)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
        }
        return true;
    }

    Card *source;
};

class EventBureaucratAttack : public Event
{
public:
    EventBureaucratAttack(Card *_source, UINT _player)
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
        const PlayerState &p = s.players[player];
        if(p.VictoryCount() == 0)
        {
            if(logging) s.Log(player, "reveals a hand with no victory cards");
        }
        else
        {
            s.decision.SelectCards(source, 1, 1);
            s.decision.controllingPlayer = player;
            
            for(Card *c : p.hand)
            {
                if(c->isVictory) s.decision.AddUniqueCard(c);
            }
            if(decisionText) s.decision.text = "Choose a victory card to put on top of your deck:";
        }
        return true;
    }

    Card *source;
    UINT player;
    AttackAnnotations annotations;
};

class EventMoatReveal : public Event
{
public:
    EventMoatReveal(Card *_source, UINT _player)
    {
        source = _source;
        player = _player;
        done = false;
        revealed = false;
    }
    bool DestroyNextEventOnStack() const
    {
        return revealed;
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
        if(decisionText) s.decision.text = "Reveal Moat?|Yes|No";
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.choice == 0)
        {
            if(logging) s.LogIndent(1, player, "reveals " + source->PrettyName());
            revealed = true;
        }
        else
        {
            if(logging) s.LogIndent(1, player, "does not reveal " + source->PrettyName());
        }
        done = true;
    }

    Card *source;
    UINT player;
    bool done;
    bool revealed;
};

class EventSpy : public Event
{
public:
    EventSpy(Card *_source, UINT _currentPlayer, UINT _targetedPlayer)
    {
        source = _source;
        currentPlayer = _currentPlayer;
        targetedPlayer = _targetedPlayer;
        done = false;
    }
    bool IsAttack() const
    {
        return (currentPlayer != targetedPlayer);
    }
    int AttackedPlayer() const
    {
        return targetedPlayer;
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
        const PlayerState &p = s.players[targetedPlayer];
        
        if(p.deck.Length() == 0)
        {
            s.Shuffle(targetedPlayer);
        }

        if(p.deck.Length() == 0)
        {
            if(logging) s.LogIndent(1, targetedPlayer, "has no cards to reveal");
            return true;
        }

        Card *c = p.deck.Last();
        if(logging) s.LogIndent(1, targetedPlayer, "reveals " + c->PrettyName());
        s.decision.MakeDiscreteChoice(source, 2);
        if(decisionText) s.decision.text = "Should " + s.data->players[targetedPlayer].name + " keep or discard" + c->PrettyName() + "?|Keep|Discard";
        s.decision.controllingPlayer = currentPlayer;
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[targetedPlayer];
        Card *c = p.deck.Last();
        if(response.choice == 0)
        {
            if(logging) s.LogIndent(1, targetedPlayer, "puts " + c->PrettyName() + " back on their deck");
        }
        else
        {
            p.deck.PopEnd();
            s.stack.PushEnd(new EventDiscardCard(targetedPlayer, c, DiscardFromSideZone));
        }
        done = true;
    }

    bool done;
    Card *source;
    UINT currentPlayer, targetedPlayer;
    AttackAnnotations annotations;
};