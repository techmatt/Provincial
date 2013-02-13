#include "Main.h"

const int debugResourceBonus = 0;
const int debugRandomCardCount = 0;
const char *debugCardName = "gambler";
const int debugCardCount = 0;

bool DecisionState::IsTrivial() const
{
    if(type == DecisionSelectCards && cardChoices.Length() == 1 && minimumCards == 1 && maximumCards == 1)
    {
        return true;
    }
    return false;
}

DecisionResponse DecisionState::TrivialResponse() const
{
    DecisionResponse response;
    if(cardChoices.Length() == 1 && minimumCards == 1 && maximumCards == 1)
    {
        response.cards.Allocate(1);
        response.cards[0] = cardChoices[0];
    }
    return response;
}

void DecisionState::GainCardFromSupply(State &s, Card *c, int minCost, int maxCost, CardFilter filter)
{
    SelectCards(c, 1, 1);
    if(decisionText) text = "Select a card to gain:";
    for(UINT supplyIndex = 0; supplyIndex < s.data->supplyCards.Length(); supplyIndex++)
    {
        int cost = s.SupplyCost(supplyIndex);
        int count = s.supply[supplyIndex].count;
        Card *candidate = s.data->supplyCards[supplyIndex];

        bool cardAcceptable = (count > 0 && cost >= minCost && cost <= maxCost);

        if(filter == FilterTreasure) cardAcceptable &= candidate->isTreasure;

        if(cardAcceptable)
        {
            AddUniqueCard(candidate);
        }
    }
}

void DecisionState::GainTreasureFromSupply(State &s, Card *c, int minCost, int maxCost)
{
    SelectCards(c, 1, 1);
    if(decisionText) text = "Select a treasure to gain:";
    for(UINT supplyIndex = 0; supplyIndex < s.data->supplyCards.Length(); supplyIndex++)
    {
        int cost = s.SupplyCost(supplyIndex);
        int count = s.supply[supplyIndex].count;
        if(count > 0 && cost >= minCost && cost <= maxCost && s.data->supplyCards[supplyIndex]->isTreasure)
        {
            AddUniqueCard(s.data->supplyCards[supplyIndex]);
        }
    }
}

void DecisionState::GainVictoryFromSupply(State &s, Card *c, int minCost, int maxCost)
{
    SelectCards(c, 1, 1);
    if(decisionText) text = "Select a treasure to gain:";
    for(UINT supplyIndex = 0; supplyIndex < s.data->supplyCards.Length(); supplyIndex++)
    {
        int cost = s.SupplyCost(supplyIndex);
        int count = s.supply[supplyIndex].count;
        if(count > 0 && cost >= minCost && cost <= maxCost && s.data->supplyCards[supplyIndex]->isVictory)
        {
            AddUniqueCard(s.data->supplyCards[supplyIndex]);
        }
    }
}

