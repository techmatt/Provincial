class EventMountebankAttack : public Event
{
public:
    EventMountebankAttack(Card *_source, UINT _player)
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
        if(p.hand.Contains(s.data->baseCards.curse))
        {
            s.decision.MakeDiscreteChoice(source, 2);
            s.decision.controllingPlayer = player;
            if(decisionText) s.decision.text = "Discard a curse?|Yes|No";
            return false;
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.curse));
            s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.copper));
            return true;
        }
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.choice == 0)
        {
            s.stack.PushEnd(new EventDiscardCard(player, s.data->baseCards.curse, DiscardFromHand));
        }
        else if(response.choice == 1)
        {
            s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.curse));
            s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.curse));
        }
        done = true;
    }

    Card *source;
    UINT player;
    AttackAnnotations annotations;
    bool done;
};

class EventRabbleAttack : public Event
{
public:
    EventRabbleAttack(Card *_source, UINT _player)
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
        if(p.deck.Length() <= 3) s.Shuffle(player);

        int cardsToReveal = Math::Min(3, (int)p.deck.Length());
        if(cardsToReveal == 0)
        {
            if(logging) s.LogIndent(1, player, "has no cards to reveal");
            return true;
        }

        Vector<Card*> discardCards;
        Vector<Card*> topDeckCards;
        for(int cardIndex = 0; cardIndex < cardsToReveal; cardIndex++)
        {
            Card *c = p.deck.Last();
            if(logging) s.LogIndent(1, player, "reveals " + c->PrettyName());
            p.deck.PopEnd();
            if(c->isAction || c->isTreasure) discardCards.PushEnd(c);
            else topDeckCards.PushEnd(c);
        }

        for(Card *c : discardCards) s.stack.PushEnd(new EventDiscardCard(player, c, DiscardFromSideZone));

        for(Card *c : topDeckCards) p.deck.PushEnd(c);

        if(topDeckCards.Length() >= 2) s.stack.PushEnd(new EventReorderDeck(source, player, topDeckCards.Length()));

        return true;
    }
    Card *source;
    UINT player;
    AttackAnnotations annotations;
};

class EventLoan : public Event
{
public:
    EventLoan(Card *_source)
    {
        source = _source;
        treasureCard = NULL;
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
        
        while(treasureCard == NULL)
        {
            Card *revealedCard = NULL;
            if(p.deck.Length() == 0) s.Shuffle(s.player);
            if(p.deck.Length() == 0)
            {
                if(logging) s.Log("tries to draw, but has no cards left");
                for(Card *c : discardZone) s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
                return true;
            }
            else
            {
                revealedCard = p.deck.Last();
                p.deck.PopEnd();
                if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());

                if(revealedCard->isTreasure)
                {
                    treasureCard = revealedCard;
                    s.decision.MakeDiscreteChoice(source, 2);
                    if(decisionText) s.decision.text = "Discard or trash" + revealedCard->PrettyName() + "?|Discard|Trash";
                }
                else
                {
                    discardZone.PushEnd(revealedCard);
                }
            }
        }
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.choice == 0)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, treasureCard, DiscardFromSideZone));
        }
        else if(response.choice == 1)
        {
            s.stack.PushEnd(new EventTrashCardFromSideZone(s.player, treasureCard));
        }
        for(Card *c : discardZone) s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
        done = true;
    }

    Vector<Card*> discardZone;
    Card *source;
    Card *treasureCard;
    bool done;
};

class EventVenture : public Event
{
public:
    EventVenture(Card *_source)
    {
        source = _source;
    }

    bool Advance(State &s)
    {
        PlayerState &p = s.players[s.player];
        Vector<Card*> discardZone;

        bool deckExhausted = false;
        Card *treasureCard = NULL;
        while(treasureCard == NULL && !deckExhausted)
        {
            Card *revealedCard = NULL;
            if(p.deck.Length() == 0) s.Shuffle(s.player);
            if(p.deck.Length() == 0)
            {
                if(logging) s.Log("tries to draw, but has no cards left");
                deckExhausted = true;
            }
            else
            {
                revealedCard = p.deck.Last();
                p.deck.PopEnd();
                if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());

                if(revealedCard->isTreasure)
                {
                    treasureCard = revealedCard;
                    if(logging) s.LogIndent(1, "plays " + treasureCard->PrettyName());
                    p.playArea.PushEnd(CardPlayInfo(treasureCard, 0));
                    s.ProcessTreasure(treasureCard);
                }
                else
                {
                    discardZone.PushEnd(revealedCard);
                }
            }
        }
        for(Card *c : discardZone) s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
        return true;
    }

    Card *source;
};

