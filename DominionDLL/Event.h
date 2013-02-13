
//
// AttackAnnotations is special annotation that attack events must store. They indicate which reactions
// have already been processed for the attack.
//
struct AttackAnnotations
{
    AttackAnnotations()
    {
        moatProcessed = false;
    }
    bool moatProcessed;
};

//
// A list of Event objects composes the event stack of a Dominion game.
// If Advance returns true, the event is done and should be removed, otherwise it will keep recurring.
//
class Event
{
public:
    virtual ~Event() {}
    virtual bool Advance(State &s) = 0;
    virtual bool IsAttack() const
    {
        return false;
    }
    virtual int AttackedPlayer() const
    {
        return -1;
    }
    virtual AttackAnnotations* Annotations()
    {
        return NULL;
    }
    virtual bool CanProcessDecisions() const
    {
        return false;
    }
    virtual bool DestroyNextEventOnStack() const
    {
        return false;
    }
    virtual void ProcessDecision(State &s, const DecisionResponse &response)
    {
        SignalError("Event does not support decisions");
    }
};

//
// Making card draw an effect is easy but inefficient since it is only needed for Stash, a rather inelegant card I don't plan to implement.
// Still, card draw can be pushed onto the stack by cards such as Cellar. This is because you need to process the discards before drawing,
// so that the discarded cards can be drawn.
//
// ** Possible triggers
// Stash
//
class EventDrawCard : public Event
{
public:
    EventDrawCard(UINT _player)
    {
        player = _player;
    }
    bool Advance(State &s);

    UINT player;
};

class EventTrashCardFromPlay : public Event
{
public:
    EventTrashCardFromPlay(UINT _player, Card *_card)
    {
        player = _player;
        card = _card;
    }
    bool Advance(State &s);

    UINT player;
    Card *card;
};

class EventTrashCardFromHand : public Event
{
public:
    EventTrashCardFromHand(UINT _player, Card *_card)
    {
        player = _player;
        card = _card;
    }
    bool Advance(State &s);

    UINT player;
    Card *card;
};

class EventTrashCardFromDeck : public Event
{
public:
    EventTrashCardFromDeck(UINT _player)
    {
        player = _player;
    }
    bool Advance(State &s)
    {
        Card *c = s.players[player].deck.Last();
        if(logging) s.LogIndent(1, player, "trashes " + c->PrettyName() + " from the top of their deck");
        s.players[player].deck.PopEnd();
        return true;
    }

    UINT player;
};

class EventTrashCardFromSideZone : public Event
{
public:
    EventTrashCardFromSideZone(UINT _player, Card *_c)
    {
        player = _player;
        c = _c;
    }
    bool Advance(State &s)
    {
        if(logging) s.LogIndent(1, player, "trashes " + c->PrettyName());
        return true;
    }

    UINT player;
    Card *c;
};

//
// When fromSideZone is true, the card is pulled from a special reserved zone instead of the hand.
//
// ** Possible triggers
// Tunnel
//
class EventDiscardCard : public Event
{
public:
    EventDiscardCard(UINT _player, Card *_card, DiscardZone _zone)
    {
        player = _player;
        card = _card;
        zone = _zone;
    }
    bool Advance(State &s);

    UINT player;
    Card *card;
    DiscardZone zone;
};