void PlayerState::NewGame(const GameData &data)
{
    playArea.FreeMemory();
    discard.FreeMemory();
    hand.FreeMemory();
    deck.FreeMemory();
    islandZone.FreeMemory();

    if(data.options.startingCondition == StartingCondition34Split)
    {
        for(UINT copperIndex = 0; copperIndex < 3; copperIndex++) deck.PushEnd(data.cards->GetCard("copper"));
        for(UINT estateIndex = 0; estateIndex < 2; estateIndex++) deck.PushEnd(data.cards->GetCard("estate"));

        for(UINT copperIndex = 0; copperIndex < 4; copperIndex++) deck.PushEnd(data.cards->GetCard("copper"));
        for(UINT estateIndex = 0; estateIndex < 1; estateIndex++) deck.PushEnd(data.cards->GetCard("estate"));
    }
    else if(data.options.startingCondition == StartingCondition25Split)
    {
        for(UINT copperIndex = 0; copperIndex < 2; copperIndex++) deck.PushEnd(data.cards->GetCard("copper"));
        for(UINT estateIndex = 0; estateIndex < 3; estateIndex++) deck.PushEnd(data.cards->GetCard("estate"));

        for(UINT copperIndex = 0; copperIndex < 5; copperIndex++) deck.PushEnd(data.cards->GetCard("copper"));
    }
    else
    {
        for(UINT copperIndex = 0; copperIndex < 7; copperIndex++) deck.PushEnd(data.cards->GetCard("copper"));
        for(UINT estateIndex = 0; estateIndex < 3; estateIndex++) deck.PushEnd(data.cards->GetCard("estate"));
        deck.Randomize();
    }

    for(UINT debugCardIndex = 0; debugCardIndex < debugCardCount; debugCardIndex++) deck.PushEnd(data.cards->GetCard(debugCardName));

    //deck.PushEnd(data.cards->GetCard("platinum"));
    //deck.PushEnd(data.cards->GetCard("silver"));
    //deck.PushEnd(data.cards->GetCard("silver"));
    //deck.PushEnd(data.cards->GetCard("copper"));
    //deck.PushEnd(data.cards->GetCard("copper"));

    for(UINT randomCards = 0; randomCards < debugRandomCardCount; randomCards++) deck.PushEnd(data.cards->RandomSupplyCard());
    //deck.PushEnd(data.cards->GetCard("mine"));

    actions = 1 + debugResourceBonus;
    buys = 1 + debugResourceBonus;
    money = 0 + debugResourceBonus;
    VPTokens = 0;
    turns = 0;
}

int PlayerState::TotalCards() const
{
    return deck.Length() + hand.Length() + discard.Length() + playArea.Length() + islandZone.Length();
}

int PlayerState::ActionCount() const
{
    int result = 0;
    for(Card *c : hand) if(c->isAction) result++;
    return result;
}

int PlayerState::TreasureCount() const
{
    int result = 0;
    for(Card *c : hand) if(c->isTreasure) result++;
    return result;
}

int PlayerState::VictoryCount() const
{
    int result = 0;
    for(Card *c : hand) if(c->isVictory) result++;
    return result;
}

int PlayerState::MoneyTotal() const
{
    int result = money;
    for(Card *c : hand) if(c->isTreasure) result += c->treasure;
    return result;
}

/*map<Card*,int> PlayerState::CardCounts() const
{
    map<Card*,int> result;
    for(Card *c : hand            ) if(result.find(c) == result.end()) result[c] = 1; else result[c]++;
    for(Card *c : deck            ) if(result.find(c) == result.end()) result[c] = 1; else result[c]++;
    for(Card *c : discard         ) if(result.find(c) == result.end()) result[c] = 1; else result[c]++;
    for(Card *c : islandZone      ) if(result.find(c) == result.end()) result[c] = 1; else result[c]++;
    for(CardPlayInfo c : playArea) if(result.find(c.card) == result.end()) result[c.card] = 1; else result[c.card]++; 
    return result;
}*/

UINT State::SupplyCost(int supplyIndex) const
{
    return SupplyCost(data->supplyCards[supplyIndex]);
}

UINT State::SupplyCost(Card *c) const
{
    int cost = c->cost;

    if(c == data->baseCards.peddler && phase == PhaseBuy)
    {
        int actionCardCount = 0;
        for(const CardPlayInfo &c : players[player].playArea) if(c.card->isAction) actionCardCount++;
        cost = Math::Max(0, cost - actionCardCount * 2);
    }

    if(data->bridgeInSupply)
    {
        const Vector<CardPlayInfo> &playArea = players[player].playArea;
        for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
        {
            if(playArea[playIndex].card == data->baseCards.bridge) cost -= playArea[playIndex].copies;
        }
        if(cost < 0) cost = 0;
    }

    if(data->quarryInSupply)
    {
        const Vector<CardPlayInfo> &playArea = players[player].playArea;
        for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
        {
            if(c->isAction && playArea[playIndex].card == data->baseCards.quarry) cost -= 2;
        }
        if(cost < 0) cost = 0;
    }

    if(data->plunderInSupply)
    {
        const Vector<CardPlayInfo> &playArea = players[player].playArea;
        for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
        {
            if(c->isTreasure && playArea[playIndex].card == data->baseCards.plunder) cost -= 2;
        }
        if(cost < 0) cost = 0;
    }

    return cost;
}

