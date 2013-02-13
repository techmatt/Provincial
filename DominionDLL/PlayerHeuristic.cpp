#include "Main.h"

PlayerHeuristic::PlayerHeuristic(const BuyAgenda *agenda)
{
    _agenda = agenda;
    _remodelGoldThreshold = 2;
}

void PlayerHeuristic::MakeDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    if(d.type != DecisionSelectCards && d.type != DecisionDiscreteChoice)
    {
        SignalError("Invalid decision type");
        return;
    }
    if(d.activeCard == NULL)
    {
        MakePhaseDecision(s, response);
    }
    else
    {
        if(s.stack.Length() > 0)
        {
            EventDiscardDownToN *discardDownEvent = dynamic_cast<EventDiscardDownToN*>(s.stack.Last());
            if(discardDownEvent != NULL)
            {
                MakeDiscardDownDecision(s, response);
                return;
            }

            EventDiscardNCards *discardNEvent = dynamic_cast<EventDiscardNCards*>(s.stack.Last());
            if(discardNEvent != NULL)
            {
                MakeDiscardDownDecision(s, response);
                return;
            }

            EventPutOnDeckDownToN *putOnTopEvent = dynamic_cast<EventPutOnDeckDownToN*>(s.stack.Last());
            if(putOnTopEvent != NULL)
            {
                MakeDiscardDownDecision(s, response);
                return;
            }

            EventReorderDeck *reorderEvent = dynamic_cast<EventReorderDeck*>(s.stack.Last());
            if(reorderEvent != NULL)
            {
                MakeDeckReorderDecision(s, response);
                return;
            }

            EventRemodelExpand* remodelEvent = dynamic_cast<EventRemodelExpand*>(s.stack.Last());
            if(remodelEvent != NULL)
            {
                if(remodelEvent->trashedCard == NULL)
                {
                    bool prosperity = s.data->options.prosperity;
                    bool remodelExpensiveThings = ((int)s.SupplyCount(s.data->baseCards.province) < 8) ||
                                                  (prosperity && (int)s.SupplyCount(s.data->baseCards.colony) < 8);
            
                    //
                    // Choosing a card to trash
                    //
                    auto scoringFunction = [remodelExpensiveThings,prosperity](Card *c)
                    {
                        if(remodelExpensiveThings)
                        {
                            if(!c->isVictory && c->cost >= 6) return 20.0 + c->cost;
                        }
                        if(c->name == "curse") return 19.0;
                        if(c->name == "estate") return 18.0;
                        if(c->isVictory) return -200.0 + c->cost;
                        return (double)-c->cost;
                    };
                    AIUtility::SelectCards(s, response, scoringFunction);
                }
                else
                {
                    //
                    // Choosing a card to gain
                    //
                    response.cards.PushEnd(_agenda->ForceBuy(s, s.decision.controllingPlayer, d.cardChoices));
                }
                return;
            }
        }
        
        if(d.activeCard->expansion == "base")
        {
            MakeBaseDecision(s, response);
        }
        else if(d.activeCard->expansion == "intrigue")
        {
            MakeIntrigueDecision(s, response);
        }
        else if(d.activeCard->expansion == "seaside")
        {
            MakeSeasideDecision(s, response);
        }
        else if(d.activeCard->expansion == "alchemy")
        {
            MakeAlchemyDecision(s, response);
        }
        else if(d.activeCard->expansion == "prosperity")
        {
            MakeProsperityDecision(s, response);
        }
        else if(d.activeCard->expansion == "custom")
        {
            MakeCustomDecision(s, response);
        }
        else
        {
            SignalError("Invalid expansion");
        }
    }
}

Vector<Card*> PlayerHeuristic::TrashableCards(const State &s) const
{
    const PlayerState &p = s.players[s.decision.controllingPlayer];
    Vector<Card*> result;
    bool trashEstates = !CardDesired(s, s.decision.controllingPlayer, s.data->baseCards.estate);
    for(Card *c : p.hand)
    {
        if(c->name == "curse" || c->name == "copper" || (trashEstates && c->name == "estate")) result.PushEnd(c);
    }
    return result;
}

