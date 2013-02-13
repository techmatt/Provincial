#include "Main.h"

#ifndef LOGGING_CONSTANT
bool logging = true;
bool decisionText = true;
#endif

void DominionGame::Init(const CardDatabase &cards)
{
    _data.Init(cards);
}

void DominionGame::NewGame(const Vector<PlayerInfo> &playerList, const GameOptions &options)
{
    _data.NewGame(playerList, options);
    _state.NewGame(_data);
}
