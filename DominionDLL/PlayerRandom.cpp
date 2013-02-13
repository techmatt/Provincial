#include "Main.h"

void PlayerRandom::MakeDecision(const State &s, DecisionResponse &response)
{
    const DecisionState &d = s.decision;
    if(d.type == DecisionSelectCards)
    {
        int cardsToPick = d.minimumCards;
        if(d.maximumCards > d.minimumCards) cardsToPick = rand() % (d.maximumCards - d.minimumCards + 1) + d.minimumCards;
        Vector<int> responseIndices;
        for(int responseIndex = 0; responseIndex < cardsToPick; responseIndex++)
        {
            int choice = -1;
            do { choice = rand() % d.cardChoices.Length(); }
            while(responseIndices.Contains(choice));
            responseIndices.PushEnd(choice);
            response.cards.PushEnd(d.cardChoices[choice]);
        }
    }
    else if(d.type == DecisionDiscreteChoice)
    {
        response.choice = rand() % d.minimumCards;
    }
    else
    {
        SignalError("Invalid decision type");
    }
}