class EventBishop : public Event
{
public:
    EventBishop(Card *_source, int _player)
    {
        source = _source;
        player = _player;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[player];
        
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, player, "has no cards to trash");
            return true;
        }

        if(player == s.player)
        {
            s.decision.SelectCards(source, 1, 1);
            if(decisionText) s.decision.text = "Choose a card to trash:";
        }
        else
        {
            s.decision.SelectCards(source, 0, 1);
            if(decisionText) s.decision.text = "You may trash a card:";
        }
        s.decision.AddUniqueCards(p.hand);
        s.decision.controllingPlayer = player;
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventTrashCardFromHand(player, c));
            if(player == s.player)
            {
                int VPValue = s.SupplyCost(c) / 2;
                s.players[player].VPTokens += VPValue;
                if(logging) s.LogIndent(1, "gains " + String(VPValue) + " VP");
            }
        }
        done = true;
    }

    int player;
    Card *source;
    bool done;
};

class EventTradeRoute : public Event
{
public:
    EventTradeRoute(Card *_source)
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
        
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
            return true;
        }

        s.decision.SelectCards(source, 1, 1);
        if(decisionText) s.decision.text = "Choose a card to trash:";
        s.decision.AddUniqueCards(p.hand);
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        s.stack.PushEnd(new EventTrashCardFromHand(s.player, response.cards[0]));
        done = true;
    }

    Card *source;
    bool done;
};

class EventMint : public Event
{
public:
    EventMint(Card *_source)
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
        
        if(p.TreasureCount() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to copy");
            return true;
        }

        s.decision.SelectCards(source, 0, 1);
        if(decisionText) s.decision.text = "Gain a copy of which treasure?";
        for(Card *c : p.hand) if(c->isTreasure) s.decision.AddUniqueCard(c);
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.cards.Length() == 0)
        {
            if(logging) s.LogIndent(1, "does not gain a treasure");
        }
        else
        {
            if(logging) s.LogIndent(1, "reveals " + response.cards[0]->PrettyName());
            s.stack.PushEnd(new EventGainCard(s.player, response.cards[0]));
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventVault : public Event
{
public:
    EventVault(Card *_source, int _player)
    {
        source = _source;
        player = _player;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[player];
        
        if(player == s.player)
        {
            if(p.hand.Length() == 0)
            {
                if(logging) s.LogIndent(1, player, "has no cards to discard");
                return true;
            }

            s.decision.SelectCards(source, 0, s.players[player].hand.Length());
            if(decisionText) s.decision.text = "Select cards to discard:";
        }
        else
        {
            if(p.hand.Length() < 2)
            {
                if(logging) s.LogIndent(1, player, "does not have enough cards to discard");
                return true;
            }

            s.decision.SelectCards(source, 0, 2);
            if(decisionText) s.decision.text = "Discard 2 cards to draw a card:";
        }
        s.decision.cardChoices = s.players[player].hand;
        s.decision.controllingPlayer = player;
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[player];
        if(player == s.player)
        {
            p.money += response.cards.Length();
            if(logging) s.LogIndent(1, "gets $" + String(response.cards.Length()));
            for(Card *c : response.cards)
            {
                s.stack.PushEnd(new EventDiscardCard(player, c, DiscardFromHand));
            }
        }
        else
        {
            if(response.cards.Length() <= 1)
            {
                if(logging) s.LogIndent(1, player, "does not discard to " + source->PrettyName());
            }
            else
            {
                s.stack.PushEnd(new EventDrawCard(player));
                for(Card *c : response.cards)
                {
                    s.stack.PushEnd(new EventDiscardCard(player, c, DiscardFromHand));
                }
            }
        }
        done = true;
    }

    int player;
    Card *source;
    bool done;
};
