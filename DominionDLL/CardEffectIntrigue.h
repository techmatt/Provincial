class CardPawn : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.MakeDiscreteChoice(c, 6);
        if(decisionText) s.decision.text = "Choose one:|+1 Card, +1 Action|+1 Card, +1 Buy|+1 Card, +1 Coin|+1 Action, +1 Buy|+1 Action, +1 Coin|+1 Coin, +1 Buy";
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
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
    }
};

class CardCourtyard : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Choose a card to put on top of your deck:";
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
        Card *c = response.cards[0];
        PlayerState &p = s.players[s.player];
        p.hand.RemoveSwap(p.hand.FindFirstIndex(c));
        p.deck.PushEnd(c);
        if(logging) s.LogIndent(1, s.decision.controllingPlayer, "puts " + c->PrettyName() + " on their deck");
    }
};

class CardShantyTown : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].ActionCount() > 0)
        {
            if(logging) s.LogIndent(1, "reveals a hand with action cards");
        }
        else
        {
            if(logging) s.LogIndent(1, "reveals a hand with no action cards");
            s.stack.PushEnd(new EventDrawCard(s.player));
            s.stack.PushEnd(new EventDrawCard(s.player));
        }
    }
};

class CardSteward : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventSteward(c));
    }
};

class CardWishingWell : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.SelectCards(c, 1, 1);
        if(decisionText) s.decision.text = "Name a card:";
        s.decision.cardChoices = s.data->supplyCards;
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        Card *c = response.cards[0];
        PlayerState &p = s.players[s.player];
        
        if(p.deck.Length() == 0)
        {
            s.Shuffle(s.player);
        }

        if(p.deck.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to reveal");
            return;
        }

        Card *revealedCard = p.deck.Last();
        if(logging) s.LogIndent(1, "names " + response.cards[0]->PrettyName());
        if(logging) s.LogIndent(1, "reveals " + revealedCard->PrettyName());
        
        if(revealedCard == response.cards[0]) s.stack.PushEnd(new EventDrawCard(s.player));
    }
};

class CardBaron : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Contains(s.data->baseCards.estate))
        {
            s.decision.MakeDiscreteChoice(c, 2);
            if(decisionText) s.decision.text = "Discard an estate?|Yes|No";
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.estate));
        }
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        PlayerState &p = s.players[s.player];
        
        if(response.choice == 0)
        {
            s.stack.PushEnd(new EventDiscardCard(s.player, s.data->baseCards.estate, DiscardFromHand));
            if(logging) s.LogIndent(1, "gets $4");
            p.money += 4;
        }
        else if(response.choice == 1)
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.estate));
        }
    }
};

class CardNobles : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.MakeDiscreteChoice(c, 2);
        if(decisionText) s.decision.text = "Choose one:|+3 Cards|+2 Actions";
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        PlayerState &p = s.players[s.player];
        
        if(response.choice == 0)
        {
            for(UINT cardIndex = 0; cardIndex < 3; cardIndex++) s.stack.PushEnd(new EventDrawCard(s.player));
        }
        else if(response.choice == 1)
        {
            p.actions += 2;
        }
    }
};

class CardTradingPost : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(p.hand.Length() == 0)
        {
            if(logging) s.LogIndent(1, "has no cards to trash");
        }
        else if(p.hand.Length() == 1)
        {
            s.stack.PushEnd(new EventChooseCardsToTrash(c, 1, 1));
            return;
        }
        else
        {
            s.stack.PushEnd(new EventGainCard(s.player, s.data->baseCards.silver, false, false, GainToHand));
            s.stack.PushEnd(new EventChooseCardsToTrash(c, 2, 2));
        }
    }
};

class CardScout : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];

        if(p.deck.Length() < 4)
        {
            s.Shuffle(s.player);
        }

        int cardsToReveal = Math::Min((int)p.deck.Length(), 4);
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
                if(logging) s.LogIndent(1, "reveals " + c->PrettyName() + ", putting it into their hand");
                p.hand.PushEnd(c);
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

class CardDuke : public CardEffect
{
public:
    int VictoryPoints(const State &s, UINT playerIndex) const
    {
        CardCounter counter(s.players[playerIndex]);
        return counter.Count(s.data->baseCards.duchy);
    }
};

class CardConspirator : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        PlayerState &p = s.players[s.player];
        if(s.ActionsPlayedThisTurn() >= 3)
        {
            s.stack.PushEnd(new EventDrawCard(s.player));
            s.players[s.player].actions++;
        }
    }
};

class CardIronworks : public CardEffect
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
        s.stack.PushEnd(new EventGainCard(s.player, response.cards[0], false, false, GainToDiscardIronworks));
    }
};

class CardSwindler : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventSwindlerAttack(c, p.index));
            }
        }
    }
};

class CardMinion : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.stack.PushEnd(new EventMinion(c));
    }
};

class CardTorturer : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventTorturerAttack(c, p.index));
            }
        }
    }
};

class CardSaboteur : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        for(const PlayerInfo &p : s.data->players)
        {
            if(p.index != s.player)
            {
                s.stack.PushEnd(new EventSaboteurAttack(c, p.index));
            }
        }
    }
};

class CardUpgrade : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        if(s.players[s.player].hand.Length() > 0)
        {
            s.decision.SelectCards(c, 1, 1);
            if(decisionText) s.decision.text = "Select a card to trash:";
            s.decision.cardChoices = s.players[s.player].hand;
            s.stack.PushEnd(new EventUpgrade(c));
        }
        else
        {
            if(logging) s.Log("has no cards to trash");
        }
    }
};

class CardTribute : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        int player = Math::Mod((int)s.player - 1, s.data->players.Length());
        PlayerState &p = s.players[player];
        if(p.deck.Length() < 2) s.Shuffle(player);
        int cardsToReveal = Math::Min(2, (int)p.deck.Length());

        if(cardsToReveal == 0)
        {
            if(logging) s.LogIndent(1, player, "has no cards to reveal");
            return;
        }
        
        Card *firstReveal = NULL;
        for(int revealIndex = 0; revealIndex < cardsToReveal; revealIndex++)
        {
            Card *revealedCard = p.deck.Last();
            p.deck.PopEnd();

            if(logging) s.LogIndent(1, player, "reveals " + revealedCard->PrettyName());
            if(revealedCard != firstReveal)
            {
                if(revealedCard->isAction) s.players[s.player].actions += 2;
                if(revealedCard->isVictory)
                {
                    s.stack.PushEnd(new EventDrawCard(s.player));
                    s.stack.PushEnd(new EventDrawCard(s.player));
                }
                if(revealedCard->isTreasure)
                {
                    if(logging) s.LogIndent(1, "gets $2");
                    s.players[s.player].money += 2;
                }
            }

            firstReveal = revealedCard;
            s.stack.PushEnd(new EventDiscardCard(player, revealedCard, DiscardFromSideZone));
        }
    }
};

class CardMiningVillage : public CardEffect
{
public:
    void PlayAction(State &s) const
    {
        s.decision.MakeDiscreteChoice(c, 2);
        if(decisionText) s.decision.text = "Trash mining village?|Yes|No";
    }
    bool CanProcessDecisions() const
    {
        return true;
    }
    void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        PlayerState &p = s.players[s.player];
        
        if(response.choice == 0)
        {
            p.money += 2;
            if(logging) s.LogIndent(1, "gains $2");
            s.stack.PushEnd(new EventTrashCardFromPlay(s.player, c));
        }
        else if(response.choice == 1)
        {
            if(logging) s.LogIndent(1, "does not trash " + c->PrettyName());
        }
    }
};