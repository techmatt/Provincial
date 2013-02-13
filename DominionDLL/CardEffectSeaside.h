
class CardCaravan : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("draws a card from " + c->PrettyName());
        s.stack.PushEnd(new EventDrawCard(s.player));
    }
};

class CardLighthouse : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("gets $1 from " + c->PrettyName());
        s.players[s.player].money++;
    }
};

class CardTactician : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to discard");
            for(CardPlayInfo &playCard : p.playArea)
            {
                if(playCard.card == c && playCard.copies > 0 && playCard.turnsLeft > 0)
                {
                    //
                    // TODO: This may be a conservative implementation of tactician
                    //
                    playCard.copies--;
                    return;
                }
            }
        }
        for(Card *c : p.hand) s.stack.PushEnd(new EventDiscardCard(s.player, c, DiscardFromHand));
    }
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("draws 5 cards from " + c->PrettyName());

        PlayerState &p = s.players[s.player];
        p.actions++;
        p.buys++;
        for(int i = 0; i < 5; i++) s.stack.PushEnd(new EventDrawCard(s.player));
    }
};

class CardFishingVillage : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("gets $1 from " + c->PrettyName());
        s.players[s.player].actions++;
        s.players[s.player].money++;
    }
};

class CardMerchantShip : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("gets $2 from " + c->PrettyName());
        s.players[s.player].money += 2;
    }
};

class CardWharf : public CardEffect
{
public:
    void ProcessDuration(State &s) const
    {
        if(logging) s.Log("draws 2 cards from " + c->PrettyName());
        s.stack.PushEnd(new EventDrawCard(s.player));
        s.stack.PushEnd(new EventDrawCard(s.player));
        s.players[s.player].buys++;
    }
};

class CardWarehouse : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventDiscardNCards(c, s.player, 3, 3));
    }
};

class CardPearlDiver : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(p.deck.Length() == 0) s.Shuffle(s.player);
        if(p.deck.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to reveal");
        }
        else
        {
            Card *revealedCard = p.deck.First();
            if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName() + " from the bottom of their deck");
            s.decision.MakeDiscreteChoice(c, 2);
            if(decisionText) s.decision.text = "Put" + revealedCard->PrettyName() + "on top of your deck?|Yes|No";
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        PlayerState &p = s.players[s.player];
        Card *revealedCard = p.deck.First();
        if(response.choice == 0)
        {
            if(logging) s.LogIndent(1, "puts " + revealedCard->PrettyName() + " on top of their deck");
            p.deck.RemoveSlow(0);
            p.deck.PushEnd(revealedCard);
        }
        else
        {
            if(logging) s.LogIndent(1, "leaves " + revealedCard->PrettyName() + " on the bottom of their deck");
        }
    }
};

class CardLookout : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventLookout(c));
    }
};

class CardNavigator : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventNavigator(c));
    }
};

class CardSalvager : public CardEffect
{
public:
    bool CanProcessDecisions() const
    {
        return true;
    }
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
            return;
        }

        s.decision.SelectCards(c, 1, 1);
        if(decisionText) s.decision.text = "Choose a card to trash:";
        s.decision.AddUniqueCards(p.hand);
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        for(Card *c : response.cards)
        {
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, c));
            int value = s.SupplyCost(c);
            s.players[s.player].money += value;
            if(logging) s.LogIndent(1, "gets $" + String(value));
        }
    }
};

class CardSeaHag : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventSeaHagAttack(c, p.index));
            }
        }
    }
};

class CardAmbassador : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventAmbassador(c));
    }
};

class CardGhostShip : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventPutOnDeckDownToN(c, p.index, 3));
            }
        }
    }
};

class CardCutpurse : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventCutpurseAttack(c, p.index));
            }
        }
    }
};

class CardSmugglers : public CardEffect
{
public:
    bool CanProcessDecisions() const
    {
        return true;
    }
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];

        s.decision.SelectCards(c, 1, 1);
        for(Card *c : s.prevGainList)
        {
            if(s.SupplyCost(c) <= 6) s.decision.AddUniqueCard(c);
        }

        if(s.decision.cardChoices.Length() == 0)
        {
            s.decision.type = DecisionNone;
            if(logging) s.LogIndent(1, "cannot gain any cards");
            return;
        }

        if(decisionText) s.decision.text = "Choose a card to gain:";
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        s.stack.PushEnd(new EventGainCard(s.player, response.cards[0]));
    }
};

class CardTreasureMap : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        s.stack.PushEnd(new EventTrashCardFromPlay(s.player, c));

        if(p.hand.Contains(s.data->baseCards.treasureMap))
        {
            s.stack.PushEnd(new EventTrashCardFromHand(s.player, c));
            for(int goldIndex = 0; goldIndex < 4; goldIndex++)
            {
                s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.gold, false, false, GainToDeckTop));
            }
        }
    }
};

class CardExplorer : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventExplorer(c));
    }
};

class CardIsland : public CardEffect
{
public:
    bool CanProcessDecisions() const
    {
        return true;
    }
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];

        bool islandFound = false;
        for(UINT playIndex = 0; playIndex < p.playArea.Length(); playIndex++)
        {
            if(p.playArea[playIndex].card == c)
            {
                p.playArea.RemoveSwap(playIndex);
                p.islandZone.PushEnd(c);
                islandFound = true;
            }
        }

        if(!islandFound)
        {
            //
            // Sometimes we will not find the island in play because of throne room
            //
            return;
        }

        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards in hand to set aside");
            return;
        }

        s.decision.SelectCards(c, 1, 1);
        s.decision.AddUniqueCards(p.hand);
        if(decisionText) s.decision.text = "Choose a card to set aside:";
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        PlayerState &p = s.players[s.player];
        Card *asideCard = response.cards[0];
        p.hand.RemoveSwap(p.hand.FindFirstIndex(asideCard));
        p.islandZone.PushEnd(response.cards[0]);
        if(logging) s.LogIndent(1, "sets " + asideCard->PrettyName() + " aside");
    }
};