UINT State::SupplyCount(Card *c) const
{
    int supplyIndex = data->SupplyIndex(c);
    if(supplyIndex == -1) return 0;
    return supply[supplyIndex].count;
}

void State::NewGame(const GameData &_data)
{
    data = &_data;
    const UINT playerCount = data->players.Length();
 
    for(UINT playerIndex = 0; playerIndex < playerCount; playerIndex++)
    {
        players[playerIndex].NewGame(*data);
        DrawCards(playerIndex, 5);
    }

    player = rand() % playerCount;
    players[player].turns = 1;
    phase = PhaseAction;

    stack.FreeMemory();
    gainList.FreeMemory();
    prevGainList.FreeMemory();

    if(logging)
    {
        data->log.Reset();
        data->log("New Game");
        data->log("Players: " + data->players[0].name + ", " + data->players[1].name);
        //data->log(data->players[player].name + " goes first");
        data->log("< " + data->players[player].name + "'s turn -- Turn 1>");
    }

    int victoryCardCount = 8;
    if(playerCount >= 3)
    {
        victoryCardCount = 12;
    }

    tradeRouteValue = 0;

    for(UINT supplyIndex = 0; supplyIndex < maxSupply; supplyIndex++) supply[supplyIndex].count = 0;
    for(UINT supplyIndex = 0; supplyIndex < data->supplyCards.Length(); supplyIndex++)
    {
        Card *c = data->supplyCards[supplyIndex];
        if(c->isVictory)
        {
            supply[supplyIndex].count = victoryCardCount;
            supply[supplyIndex].tradeRouteToken = 1;
        }
        else
        {
            supply[supplyIndex].count = data->supplyCards[supplyIndex]->supply;
            supply[supplyIndex].tradeRouteToken = 0;
        }
    }
    supply[data->SupplyIndex(data->baseCards.copper)].count = 60 - 7 * playerCount;
    supply[data->SupplyIndex(data->baseCards.curse)].count = 10 * (playerCount - 1);

    decision.type = DecisionNone;
    decision.text.FreeMemory();

    AdvanceToNextDecision(0);
}

void State::Shuffle(UINT playerIndex)
{
    if(logging) Log(playerIndex, "shuffles");

    PlayerState &curPlayer = players[playerIndex];
    curPlayer.deck.Append(curPlayer.discard);
    curPlayer.discard.FreeMemory();
    curPlayer.deck.Randomize();
}

void State::DrawCards(UINT playerIndex, UINT cardCount)
{
    for(UINT cardIndex = 0; cardIndex < cardCount; cardIndex++) DrawCard(playerIndex);
}

Card* State::DrawCard(UINT playerIndex)
{
    PlayerState &curPlayer = players[playerIndex];
    if(curPlayer.deck.Length() == 0)
    {
        Shuffle(playerIndex);
    }
    if(curPlayer.deck.Length() > 0)
    {
        Card *c = curPlayer.deck.Last();
        if(logging) LogIndent(1, playerIndex, "draws " + c->PrettyName());
        curPlayer.hand.PushEnd(c);
        curPlayer.deck.PopEnd();
        return c;
    }
    else
    {
        if(logging) Log(playerIndex, "tries to draw, but has no cards left");
        return NULL;
    }
}

void State::DiscardCard(UINT playerIndex, Card *c)
{
    PlayerState &p = players[playerIndex];
    if(logging) Log(playerIndex, "discards " + c->PrettyName());
    int index = p.hand.FindFirstIndex(c);
    p.hand.RemoveSwap(index);
    p.discard.PushEnd(c);
}

void State::PlayCard(UINT playerIndex, Card *c)
{
    PlayerState &p = players[playerIndex];
    if(logging) Log(playerIndex, "plays " + c->PrettyName());
    int index = p.hand.FindFirstIndex(c);
    p.hand.RemoveSwap(index);

    int turnsLeft = 0;
    if(c->isDuration) turnsLeft = 1;
    if(c->isPermanent) turnsLeft = 999;

    p.playArea.PushEnd(CardPlayInfo(c, turnsLeft));
}