UINT PlayerHeuristic::TrashableCardCount(const State &s) const
{
    const PlayerState &p = s.players[s.decision.controllingPlayer];
    UINT result = 0;
    bool trashEstates = !CardDesired(s, s.decision.controllingPlayer, s.data->baseCards.estate);
    for(Card *c : p.hand)
    {
        if(c->name == "curse" || c->name == "copper" || (trashEstates && c->name == "estate")) result++;
    }
    return result;
}

UINT PlayerHeuristic::DiscardableCardCount(const State &s) const
{
    const PlayerState &p = s.players[s.decision.controllingPlayer];
    UINT result = 0;
    for(Card *c : p.hand)
    {
        if(c->name == "curse" || c->name == "copper" || c->IsPureVictory()) result++;
    }
    return result;
}

bool PlayerHeuristic::CardDesired(const State &s, int player, Card *c) const
{
    Vector<Card*> choice(1, c);
    return (_agenda->Buy(s, player, choice) != NULL);
}

void PlayerHeuristic::MakePhaseDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(s.phase == PhaseAction)
    {
        auto scoringFunction = [this,&p,&s](Card *c) 
        {
            int score = c->priority + 2;
            if(c->name == "moneylender" && !p.hand.Contains(s.data->baseCards.copper)) score = -1;
            if(c->name == "treasure map" && p.hand.Count(s.data->baseCards.treasureMap) == 1) score = -1;
            
            if(c->name == "tactician")
            {
                if(p.hand.Length() == 2) score = 1000;
                else if(p.MoneyTotal() >= 6) score = -1;
            }

            if(c->name == "ambassador" && this->TrashableCardCount(s) == 0) score = -1;
            if(c->name == "trade route" && this->TrashableCardCount(s) == 0) score = -1;
            if(c->name == "bishop" && this->TrashableCardCount(s) == 0) score = -1;
            if(c->name == "trading post" && this->TrashableCardCount(s) <= 1) score = -1;

            if(c->name == "shanty town" && p.actions >= 2) score = 1;

            if(c->name == "smugglers" && s.prevGainList.Length() == 0) score = 2;

            if(c->name == "conspirator" && s.ActionsPlayedThisTurn() >= 2) score = s.data->baseCards.treasury->priority;

            return score;
        };
        response.singleCard = AIUtility::BestCard(d.cardChoices, scoringFunction, 0.0);
    }
    else if(s.phase == PhaseTreasure)
    {
        //
        // grand market complicates playing treasures.
        //
        if(s.data->grandMarketInSupply && s.SupplyCount(s.data->baseCards.grandMarket) > 0 && CardDesired(s, player, s.data->baseCards.grandMarket))
        {
            bool playCoppers = true;

            int buyingPower = p.MoneyTotal() - (int)p.hand.Count(s.data->baseCards.copper);
            if(s.data->options.prosperity)
            {
                if(buyingPower >= 6 && buyingPower < 9 && !CardDesired(s, player, s.data->baseCards.province)) playCoppers = false;
            }
            else
            {
                if(buyingPower >= 6 && buyingPower < 8) playCoppers = false;
            }

            if(p.CardInPlay(s.data->baseCards.copper)) playCoppers = true;

            if(playCoppers) response.singleCard = d.cardChoices[0];
            else
            {
                response.singleCard = NULL;
                for(Card *c : d.cardChoices) if(c != s.data->baseCards.copper) response.singleCard = c;
            }
        }
        else
        {
            response.singleCard = d.cardChoices[0];
        }
    }
    else if(s.phase == PhaseBuy)
    {
        response.singleCard = _agenda->Buy(s, player, d.cardChoices);
    }
    else
    {
        SignalError("Invalid phase");
    }
}