//
// ** Possible triggers
// Watchtower
// Royal seal
// Mint
//
class EventGainCard : public Event
{
public:
    EventGainCard(UINT _player, Card *_card)
    {
        player = _player;
        card = _card;
        bought = false;
        isAttack = false;
        zone = GainToDiscard;
        state = TriggerNone;
    }
    EventGainCard(UINT _player, Card *_card, bool _bought, bool _isAttack, GainZone _zone)
    {
        player = _player;
        card = _card;
        bought = _bought;
        isAttack = _isAttack;
        zone = _zone;
        state = TriggerNone;
    }
    bool IsAttack() const
    {
        return isAttack;
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
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(state == TriggerProcessingRoyalSeal)
        {
            if(response.choice == 0)
            {
                if(logging) s.LogIndent(1, "uses Royal Seal");
                zone = GainToDeckTop;
            }
            else
            {
                if(logging) s.LogIndent(1, "does not use Royal Seal");
            }
        }
        else if(state == TriggerProcessingWatchtower)
        {
            if(response.choice == 1)
            {
                if(logging) s.LogIndent(1, player, "reveals " + s.data->baseCards.watchtower->PrettyName());
                zone = GainToDeckTop;
            }
            else if(response.choice == 0)
            {
                if(logging) s.LogIndent(1, player, "reveals " + s.data->baseCards.watchtower->PrettyName());
                if(logging) s.LogIndent(1, player, "trashes " + card->PrettyName());
                zone = GainToTrash;
            }
            else if(response.choice == 2)
            {
                if(logging) s.LogIndent(1, player, "doesn't reveal " + s.data->baseCards.watchtower->PrettyName());
            }
        }
        state = TriggerProcessed;
    }
    bool Advance(State &s)
    {
        SupplyEntry &curSupply = s.supply[s.data->SupplyIndex(card)];
        PlayerState &p = s.players[player];
        if(curSupply.count > 0)
        {
            if((s.data->watchtowerInSupply || s.data->royalSealInSupply) && state == TriggerNone)
            {
                if(p.hand.Contains(s.data->baseCards.watchtower))
                {
                    state = TriggerProcessingWatchtower;
                    s.decision.MakeDiscreteChoice(s.data->baseCards.watchtower, 3);
                    s.decision.controllingPlayer = player;
                    if(decisionText) s.decision.text = "About to gain" + card->PrettyName() +":|Trash it|Put it on top of deck|Don't use Watchtower";
                    return false;
                }
                else if(p.CardInPlay(s.data->baseCards.royalSeal))
                {
                    state = TriggerProcessingRoyalSeal;
                    s.decision.MakeDiscreteChoice(s.data->baseCards.royalSeal, 2);
                    if(decisionText) s.decision.text = "Put" + card->PrettyName() + "on top of your deck?|Yes|No";
                    return false;
                }
            }

            if(s.data->useGainList && player == s.player) s.gainList.PushEnd(card);
            if(s.data->tradeRouteInSupply)
            {
                SupplyEntry &entry = s.supply[s.data->SupplyIndex(card)];
                if(entry.tradeRouteToken == 1)
                {
                    entry.tradeRouteToken = 0;
                    s.tradeRouteValue++;
                }
            }

            String zoneModifier;
            if(zone == GainToHand)
            {
                p.hand.PushEnd(card);
                zoneModifier = " in hand";
            }
            else if(zone == GainToDiscard || zone == GainToDiscardIronworks)
            {
                p.discard.PushEnd(card);
                if(zone == GainToDiscardIronworks)
                {
                    if(card->isAction) p.actions++;
                    if(card->isTreasure)
                    {
                        if(logging) s.LogIndent(1, player, "gets $1");
                        p.money++;
                    }
                    if(card->isVictory) s.stack.PushEnd(new EventDrawCard(s.player));
                }
            }
            else if(zone == GainToDeckTop)
            {
                p.deck.PushEnd(card);
                zoneModifier = " and puts it on their deck";
            }
            curSupply.count--;
            if(bought)
            {
                int cost = s.SupplyCost(card);
                if(logging) s.Log(player, "buys " + card->PrettyName());
                if(logging) s.LogIndent(1, "spends $" + String(cost));
                p.money -= cost;
                p.buys--;
            }
            else if(zone != GainToTrash)
            {
                if(logging) s.LogIndent(1, player, "gains a " + card->PrettyName() + zoneModifier);
            }
        }
        else
        {
            if(bought)
            {
                //
                // We will sometimes get here if the player asks for multiple piles of the same type
                //
                if(logging) s.LogIndent(1, player, "cannot buy " + card->PrettyName());
                p.buys--;
            }
            else
            {
                if(logging) s.LogIndent(1, player, "cannot gain " + card->PrettyName());
            }
        }
        return true;
    }

    enum TriggerState
    {
        TriggerNone,
        TriggerProcessed,
        TriggerProcessingRoyalSeal,
        TriggerProcessingWatchtower,
    };

    UINT player;
    Card *card;
    bool bought;
    bool isAttack;
    TriggerState state;
    GainZone zone;
    AttackAnnotations annotations;
};

class EventDiscardDownToN : public Event
{
public:
    EventDiscardDownToN(Card *_source, UINT _player, UINT _handSize)
    {
        source = _source;
        player = _player;
        handSize = _handSize;
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
    bool Advance(State &s)
    {
        if(done) return true;

        int currentHandSize = int(s.players[player].hand.Length());
        int cardsToDiscard = currentHandSize - handSize;
        if(cardsToDiscard <= 0)
        {
            if(logging) s.Log(player, "has " + String(currentHandSize) + " cards in hand");
            return true;
        }

        s.decision.SelectCards(source, cardsToDiscard, cardsToDiscard);
        s.decision.cardChoices = s.players[player].hand;
        s.decision.controllingPlayer = player;
        if(decisionText)
        {
            if(cardsToDiscard == 1) s.decision.text = "Choose a card to discard:";
            else s.decision.text = "Choose " + String(cardsToDiscard) + " cards to discard:";
        }
        return false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventDiscardCard(s.decision.controllingPlayer, c, DiscardFromHand));
        }
        done = true;
    }

    Card *source;
    UINT player;
    UINT handSize;
    AttackAnnotations annotations;
    bool done;
};