void State::ReorderDeck(Card *source, UINT playerIndex, UINT cardCount)
{
    for(UINT reorderIndex = 2; reorderIndex <= cardCount; reorderIndex++)
    {
        stack.PushEnd(new EventReorderDeck(source, playerIndex, reorderIndex));
    }
}

void State::ProcessAction(Card *c)
{
    PlayerState &p = players[player];
    p.actions += c->actions;
    p.buys += c->buys;
    p.money += c->money;
    p.VPTokens += c->victoryTokens;
    if(logging)
    {
        if(c->victoryTokens > 0) LogIndent(1, "gains " + String(c->victoryTokens) + " VP");
        if(c->victoryTokens < 0) LogIndent(1, "loses " + String(c->victoryTokens) + " VP");
        if(c->money > 0) LogIndent(1, "gets $" + String(c->money));
        if(c->money < 0) LogIndent(1, "pays $" + String(-c->money));
    }
    for(int cardIndex = 0; cardIndex < c->cards; cardIndex++)
    {
        DrawCard(player);
    }
    if(c->isAttack && data->championInSupply)
    {
        for(const PlayerInfo &p : data->players)
        {
            if(p.index != player)
            {
                for(CardPlayInfo &c : players[p.index].playArea)
                {
                    if(c.card == data->baseCards.champion)
                    {
                        for(int copyIndex = 0; copyIndex < c.copies; copyIndex++)
                        {
                            if(logging) LogIndent(1, p.index, "gains 1 VP from " + data->baseCards.champion->PrettyName());
                            players[p.index].VPTokens++;
                        }
                    }
                }
            }
        }
    }
    if(c->effect != NULL)
    {
        c->effect->PlayAction(*this);
    }
}

void State::ProcessTreasure(Card *c)
{
    PlayerState &p = players[player];
    int treasureValue = c->treasure;

    if(data->coppersmithInSupply)
    {
        const Vector<CardPlayInfo> &playArea = players[player].playArea;
        for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
        {
            if(playArea[playIndex].card == data->baseCards.coppersmith) treasureValue += playArea[playIndex].copies;
        }
    }

    if(c == data->baseCards.bank)
    {
        for(const CardPlayInfo &c : players[player].playArea) if(c.card->isTreasure) treasureValue++;
    }

    if(logging) LogIndent(1, "gets $" + String(treasureValue));
    p.money += treasureValue;
    p.buys += c->buys;

    if(c->effect != NULL)
    {
        c->effect->PlayAction(*this);
    }
}