void PlayerHeuristic::MakeCopyDecision(const State &s, DecisionResponse &response)
{
    AIUtility::SelectCards(s, response, [](Card *c)
    {
        int p = c->priority;
        if(c->isDuration) p = 1000 + c->cost;
        if(c->isPermanent && c->cost >= 3) p = 1000 + c->cost;
        return p;
    } );
}

void PlayerHeuristic::MakeDiscardDownDecision(const State &s, DecisionResponse &response)
{
    auto scoringFunction = [](Card *c)
    {
        if(c->IsPureVictory()) return 20;
        if(c->name == "curse") return 19;
        if(c->name == "copper") return 18;
        return -100 - c->cost;
    };
    AIUtility::SelectCards(s, response, scoringFunction);
}

void PlayerHeuristic::MakeTrashDecision(const State &s, DecisionResponse &response)
{
    auto scoringFunction = [](Card *c)
    {
        if(c->name == "curse") return 20;
        if(c->name == "estate") return 19;
        if(c->name == "copper") return 18;
        if(c->isVictory) return -100 - c->cost;
        return -(c->cost);
    };
    AIUtility::SelectCards(s, response, scoringFunction);
}

void PlayerHeuristic::MakeDeckReorderDecision(const State &s, DecisionResponse &response)
{
    auto scoringFunction = [&s](Card *c)
    {
        if(c->isAction) return 100 + c->cost;
        if(c->isTreasure) return 50 + c->cost;
        return c->cost;
    };
    AIUtility::SelectCards(s, response, scoringFunction);
}