class EventPutOnDeckDownToN : public Event
{
public:
    EventPutOnDeckDownToN(Card *_source, UINT _player, UINT _handSize)
    {
        source = _source;
        player = _player;
        handSize = _handSize;
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
    bool Advance(State &s)
    {
        if(done) return true;

        int currentHandSize = int(s.players[player].hand.Length());
        int cardsToDiscard = currentHandSize - handSize;
        if(cardsToDiscard <= 0)
        {
            if(logging) s.Log(player, "has " + String(currentHandSize) + " cards in hand");
            return true;
        }

        s.decision.SelectCards(source, cardsToDiscard, cardsToDiscard);
        s.decision.cardChoices = s.players[player].hand;
        s.decision.controllingPlayer = player;
        if(decisionText)
        {
            if(cardsToDiscard == 1) s.decision.text = "Choose a card to put on top of your deck:";
            else s.decision.text = "Choose " + String(cardsToDiscard) + " cards to put on top of your deck:";
        }
        return false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[player];
        for(Card *c : response.cards)
        {
            if(logging) s.LogIndent(1, player, "puts " + c->PrettyName() + " on top of their deck");
            p.hand.RemoveSwap(p.hand.FindFirstIndex(c));
            p.deck.PushEnd(c);
        }
        done = true;
    }

    Card *source;
    UINT player;
    UINT handSize;
    AttackAnnotations annotations;
    bool done;
};

class EventDiscardNCards : public Event
{
public:
    EventDiscardNCards(Card *_source, UINT _player, int _minDiscardCount, int _maxDiscardCount)
    {
        source = _source;
        player = _player;
        minDiscardCount = _minDiscardCount;
        maxDiscardCount = _maxDiscardCount;
        done = false;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        int currentHandSize = int(s.players[player].hand.Length());
        int cardsToDiscard = Math::Min(currentHandSize, maxDiscardCount);
        minDiscardCount = Math::Min(minDiscardCount, cardsToDiscard);
        if(cardsToDiscard == 0)
        {
            if(logging) s.Log(player, "has no cards to discard");
            return true;
        }

        s.decision.SelectCards(source, minDiscardCount, cardsToDiscard);
        s.decision.cardChoices = s.players[player].hand;
        s.decision.controllingPlayer = player;
        if(decisionText)
        {
            if(cardsToDiscard == 1) s.decision.text = "Choose a card to discard:";
            else s.decision.text = "Choose " + String(cardsToDiscard) + " cards to discard:";
        }
        return false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventDiscardCard(s.decision.controllingPlayer, c, DiscardFromHand));
        }
        if(source == s.data->baseCards.furnace && response.cards.Length() > 0)
        {
            if(logging) s.LogIndent(1, "gets $1 from " + source->PrettyName());
            s.players[s.player].money++;
        }
        done = true;
    }

    Card *source;
    UINT player;
    int minDiscardCount, maxDiscardCount;
    bool done;
};

class EventPlayActionNTimes : public Event
{
public:
    EventPlayActionNTimes(Card *_source, UINT _count)
    {
        source = _source;
        target = NULL;
        count = _count;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(count == 0) return true;
        PlayerState &p = s.players[s.player];

        if(target == NULL)
        {
            if(p.ActionCount() == 0)
            {
                if(logging) s.Log("has no actions to play");
                return true;
            }

            s.decision.SelectCards(source, 1, 1);
            if(decisionText) s.decision.text = "Select an action to play " + String(count) + " times:";
            if(source == s.data->baseCards.trailblazer)
            {
                for(Card *c : p.hand)
                {
                    if(c->isAction && s.SupplyCost(c) <= 3) s.decision.AddUniqueCard(c);
                }
                if(s.decision.cardChoices.Length() == 0)
                {
                    s.decision.type = DecisionNone;
                    if(logging) s.Log("has no actions to play");
                    return true;
                }
            }
            else
            {
                for(Card *c : p.hand)
                {
                    if(c->isAction) s.decision.AddUniqueCard(c);
                }
            }
        }
        else
        {
            if(logging) s.Log("plays " + target->PrettyName());
            count--;
            s.ProcessAction(target);
        }
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        target = response.cards[0];
        int index = p.hand.FindFirstIndex(target);
        p.hand.RemoveSwap(index);

        if(source == s.data->baseCards.witchdoctor)
        {
            int supplyCount = s.SupplyCount(target);
            if(supplyCount == 0)
            {
                if(logging) s.LogIndent(1, "cannot gain " + target->PrettyName());
                count = 1;
            }
            else
            {
                s.stack.PushEnd(new EventGainCard(s.player, target, false, false, GainToDiscard));
            }
        }

        CardPlayInfo newInfo(target, 1);
        newInfo.copies = count;

        p.playArea.PushEnd(newInfo);
    }

    UINT count;
    Card *source, *target;
};