void State::ProcessDecision(const DecisionResponse &response)
{
    if(decision.type == DecisionGameOver) return;
    Assert(decision.type != DecisionNone, "No decision active");

    Card *singleCard = response.singleCard;
    if(decision.type == DecisionSelectCards && decision.maximumCards <= 1)
    {
        if(response.cards.Length() == 1) singleCard = response.cards[0];
        Assert(response.cards.Length() <= 1, "Invalid number of cards in response");
        Assert(decision.minimumCards == 0 || singleCard != NULL, "No response chosen");
    }
    else
    {
        Assert(decision.type != DecisionSelectCards || (response.cards.Length() >= decision.minimumCards && response.cards.Length() <= decision.maximumCards), "Invalid number of cards in response");
        Assert(decision.type != DecisionDiscreteChoice || (response.choice < decision.minimumCards), "Invalid choice");
    }

    //
    // Reset the current decision. Note that some cards may still spawn another decision
    // before this function returns.
    //
    decision.type = DecisionNone;

    PlayerState &p = players[player];

    if(decision.activeCard == NULL)
    {
        if(phase == PhaseAction)
        {
            if(singleCard == NULL)
            {
                if(logging) Log("chooses not to play an action");
                phase = PhaseTreasure;
            }
            else
            {
                PlayCard(player, singleCard);
                p.actions--;
                ProcessAction(singleCard);
            }
        }
        else if(phase == PhaseTreasure)
        {
            if(singleCard == NULL)
            {
                if(logging) Log("chooses not to play a teasure");
                phase = PhaseBuy;
                BuyPhaseStart();
            }
            else
            {
                PlayCard(player, singleCard);
                ProcessTreasure(singleCard);
            }
        }
        else if(phase == PhaseBuy)
        {
            if(singleCard == NULL)
            {
                if(logging) Log("chooses not to buy a card");
                phase = PhaseCleanup;
            }
            else
            {
                data->players[player].ledger.RecordBuy(singleCard);
                
                if(data->hoardInSupply && singleCard->isVictory)
                {
                    const Vector<CardPlayInfo> &playArea = players[player].playArea;
                    for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
                    {
                        if(playArea[playIndex].card == data->baseCards.hoard) stack.PushEnd(new EventGainCard(player, data->baseCards.gold, false, false, GainToDiscard));
                    }
                }

                if(data->gardenerInSupply && singleCard->isVictory)
                {
                    const Vector<CardPlayInfo> &playArea = players[player].playArea;
                    for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
                    {
                        if(playArea[playIndex].card == data->baseCards.gardener)
                        {
                            players[player].VPTokens++;
                            if(logging) LogIndent(1, "gains 1 VP from " + data->baseCards.gardener->PrettyName());
                        }
                    }
                }

                if(data->goonsInSupply)
                {
                    const Vector<CardPlayInfo> &playArea = players[player].playArea;
                    for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
                    {
                        if(playArea[playIndex].card == data->baseCards.goons)
                        {
                            players[player].VPTokens++;
                            if(logging) LogIndent(1, "gains 1 VP from " + data->baseCards.goons->PrettyName());
                        }
                    }
                }

                if(data->mintInSupply && singleCard == data->baseCards.mint)
                {
                    Vector<CardPlayInfo> &playArea = players[player].playArea;
                    Vector<Card*> treasuresToTrash;
                    for(UINT playIndex = 0; playIndex < playArea.Length(); playIndex++)
                    {
                        if(playArea[playIndex].card->isTreasure) treasuresToTrash.PushEnd(playArea[playIndex].card);
                    }
                    for(Card *c : treasuresToTrash) stack.PushEnd(new EventTrashCardFromPlay(player, c));
                }

                stack.PushEnd(new EventGainCard(player, singleCard, true, false, GainToDiscard));
            }
        }
    }
    else
    {
        //
        // This order of processing card effects before the stack must be kept because of throne room and king's court
        //
        if(decision.activeCard->effect != NULL &&
           decision.activeCard->effect->CanProcessDecisions())
        {
            decision.activeCard->effect->ProcessDecision(*this, response);
        }
        else if(stack.Length() > 0 && stack.Last()->CanProcessDecisions())
        {
            stack.Last()->ProcessDecision(*this, response);
        }
        else
        {
            SignalError("Decision cannot be processed");
        }
    }
    if(decision.controllingPlayer == -1) decision.controllingPlayer = player;
    decision.maximumCards = Math::Min(decision.maximumCards, decision.cardChoices.Length());
}