void PlayerHeuristic::MakeBaseDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    if(a.name == "cellar")
    {
        for(Card *c : d.cardChoices) if((c->IsPureVictory()) || c->cost <= 2) response.cards.PushEnd(c);
    }
    else if(a.name == "chapel")
    {
        CardCounter counter(s.players[player]);
        
        int treasureTotal = 0;
        for(auto &c : counter.counts) if(c.first->isTreasure) treasureTotal += c.first->treasure * c.second;

        bool trashCoppers = (treasureTotal >= 7);
        for(Card *c : d.cardChoices) if(c->name == "chapel" || (c->name == "estate" && !CardDesired(s, player, s.data->baseCards.estate)) || c->name == "curse" || (trashCoppers && c->name == "copper")) response.cards.PushEnd(c);
        if(response.cards.Length() > 4) response.cards.ReSize(4);
    }
    else if(a.name == "moat")
    {
        response.choice = 0;
    }
    else if(a.name == "chancellor")
    {
        response.choice = 0;
    }
    else if(a.name == "workshop")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "bureaucrat")
    {
        response.cards.PushEnd(d.cardChoices[0]);
    }
    else if(a.name == "feast")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "militia")
    {
        MakeDiscardDownDecision(s, response);
    }
    else if(a.name == "spy")
    {
        EventSpy* event = dynamic_cast<EventSpy*>(s.stack.Last());
        Card *revealedCard = s.players[event->targetedPlayer].deck.Last();
        response.choice = 1;
        if(revealedCard->IsPureVictory() || revealedCard->name == "copper" || revealedCard->name == "curse") response.choice = 0;
        if(event->targetedPlayer == player) response.choice = 1 - response.choice;
    }
    else if(a.name == "throne room")
    {
        MakeCopyDecision(s, response);
    }
    else if(a.name == "library")
    {
        if(s.players[s.player].actions == 0) response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "mine")
    {
        EventMine* event = dynamic_cast<EventMine*>(s.stack.Last());
        if(event->trashedCard == NULL)
        {
            //
            // Choosing a card to trash, favoring the most expensive treasure that can be upgraded.
            // If nothing can be upgraded, favor the cheapest treasure.
            //
            auto scoringFunction = [&s](Card *c)
            {
                if(c->name == "gold" && s.SupplyCount(s.data->baseCards.platinum) > 0) return 20.0;
                if(c->name == "silver" && s.SupplyCount(s.data->baseCards.gold) > 0) return 19.0;
                if(c->name == "copper" && s.SupplyCount(s.data->baseCards.silver) > 0) return 18.0;
                return (double)-c->cost;
            };
            AIUtility::SelectCards(s, response, scoringFunction);
        }
        else
        {
            //
            // Choosing a card to gain
            //
            response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
        }
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

void PlayerHeuristic::MakeIntrigueDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(a.name == "pawn")
    {
        if(p.ActionCount() >= 1) response.choice = 0; //+1 card, +1 action
        else response.choice = 2; //+1 card, +1 coin
    }
    else if(a.name == "courtyard")
    {
        auto scoringFunction = [&s,&p](Card *c)
        {
            if(c->isAction && p.actions == 0) return 4;
            if(c->isTreasure) return 2;
            if(c->isAction && p.actions >= 1) return 1;
            return 3;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "steward")
    {
        //"Choose one:|+2 Cards|+2 Coins|Trash 2 cards"
        EventSteward* event = dynamic_cast<EventSteward*>(s.stack.Last());
        if(event != NULL)
        {
            if(TrashableCardCount(s) >= 2) response.choice = 2;
            else if(p.actions >= 1) response.choice = 0;
            else response.choice = 1;
        }
        else
        {
            // Trash event
            MakeTrashDecision(s, response);
        }
    }
    else if(a.name == "wishing well")
    {
        CardCounter counter(p.deck);
        auto scoringFunction = [&counter](Card *c)
        {
            return counter.Count(c);
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "baron")
    {
        //"Discard an estate?|Yes|No"
        response.choice = 0;
    }
    else if(a.name == "nobles")
    {
        //"Choose one:|+3 Cards|+2 Actions"
        if(p.actions >= 1 || p.ActionCount() == 0) response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "trading post")
    {
        MakeTrashDecision(s, response);
    }
    else if(a.name == "ironworks")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "swindler")
    {
        bool giveVictoryCards = true;
        if(s.data->options.prosperity  && s.SupplyCount(s.data->baseCards.colony) >= 8) giveVictoryCards = true;
        if(!s.data->options.prosperity && s.SupplyCount(s.data->baseCards.province) >= 8) giveVictoryCards = true;
        auto scoringFunction = [giveVictoryCards](Card *c)
        {
            if(c->name == "curse") return 20;
            if(giveVictoryCards && c->IsPureVictory()) return 19;
            
            return -c->priority;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "minion")
    {
        //"Choose one:|+2 Coins|Discard your hand, +4 Cards"
        if(s.player != player) MakeDiscardDownDecision(s, response);
        else
        {
            if(p.MoneyTotal() >= 6) response.choice = 0;
            else if(p.ActionCount() >= 1) response.choice = 0;
            else response.choice = 1;
        }
    }
    else if(a.name == "saboteur")
    {
        Card *c = _agenda->Buy(s, player, d.cardChoices);
        if(c != NULL) response.cards.PushEnd(c);
    }
    else if(a.name == "torturer")
    {
        //"Choose one:|Discard 2 cards|Gain a curse in hand"
        if(s.SupplyCount(s.data->baseCards.curse) == 0) response.choice = 1;
        else if(DiscardableCardCount(s) >= 2) response.choice = 0;
        else if(p.MoneyTotal() >= 6) response.choice = 1;
        else if(p.hand.Length() <= 3 && p.ActionCount() >= 1) response.choice = 1;
        else response.choice = 0;
    }
    else if(a.name == "mining village")
    {
        //"Trash mining village?|Yes|No"
        int buyingPower = p.MoneyTotal();
        if(s.data->options.prosperity)
        {
            if(buyingPower == 9 || buyingPower == 7 || buyingPower == 4) response.choice = 0;
            else response.choice = 1;
        }
        else
        {
            if(buyingPower == 6 || buyingPower == 4) response.choice = 0;
            else response.choice = 1;
        }
    }
    else if(a.name == "upgrade")
    {
        EventUpgrade* event = dynamic_cast<EventUpgrade*>(s.stack.Last());
        if(event->trashedCard == NULL)
        {   
            //
            // Choosing a card to trash
            //
            bool buyingDuchies = CardDesired(s, player, s.data->baseCards.duchy);
            bool buyingEstates = CardDesired(s, player, s.data->baseCards.estate);
            auto scoringFunction = [buyingDuchies,buyingEstates](Card *c)
            {
                if(!buyingEstates && c->name == "estate") return 21.0;
                if(c->name == "curse") return 20.0;
                if(buyingDuchies && c->cost == 4) return 19.0;
                if(c->cost == 2) return 18.0;
                if(c->cost == 0) return 17.0;
                if(c->cost == 4) return 16.0;
                if(c->cost == 5) return 15.0;
                if(c->cost == 3) return 14.0;
                if(c->isVictory) return -100.0 - c->cost;

                return (double)-c->cost;
            };
            AIUtility::SelectCards(s, response, scoringFunction);
        }
        else
        {
            //
            // Choosing a card to gain
            //
            response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
        }
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

void PlayerHeuristic::MakeAlchemyDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(a.name == "pawn")
    {
        
    }
    else if(a.name == "courtyard")
    {
        
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

void PlayerHeuristic::MakeSeasideDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(a.name == "salvager")
    {
        auto scoringFunction = [](Card *c)
        {
            if(c->name == "estate") return 21.0;
            if(c->name == "curse") return 20.0;
            if(c->cost == 2) return 19.0;
            if(c->cost == 4) return 18.0;
            if(c->cost == 3) return 17.0;
            if(c->isVictory) return -100.0 - c->cost;

            return (double)-c->cost;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "lookout")
    {
        EventLookout* event = dynamic_cast<EventLookout*>(s.stack.Last());
        int stage = event->Stage();
        if(stage == 1)
        {
            MakeTrashDecision(s, response);
        }
        else if(stage == 2)
        {
            MakeDiscardDownDecision(s, response);
        }
    }
    else if(a.name == "pearl diver")
    {
        //"Put" + revealedCard->PrettyName() + "on top of your deck?|Yes|No"
        Card *c = p.deck.First();
        if((c->isAction || c->isTreasure) && c->name != "copper") response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "navigator")
    {
        //"Discard revealed cards?|Yes|No"
        int cardsToReveal = Math::Min((int)p.deck.Length(), 5);
        int handScore = 0;
        for(int cardIndex = 0; cardIndex < cardsToReveal; cardIndex++)
        {
            Card *c = p.deck[p.deck.Length() - 1 - cardIndex];
            if(c->isAttack) handScore++;
            if(c->isAction) handScore++;
            if(c->isTreasure && c->name != "copper") handScore++;
            if(c->isVictory && !c->isAction && !c->isTreasure) handScore--;
            if(c->name == "curse") handScore--;
        }

        if(handScore < 0) response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "ambassador")
    {
        if(d.type == DecisionSelectCards)
        {
            auto scoringFunction = [](Card *c)
            {
                if(c->name == "curse") return 21;
                if(c->name == "estate") return 20;
                if(c->name == "copper") return 19;
                if(c->isVictory) return -100 - c->cost;
                if(c->isTreasure) return -50 - c->cost;

                return -c->cost;
            };
            AIUtility::SelectCards(s, response, scoringFunction);
        }
        else
        {
            //
            // Always return as many cards as possible
            //
            response.choice = d.minimumCards - 1;
        }
    }
    else if(a.name == "smugglers")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "explorer")
    {
        //"Reveal a Province?|Yes|No"
        response.choice = 0;
    }
    else if(a.name == "island")
    {
        auto scoringFunction = [](Card *c)
        {
            if(c->isVictory && !c->isTreasure && !c->isAction && !c->isReaction) return 20;
            if(c->name == "copper") return 19;
            return -c->cost;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

void PlayerHeuristic::MakeProsperityDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(a.name == "king's court")
    {
        MakeCopyDecision(s, response);
    }
    else if(a.name == "mountebank")
    {
        response.choice = 0;
    }
    else if(a.name == "counting house")
    {
        response.choice = s.decision.minimumCards - 1;
    }
    else if(a.name == "loan")
    {
        //"Discard or trash " + revealedCard->PrettyName() + "?|Discard|Trash"
        EventLoan *loanEvent = dynamic_cast<EventLoan*>(s.stack.Last());
        if(loanEvent->treasureCard->cost > 0) response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "bishop")
    {
        bool estatesDresired = CardDesired(s, player, s.data->baseCards.estate);
        if(player == s.player)
        {
            auto scoringFunction = [estatesDresired](Card *c)
            {
                if(c->name == "curse") return 100;
                if(c->name == "estate" && !estatesDresired) return 20;
                if(estatesDresired && (c->cost == 2 || c->cost == 4 || c->cost == 5 || c->cost == 7)) return 20 + c->cost;
                if(c->name == "copper") return 19;
                return -(c->cost);
            };
            AIUtility::SelectCards(s, response, scoringFunction);
        }
        else
        {
            if(p.hand.Contains(s.data->baseCards.curse) || (!estatesDresired && p.hand.Contains(s.data->baseCards.estate)) || p.hand.Contains(s.data->baseCards.copper))
            {
                auto scoringFunction = [estatesDresired](Card *c)
                {
                    if(c->name == "curse") return 21;
                    if(c->name == "estate" && !estatesDresired) return 20;
                    if(c->name == "copper") return 19;
                    return -(c->cost);
                };
                AIUtility::SelectCards(s, response, scoringFunction);
            }
        }
    }
    else if(a.name == "royal seal")
    {
        //"Put " + card->PrettyName() + " on top of your deck?|Yes|No"
        EventGainCard *gainEvent = dynamic_cast<EventGainCard*>(s.stack.Last());
        if(gainEvent->card->cost > 0 && (gainEvent->card->isAction || gainEvent->card->isTreasure)) response.choice = 0;
        else response.choice = 1;
    }
    else if(a.name == "watchtower")
    {
        //"About to gain" + card->PrettyName() +":|Trash it|Put it on top of your deck|Don't reveal Watchtower"
        EventGainCard *gainEvent = dynamic_cast<EventGainCard*>(s.stack.Last());
        if(gainEvent->card->name == "copper" || gainEvent->card->name == "curse") response.choice = 0;
        else if(gainEvent->card->isAction || gainEvent->card->isTreasure) response.choice = 1;
        else response.choice = 2;
    }
    else if(a.name == "trade route")
    {
        MakeTrashDecision(s, response);
    }
    else if(a.name == "mint")
    {
        auto scoringFunction = [](Card *c)
        {
            return (c->cost);
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "vault")
    {
        if(s.decision.controllingPlayer == s.player)
        {
            for(Card *c : d.cardChoices) if((c->isVictory && !c->isAction && !c->isTreasure) || c->cost <= 2) response.cards.PushEnd(c);
        }
        else
        {
            Vector<Card*> priorityDiscards;
            for(Card *c : d.cardChoices) if(c->isVictory && !c->isAction && !c->isTreasure)
            {
                priorityDiscards.PushEnd(c);
                response.cards.PushEnd(c);
            }
            for(Card *c : d.cardChoices) if(c->cost <= 2)
            {
                if(!priorityDiscards.Contains(c)) response.cards.PushEnd(c);
            }
            if(response.cards.Length() >= 2) response.cards.ReSize(2);
            else response.cards.FreeMemory();
        }
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

void PlayerHeuristic::MakeCustomDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    Card &a = *d.activeCard;
    int player = s.decision.controllingPlayer;
    const PlayerState &p = s.players[player];
    if(a.name == "aqueduct")
    {
        EventAqueduct* event = dynamic_cast<EventAqueduct*>(s.stack.Last());
        if(event->trashedCard == NULL)
        {
            //
            // Choosing a card to trash, favoring the most expensive victory card that can be upgraded.
            // If nothing can be upgraded, favor the cheapest card.
            //
            auto scoringFunction = [&s](Card *c)
            {
                if(c->name == "province" && s.SupplyCount(s.data->baseCards.colony) > 0) return 20.0;
                if(c->name == "duchy" && s.SupplyCount(s.data->baseCards.province) > 0) return 19.0;
                if(c->name == "estate" && s.SupplyCount(s.data->baseCards.duchy) > 0) return 18.0;
                return (double)-c->cost;
            };
            AIUtility::SelectCards(s, response, scoringFunction);
        }
        else
        {
            //
            // Choosing a card to gain
            //
            response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
        }
    }
    else if(a.name == "architect")
    {
        auto scoringFunction = [&s,&p](Card *c)
        {
            if(c->isAction && p.actions == 0) return 4;
            if(c->isTreasure) return 2;
            if(c->isAction && p.actions >= 1) return 1;
            return 3;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "gambler")
    {
        MakeTrashDecision(s, response);
    }
    else if(a.name == "squire")
    {
        MakeTrashDecision(s, response);
    }
    else if(a.name == "aristocrat")
    {
        MakeDeckReorderDecision(s, response);
    }
    else if(a.name == "knight")
    {
        if(p.ActionCount() >= 2) response.choice = 0; //+1 card, +1 action
        else response.choice = 2; //+1 card, +1 coin
    }
    else if(a.name == "street urchin")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "trailblazer")
    {
        MakeCopyDecision(s, response);
    }
    else if(a.name == "witchdoctor")
    {
        MakeCopyDecision(s, response);
    }
    else if(a.name == "grand court")
    {
        MakeCopyDecision(s, response);
    }
    else if(a.name == "evangelist")
    {
        auto scoringFunction = [](Card *c)
        {
            if(c->name == "curse") return 20;
            if(c->name == "estate") return 19;
            if(c->name == "copper") return 18;
            return -1;
        };
        Card *c = AIUtility::BestCard(d.cardChoices, scoringFunction, 0.0);
        if(c != NULL) response.cards.PushEnd(c);
    }
    else if(a.name == "pillage")
    {
        response.cards.PushEnd(_agenda->ForceBuy(s, player, d.cardChoices));
    }
    else if(a.name == "sepulcher")
    {
        auto scoringFunction = [&s,&p](Card *c)
        {
            return c->cost;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else if(a.name == "furnace")
    {
        //
        // It probably doesn't matter which card we discard, since furnace occurs during the buy phase.
        //
        auto scoringFunction = [](Card *c)
        {
            if(c->name == "curse") return 5;
            if(c->isAction || c->IsPureVictory()) return 4;
            if(c->name == "copper") return 3;
            if(c->isTreasure) return 1;
            return 2;
        };
        AIUtility::SelectCards(s, response, scoringFunction);
    }
    else
    {
        SignalError("Unexpected decision");
    }
}

PlayerHeuristic* PlayerHeuristic::Mutate(const CardDatabase &cards, const GameOptions &options) const
{
    PlayerHeuristic *result = new PlayerHeuristic(_agenda->Mutate(cards, options));
    if(rnd() <= 0.2)
    {
        result->_remodelGoldThreshold = Utility::Bound(result->_remodelGoldThreshold + AIUtility::Delta(), 0, 12);
    }
    return result;
}