class EventChooseCardsToTrash : public Event
{
public:
    EventChooseCardsToTrash(Card *_source, int _minCards, int _maxCards)
    {
        done = false;
        source = _source;
        minCards = _minCards;
        maxCards = _maxCards;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;
        minCards = Math::Min(minCards, (int)s.players[s.player].hand.Length());
        maxCards = Math::Min(maxCards, (int)s.players[s.player].hand.Length());

        if(maxCards == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
            s.decision.type = DecisionNone;
            return true;
        }

        s.decision.SelectCards(source, minCards, maxCards);
        if(decisionText)
        {
            if(minCards == maxCards && minCards == 1) s.decision.text = "Choose a card to trash:";
            else if(minCards == maxCards) s.decision.text = "Choose " + String(minCards) + " cards to trash:";
            else if(minCards == 0) s.decision.text = "Choose up to " + String(maxCards) + " cards to trash:";
            else s.decision.text = "Choose cards to trash:";
        }
        s.decision.cardChoices = s.players[s.player].hand;
        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        PlayerState &p = s.players[s.player];
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, c));
        }

        if(source == s.data->baseCards.evangelist && response.cards.Length() == 0)
        {
            s.stack.PushEnd(new EventTrashCardFromPlay(s.player, s.data->baseCards.evangelist));
        }

        done = true;
    }

    Card *source;
    int minCards;
    int maxCards;
    bool done;
};

class EventReorderDeck : public Event
{
public:
    EventReorderDeck(Card *_source, UINT _player, int _cardCount)
    {
        done = false;
        source = _source;
        player = _player;
        cardCount = _cardCount;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        PlayerState &p = s.players[player];
        s.decision.SelectCards(source, 1, 1);
        s.decision.controllingPlayer = player;
        if(decisionText)
        {
            if(cardCount == 2) s.decision.text = "Choose a card to put 2nd from the top:";
            else if(cardCount == 3) s.decision.text = "Choose a card to put 3rd from the top:";       
            else if(cardCount == 4) s.decision.text = "Choose a card to put 4th from the top:";       
            else if(cardCount == 5) s.decision.text = "Choose a card to put 5th from the top:";
            else s.decision.text = "Choose a card to on your deck:";
        }

        for(int cardIndex = 0; cardIndex < cardCount; cardIndex++)
        {
            s.decision.AddUniqueCard(p.deck[p.deck.Length() - 1 - cardIndex]);
        }

        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(logging) s.LogIndent(1, player, "puts " + response.cards[0]->PrettyName() + " on top of their deck");
        PlayerState &p = s.players[player];
        for(int cardIndex = 0; cardIndex < cardCount; cardIndex++)
        {
            if(p.deck[p.deck.Length() - 1 - cardIndex] == response.cards[0])
            {
                Utility::Swap(p.deck[p.deck.Length() - cardCount], p.deck[p.deck.Length() - 1 - cardIndex]);
                if(logging && cardCount == 2) s.LogIndent(1, player, "puts " + p.deck.Last()->PrettyName() + " on top of their deck");
                done = true;
                return;
            }
        }
        SignalError("Response card not found");
    }

    Card *source;
    UINT player;
    int cardCount;
    bool done;
};

class EventChooseCardToGain : public Event
{
public:
    EventChooseCardToGain(Card *_source, UINT _controllingPlayer, UINT _gainingPlayer, bool _optionalGain, int _minCost, int _maxCost, CardFilter _filter, GainZone _zone)
    {
        source = _source;
        controllingPlayer = _controllingPlayer;
        gainingPlayer = _gainingPlayer;
        minCost = _minCost;
        maxCost = _maxCost;
        optionalGain = _optionalGain;
        zone = _zone;
        filter = _filter;
        done = false;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    bool Advance(State &s)
    {
        if(done) return true;

        s.decision.GainCardFromSupply(s, source, minCost, maxCost, filter);
        if(optionalGain) s.decision.minimumCards = 0;
        s.decision.controllingPlayer = controllingPlayer;

        if(s.decision.cardChoices.Length() == 0)
        {
            if(logging) s.LogIndent(1, gainingPlayer, "has no cards to gain");
            s.decision.type = DecisionNone;
            return true;
        }

        return false;
    }
    void ProcessDecision(State &s, const DecisionResponse &response)
    {
        if(response.cards.Length() > 0)
        {
            s.stack.PushEnd(new EventGainCard(gainingPlayer, response.cards[0], false, false, zone));
        }
        else
        {
            if(logging) s.LogIndent(1, gainingPlayer, "does not gain a card");
        }
        done = true;
    }

    Card *source;
    UINT controllingPlayer, gainingPlayer;
    int minCost, maxCost;
    GainZone zone;
    CardFilter filter;
    bool optionalGain;
    bool done;
};