void State::AdvancePhase()
{
    PlayerState &p = players[player];
    if(phase == PhaseAction)
    {
        if(p.ActionCount() == 0 || p.actions == 0) phase = PhaseTreasure;
        else
        {
            if(decisionText) decision.text = "Choose an action to play:";
            decision.SelectCards(NULL, 0, 1);
            for(Card *c : p.hand)
            {
                if(c->isAction) decision.AddUniqueCard(c);
            }
        }
    }
    if(phase == PhaseTreasure)
    {
        if(p.TreasureCount() == 0)
        {
            phase = PhaseBuy;
            BuyPhaseStart();
        }
        else
        {
            if(decisionText) decision.text = "Choose a treasure to play:";
            decision.SelectCards(NULL, 0, 1);
            
            //
            // Bank should always be played last so that it's easy to play treasures
            //

            bool bankPresent = false;
            for(Card *c : p.hand)
            {
                if(c->isTreasure)
                {
                    if(c == data->baseCards.bank) bankPresent = true;
                    else decision.AddUniqueCard(c);
                }
            }

            if(bankPresent) decision.AddUniqueCard(data->baseCards.bank);
        }
    }
    if(phase == PhaseBuy && stack.Length() == 0)
    {
        if(p.buys == 0) phase = PhaseCleanup;
        else
        {
            if(decisionText) decision.text = "Choose a card to buy:";
            decision.SelectCards(NULL, 0, 1);
            for(UINT supplyIndex = 0; supplyIndex < data->supplyCards.Length(); supplyIndex++)
            {
                int cost = SupplyCost(supplyIndex);
                int count = supply[supplyIndex].count;
                if(cost <= p.money && count > 0)
                {
                    Card *c = data->supplyCards[supplyIndex];
                    bool canBuyCard = true;
                    if(c == data->baseCards.grandMarket && p.CardInPlay(data->baseCards.copper))
                    {
                        canBuyCard = false;
                    }
                    if(c == data->baseCards.grandCourt)
                    {
                        for(CardPlayInfo &c : p.playArea)
                        {
                            if(c.card->isAction) canBuyCard = false;
                        }
                    }
                    if(canBuyCard) decision.cardChoices.PushEnd(c);
                }
            }
            if(decision.cardChoices.Length() == 0)
            {
                decision.type = DecisionNone;
                phase = PhaseCleanup;
                if(logging) Log("cannot afford to buy any cards");
            }
        }
    }
    if(phase == PhaseCleanup)
    {
        while(p.hand.Length() > 0)
        {
            DiscardCard(player, p.hand.Last());
        }
        
        Vector<CardPlayInfo> newPlayArea;
        for(CardPlayInfo &c : p.playArea)
        {
            if(c.turnsLeft >= 1)
            {
                CardPlayInfo newInfo = c;
                newInfo.turnsLeft = c.turnsLeft - 1;
                newPlayArea.PushEnd(newInfo);
            }
            else
            {
                bool discardCard = true;
                if(data->treasuryInSupply && c.card == data->baseCards.treasury)
                {
                    bool victoryCardGained = false;
                    for(Card *gainedCard : gainList)
                    {
                        if(gainedCard->isVictory) victoryCardGained = true;
                    }
                    if(!victoryCardGained)
                    {
                        discardCard = false;
                        if(logging) Log("puts " + c.card->PrettyName() + " on top of their deck");
                        p.deck.PushEnd(c.card);
                    }
                }
                if(discardCard) p.discard.PushEnd(c.card);
            }
        }

        if(newPlayArea.Length() > 0) p.playArea = newPlayArea;
        else p.playArea.FreeMemory();

        if(logging) Log("draws a new hand");

        int cardsToDraw = 5;
        if(data->promisedLandInSupply)
        {
            for(CardPlayInfo &c : p.playArea)
            {
                if(c.card == data->baseCards.promisedLand)
                {
                    cardsToDraw -= c.copies;
                }
            }
            if(cardsToDraw < 0) cardsToDraw = 0;
        }

        for(int cardIndex = 0; cardIndex < cardsToDraw; cardIndex++)
        {
            DrawCard(player);
        }
        if(logging) Log("ends their turn\n");

        CheckEndConditions();
        if(decision.type == DecisionGameOver) return;

        player = (player + 1) % playerMaximum;
        phase = PhaseAction;
        PlayerState &newPlayer = players[player];
        newPlayer.actions = 1 + debugResourceBonus;
        newPlayer.buys = 1 + debugResourceBonus;
        newPlayer.money = 0 + debugResourceBonus;
        newPlayer.turns++;

        if(data->useGainList)
        {
            prevGainList.FreeMemory();
            prevGainList = move(gainList);
        }

        if(logging) data->log("< " + data->players[player].name + "'s turn - Turn " + String(newPlayer.turns) + " >");

        for(CardPlayInfo &c : players[player].playArea)
        {
            for(int copyIndex = 0; copyIndex < c.copies; copyIndex++)
            {
                if(c.card->effect) c.card->effect->ProcessDuration(*this);
            }
        }
    }
}

