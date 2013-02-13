#include "Main.h"

void AIUtility::SelectCards(const State &s, DecisionResponse &response, const function<double(Card*)> &scoringFunction)
{
    Vector<Card*> v = s.decision.cardChoices;
    v.Randomize();
    v.Sort([scoringFunction](Card *a, Card *b)
    {
        return (scoringFunction(a) > scoringFunction(b));
    });
    for(int responseIndex = 0; responseIndex < Math::Max(1, int(s.decision.minimumCards)); responseIndex++)
    {
        response.cards.PushEnd(v[responseIndex]);
    }
}

Card* AIUtility::BestCard(const Vector<Card*> &choices, const function<double(Card*)> &scoringFunction, double minScore)
{
    Vector<Card*> v = choices;
    v.Randomize();
    v.Sort([scoringFunction](Card *a, Card *b)
    {
        return (scoringFunction(a) >= scoringFunction(b));
    });
    if(scoringFunction(v[0]) < minScore) return NULL;
    return v[0];
}

void AIUtility::AdvanceAIs(DominionGame &game)
{
    auto &d = game.state().decision;
    int decisionIndex = 0;
    while(1)
    {
        if(d.type == DecisionNone || d.type == DecisionGameOver) return;

        const auto &p = game.data().players[d.controllingPlayer];
        if(dynamic_cast<PlayerHuman*>(p.controller) != NULL) return;

        DecisionResponse response;
        p.controller->MakeDecision(game.state(), response);

        game.state().ProcessDecision(response);
        game.state().AdvanceToNextDecision(0);
    }
}

PlayerHeuristic* AIUtility::MakeTwoCardPlayer(const CardDatabase &cards, const String &cardA, const String &cardB)
{
    Card *a = NULL, *b = NULL;
    if(cardA.Length() > 0) a = cards.GetCard(cardA);
    if(cardB.Length() > 0) b = cards.GetCard(cardB);
    return MakeTwoCardPlayer(cards, a, b);
}

PlayerHeuristic* AIUtility::MakeTwoCardPlayer(const CardDatabase &cards, Card *a, Card *b)
{
    BuyMenu menu;
    menu.duchyBuyThreshold = 3;
    menu.estateBuyThreshold = 1;

    //
    // provinces are instantly bought, and adding them only increases the number of possible mutations.
    //
    //menu.entries.PushEnd(BuyMenuEntry(cards.GetCard("province"), 99));
    menu.entries.PushEnd(BuyMenuEntry(cards.GetCard("gold"), 99, 6, 6));
    if(a != NULL) menu.entries.PushEnd(BuyMenuEntry(a, 1, 0, 11));
    if(b != NULL) menu.entries.PushEnd(BuyMenuEntry(b, 1, 0, 11));
    menu.entries.PushEnd(BuyMenuEntry(cards.GetCard("silver"), 99, 3, 3));

    return new PlayerHeuristic(new BuyAgendaMenu(menu));
}

CardCounter::CardCounter(const PlayerState &p)
{
    for(Card *c : p.hand           ) AddCard(c);
    for(Card *c : p.deck           ) AddCard(c);
    for(Card *c : p.discard        ) AddCard(c);
    for(CardPlayInfo c : p.playArea) AddCard(c.card);
}

CardCounter::CardCounter(const Vector<Card*> &cardList)
{
    for(Card *c : cardList) AddCard(c);
}