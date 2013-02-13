class EventNavigator : public Event
{
public:
    EventNavigator(Card *_source)
    {
        source = _source;
        done = false;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[s.player];

        if(p.deck.Length() < 5) s.Shuffle(s.player);
        int cardsToReveal = Math::Min((int)p.deck.Length(), 5);
        if(cardsToReveal == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to reveal");
            return true;
        }

        for(int cardIndex = 0; cardIndex < cardsToReveal; cardIndex++)
        {
            Card *c = p.deck[p.deck.Length() - 1 - cardIndex];
            if(logging) s.LogIndent(1, "reveals " + c->PrettyName());
        }

        s.decision.MakeDiscreteChoice(source, 2);
        if(decisionText) s.decision.text = "Discard revealed cards?|Yes|No";

        return false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        int cardsToReveal = Math::Min((int)p.deck.Length(), 5);
        if(response.choice == 0)
        {
            for(int cardIndex = 0; cardIndex < cardsToReveal; cardIndex++)
            {
                Card *c = p.deck.Last();
                p.deck.PopEnd();
                s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromSideZone));
            }
        }
        else
        {
            if(logging) s.LogIndent(1, "puts the cards back on top of their deck");
            s.ReorderDeck(source, s.player, cardsToReveal);
        }
        done = true;
    }

    Card *source;
    bool done;
};

class EventLookout : public Event
{
public:
    EventLookout(Card *_source)
    {
        source = _source;
        done = false;
        cardToTrash = NULL;
        revealedCards[0] = NULL;
        revealedCards[1] = NULL;
        revealedCards[2] = NULL;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    UINT Stage() const
    {
        int count = 0;
        if(revealedCards[0] != NULL) count++;
        if(revealedCards[1] != NULL) count++;
        if(revealedCards[2] != NULL) count++;
        if(count == 0) return 0;
        if(count == 3) return 1;
        if(count == 2) return 2;
        return -1;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[s.player];
        
        int stage = Stage();
        if(stage == 0)
        {
            if(p.deck.Length() < 3) s.Shuffle(s.player);
            if(p.deck.Length() < 3)
            {
                if(logging) s.Log("does not have enough cards to reveal");
                return true;
            }

            for(int cardIndex = 0; cardIndex < 3; cardIndex++)
            {
                revealedCards[cardIndex] = p.deck.Last();
                p.deck.PopEnd();
                if(logging) s.LogIndent(1, "reveals " + revealedCards[cardIndex]->PrettyName());
            }

            s.decision.SelectCards(source, 1, 1);
            if(decisionText) s.decision.text = "Choose a card to trash:";
            for(int i = 0; i < 3; i++) s.decision.AddUniqueCard(revealedCards[i]);
        }

        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        int stage = Stage();
        if(stage == 1)
        {
            cardToTrash = response.cards[0];

            s.decision.SelectCards(source, 1, 1);
            if(decisionText) s.decision.text = "Choose a card to discard:";
            bool cardFound = false;
            for(int i = 0; i < 3; i++)
            {
                if(!cardFound && revealedCards[i] == response.cards[0])
                {
                    cardFound = true;
                    revealedCards[i] = NULL;
                }
                if(revealedCards[i] != NULL) s.decision.AddUniqueCard(revealedCards[i]);
            }
        }
        else if(stage == 2)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, response.cards[0], DiscardFromSideZone));
            s.stack.PushEnd(new EventTrashCardFromSideZone(s.player, cardToTrash));
            bool cardFound = false;
            for(int i = 0; i < 3; i++)
            {
                if(!cardFound && revealedCards[i] == response.cards[0])
                {
                    cardFound = true;
                    revealedCards[i] = NULL;
                }
            }
            for(int i = 0; i < 3; i++)
            {
                if(revealedCards[i] != NULL)
                {
                    if(logging) s.LogIndent(1, "puts " + revealedCards[i]->PrettyName() + " on top of their deck");
                    s.players[s.player].deck.PushEnd(revealedCards[i]);
                }
            }
            done = true;
        }
    }

    Card *source;
    Card *revealedCards[3];
    Card *cardToTrash;
    bool done;
};