void State::AdvanceToNextDecision(UINT recursionDepth)
{
    if(recursionDepth >= 500)
    {
        SignalError("Excessive recusion depth -- top of stack is probably stuck");
    }
    if(decision.type == DecisionGameOver) return;

    //
    // Sometimes, resolving a decision will automatically create a new decision.
    //
    if(decision.type != DecisionNone)
    {
        //
        // If the decision has only one possible response, respond to it and keep looking for
        // a non-trivial decision.
        //
        if(decision.IsTrivial())
        {
            ProcessDecision(decision.TrivialResponse());
            AdvanceToNextDecision(recursionDepth + 1);
        }
        return;
    }

    if(stack.Length() == 0)
    {
        //
        // If the stack is empty, the basic rules of Dominion control what happens.
        //
        AdvancePhase();
    }
    else
    {
        //
        // If the stack is not empty, process the top element on the stack
        //
        UINT eventStackIndex = stack.Length() - 1;
        Event *curEvent = stack[eventStackIndex];

        //
        // If a reaction is played, we may need to skip processing this event until the reaction is resolved.
        //
        bool skipEventProcessing = false;
        bool lighthouseActive = false;
        if(curEvent->IsAttack()) // && data->reactionCards.Length() > 0) -> this optimization doesn't work when we're debugging because people can have cards not in the supply
        {
            Assert(curEvent->AttackedPlayer() != -1, "Invalid player");
            const PlayerState &p = players[curEvent->AttackedPlayer()];
            AttackAnnotations &annotations = *curEvent->Annotations();

            //
            // Moat
            //
            if(!annotations.moatProcessed && p.hand.Contains(data->baseCards.moat))
            {
                stack.PushEnd(new EventMoatReveal(data->baseCards.moat, curEvent->AttackedPlayer()));
                annotations.moatProcessed = true;
                skipEventProcessing = true;
            }

            //
            // Lighthouse
            //
            if(p.CardInPlay(data->baseCards.lighthouse))
            {
                if(logging) LogIndent(1, curEvent->AttackedPlayer(), " is protected from the attack by " + data->baseCards.lighthouse->PrettyName());
                Event *nextEvent = stack.Last();
                stack.PopEnd();
                delete nextEvent;
                skipEventProcessing = true;
            }
        }

        if(!skipEventProcessing)
        {
            bool eventCompleted = curEvent->Advance(*this);
            if(eventCompleted)
            {
                bool destroyNextEvent = lighthouseActive || curEvent->DestroyNextEventOnStack();
                stack.RemoveSlow(eventStackIndex);
                delete curEvent;

                //
                // Cards such as Moat and Lighthouse will also remove the next event on the stack (typically, an attack event)
                //
                if(destroyNextEvent)
                {
                    Event *nextEvent = stack.Last();
                    stack.PopEnd();
                    delete nextEvent;
                }
            }
        }
    }

    //
    // If we failed to find a decision, keep advancing the game state until we do.
    //
    if(decision.type == DecisionNone) AdvanceToNextDecision(recursionDepth + 1);

    //
    // By default, decisions are controlled by the current player.
    //
    if(decision.controllingPlayer == -1) decision.controllingPlayer = player;
    decision.maximumCards = Math::Min(decision.maximumCards, decision.cardChoices.Length());

    //
    // If the decision has only one possible response, respond to it and keep looking for
    // a non-trivial decision.
    //
    if(decision.IsTrivial())
    {
        ProcessDecision(decision.TrivialResponse());
        AdvanceToNextDecision(recursionDepth + 1);
    }

    Assert(decisionText || decision.text.Length() == 0, "Decision text is disabled!");

    //
    // Events and effects should never have no available options; they should never have
    // been put on the stack in the first place.
    //
    if(decision.type == DecisionSelectCards && decision.cardChoices.Length() == 0)
    {
        ofstream debugFile("debug.txt");
        debugFile << decision.activeCard->name << ' ' << decision.minimumCards << "," << decision.maximumCards << endl;
        debugFile.flush();
        debugFile.close();
        __asm int 3;
    }
}

void State::Log(const String &s)
{
    Assert(logging, "Logging is disabled!");
    data->log(data->players[player].name + " " + s);
}

void State::Log(UINT playerIndex, const String &s)
{
    Assert(logging, "Logging is disabled!");
    data->log(data->players[playerIndex].name + " " + s);
}

