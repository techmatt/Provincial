#include "Main.h"

bool EventTrashCardFromHand::Advance(State &s)
{
    int handIndex = s.players[player].hand.FindFirstIndex(card);
    if(handIndex == -1)
    {
        if(logging) s.Log(player, "fails to trash " + card->name);
    }
    else
    {
        if(logging) s.Log(player, "trashes " + card->PrettyName());
        s.players[player].hand.RemoveSwap(handIndex);

        //
        // Normally, this is where we would add the card to the trash. However, I have not yet decided to add the trash to the state,
        // although I will need to for expansions like Dark Ages.
        //
    }
    return true;
}

bool EventTrashCardFromPlay::Advance(State &s)
{
    int playAreaIndex = -1;
    const Vector<CardPlayInfo> &playArea = s.players[player].playArea;
    for(UINT searchIndex = 0; searchIndex < playArea.Length(); searchIndex++) if(playArea[searchIndex].card == card) playAreaIndex = searchIndex;
    if(playAreaIndex == -1)
    {
        if(logging) s.Log(player, "fails to trash " + card->name);
    }
    else
    {
        if(logging) s.Log(player, "trashes " + card->PrettyName());
        s.players[player].playArea.RemoveSwap(playAreaIndex);
    }
    return true;
}

bool EventDiscardCard::Advance(State &s)
{
    int handIndex = s.players[player].hand.FindFirstIndex(card);
    if(zone == DiscardFromHand && handIndex == -1)
    {
        if(logging) s.Log("fails to discard " + card->name);
    }
    else
    {
        if(logging) s.Log(player, + "discards " + card->PrettyName());
        if(zone == DiscardFromHand)
        {
            s.players[player].hand.RemoveSwap(handIndex);
        }
        s.players[player].discard.PushEnd(card);
    }
    return true;
}

bool EventDrawCard::Advance(State &s)
{
    s.DrawCard(player);
    return true;
}