class EventSeaHagAttack : public Event
{
public:
    EventSeaHagAttack(Card *_source, UINT _player)
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
        if(p.deck.Length() == 0) s.Shuffle(player);

        s.stack.PushEnd(new EventGainCard(player, s.data->baseCards.curse, false, false, GainToDeckTop));

        if(p.deck.Length() == 0)
        {
            if(logging) s.LogIndent(1, player, "has no cards to reveal");
        }
        else
        {
            if(logging) s.LogIndent(1, player, "reveals " + p.deck.Last()->PrettyName());
            s.stack.PushEnd(new EventDiscardCard(player, p.deck.Last(), DiscardFromSideZone));
            p.deck.PopEnd();
        }
        return true;
    }
    Card *source;
    UINT player;
    AttackAnnotations annotations;
};

class EventCutpurseAttack : public Event
{
public:
    EventCutpurseAttack(Card *_source, UINT _player)
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

        if(p.hand.Contains(s.data->baseCards.copper))
        {
            s.stack.PushEnd(new EventDiscardCard(player, s.data->baseCards.copper, DiscardFromHand));
        }
        else
        {
            if(logging) for(Card *c : p.hand) s.LogIndent(1, player, "reveals " + c->PrettyName());
        }
        return true;
    }
    Card *source;
    UINT player;
    AttackAnnotations annotations;
};

class EventAmbassador : public Event
{
public:
    EventAmbassador(Card *_source)
    {
        source = _source;
        revealedCard = NULL;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        if(revealedCard == NULL)
        {
            PlayerState &p = s.players[s.player];
        
            if(p.hand.Length() == 0)
            {
                if(logging) s.LogIndent(1, "has no cards to reveal");
                return true;
            }

            s.decision.SelectCards(source, 1, 1);
            if(decisionText) s.decision.text = "Choose a card to reveal:";
            s.decision.AddUniqueCards(p.hand);
        }
        
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(revealedCard == NULL)
        {
            revealedCard = response.cards[0];
            if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());

            int handCount = 0;
            for(Card *c : p.hand) if(c == revealedCard) handCount++;
            handCount = Math::Min(handCount, 2);
            
            if(handCount == 1)
            {
                s.decision.MakeDiscreteChoice(source, 2);
                if(decisionText) s.decision.text = "Return how many cards to the supply?|0|1";
            }
            else if(handCount == 2)
            {
                s.decision.MakeDiscreteChoice(source, 3);
                if(decisionText) s.decision.text = "Return how many cards to the supply?|0|1|2";
            }
        }
        else
        {
            if(logging) s.LogIndent(1, "returns " + String(response.choice) + " " + revealedCard->PrettyName() + " to the supply");
            for(UINT returnIndex = 0; returnIndex < response.choice; returnIndex++)
            {
                p.hand.RemoveSwap(p.hand.FindFirstIndex(revealedCard));
                s.supply[s.data->SupplyIndex(revealedCard)].count++;
            }

            for(const PlayerInfo &p : s.data->players)
            {
                if(p.index != s.player)
                {
                    s.stack.PushEnd(new EventGainCard(p.index, revealedCard, false, true, GainToDiscard));
                }
            }

            done = true;
        }
    }

    Card *source;
    Card *revealedCard;
    bool done;
};

class EventExplorer : public Event
{
public:
    EventExplorer(Card *_source)
    {
        source = _source;
        done = false;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[s.player];

        if(p.hand.Contains(s.data->baseCards.province))
        {
            s.decision.MakeDiscreteChoice(source, 2);
            if(decisionText) s.decision.text = "Reveal a Province?|Yes|No";
            return false;
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.silver, false, false, GainToHand));
            return true;
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        if(response.choice == 0)
        {
            if(logging) s.LogIndent(1, "reveals a " + s.data->baseCards.province->PrettyName());
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.gold, false, false, GainToHand));
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.silver, false, false, GainToHand));
        }
        done = true;
    }

    Card *source;
    bool done;
};