void State::LogIndent(UINT indentLevel, const String &s)
{
    Assert(logging, "Logging is disabled!");
    String preamble;
    for(UINT i = 0; i < indentLevel; i++) preamble += "  ";
    data->log(preamble + data->players[player].name + " " + s);
}

void State::LogIndent(UINT indentLevel, UINT playerIndex, const String &s)
{
    Assert(logging, "Logging is disabled!");
    String preamble;
    for(UINT i = 0; i < indentLevel; i++) preamble += "  ";
    data->log(preamble + data->players[playerIndex].name + " " + s);
}

int State::PlayerScore(UINT playerIndex) const
{
    const PlayerState &p = players[playerIndex];
    int sum = p.VPTokens;
    for(Card *c : p.hand)
    {
        sum += c->victoryPoints;
        if(c->isVictory && c->effect != NULL) sum += c->effect->VictoryPoints(*this, playerIndex);
    }
    for(Card *c : p.deck)
    {
        sum += c->victoryPoints;
        if(c->isVictory && c->effect != NULL) sum += c->effect->VictoryPoints(*this, playerIndex);
    }
    for(Card *c : p.discard)
    {
        sum += c->victoryPoints;
        if(c->isVictory && c->effect != NULL) sum += c->effect->VictoryPoints(*this, playerIndex);
    }
    for(Card *c : p.islandZone)
    {
        sum += c->victoryPoints;
        if(c->isVictory && c->effect != NULL) sum += c->effect->VictoryPoints(*this, playerIndex);
    }
    for(const CardPlayInfo &c : p.playArea)
    {
        sum += c.card->victoryPoints;
        if(c.card->isVictory && c.card->effect != NULL) sum += c.card->effect->VictoryPoints(*this, playerIndex);
    }
    return sum;
}

Vector<int> State::WinningPlayers() const
{
    Vector<int> result;
    double bestScore = -100000.0;
    for(const PlayerInfo &player : data->players)
    {
        int score = PlayerScore(player.index);
        double trueScore = score + 1.0 - players[player.index].turns * 0.0001;
        if(trueScore > bestScore)
        {
            bestScore = trueScore;
            result.Allocate(1, player.index);
        }
        else if(trueScore == bestScore)
        {
            result.PushEnd(player.index);
        }
    }
    return result;
}

void State::CheckEndConditions()
{
    if(players[player].turns >= 100 || EmptySupplyPiles() >= 3 || SupplyCount(data->baseCards.province) == 0 ||
        (data->supplyCards.Contains(data->baseCards.colony) && SupplyCount(data->baseCards.colony) == 0))
    {
        decision.type = DecisionGameOver;
        if(logging)
        {
            data->log("\n\n\n");

            double bestScore = -100000.0;
            for(const PlayerInfo &player : data->players)
            {
                data->log(player.name + ": " + String(PlayerScore(player.index)) + " VP");
            }

            for(int playerIndex : WinningPlayers())
            {
                data->log(data->players[playerIndex].name + " is the winner!!");
            }

            data->log("\n\n");
        }
    }
}

UINT State::EmptySupplyPiles() const
{
    UINT result = 0;
    for(UINT supplyIndex = 0; supplyIndex < data->supplyCards.Length(); supplyIndex++)
    {
        if(supply[supplyIndex].count == 0) result++;
    }
    return result;
}

UINT State::ActionsPlayedThisTurn() const
{
    int result = 0;
    const PlayerState &p = players[player];
    for(const CardPlayInfo &c : p.playArea)
    {
        if(c.card->isDuration)
        {
            if(c.turnsLeft == 1) result++;
        }
        if(c.card->isPermanent)
        {
            if(c.turnsLeft == 999) result++;
        }
        else result++;
    }
    return result;
}

void State::BuyPhaseStart()
{
    if(data->furnaceInSupply)
    {
        for(CardPlayInfo &c : players[player].playArea)
        {
            if(c.card == data->baseCards.furnace)
            {
                for(int copyIndex = 0; copyIndex < c.copies; copyIndex++) stack.PushEnd(new EventDiscardNCards(c.card, player, 0, 1));
            }
        }
    }
}
