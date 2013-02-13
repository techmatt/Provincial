#include "Main.h"

void App::Init()
{
    //AllocConsole();
    srand(timeGetTime());
    _parameters.Init("data/parameters.txt");
    _cards.Init(_parameters.useCustomCards);
    _game.Init(_cards);
    _AITestResult = -1.0;
}

UINT32 App::ProcessCommand(const String &command)
{
    Vector<String> parameters = command.Partition("@");
    if(parameters[0] == "newKingdomCards")
    {
        _options = GameOptions();
        if(parameters.Length() == 1)
        {
            _options.RandomizeSupplyPiles(_cards);
        }
        else
        {
            _options.SetupGame(_cards, parameters[1]);
        }

        Vector<PlayerInfo> playerList;
        playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHuman));
        //playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHeuristic(new BuyAgendaExpensiveNovelties)));
        playerList.PushEnd(PlayerInfo(1, "Beth", new PlayerHeuristic(new BuyAgendaExpensiveNovelties)));

        logging = true;
        decisionText = true;
        _game.NewGame(playerList, _options);
        AIUtility::AdvanceAIs(_game);
    }
    else if(parameters[0] == "newGame")
    {
        if(_options.supplyPiles.Length() == 0)
        {
            _options.RandomizeSupplyPiles(_cards);
        }
        Vector<PlayerInfo> playerList;
        if(parameters.Length() == 1)
        {
            playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHuman));
            //playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHeuristic(new BuyAgendaExpensiveNovelties)));
            playerList.PushEnd(PlayerInfo(1, "Beth", new PlayerHeuristic(new BuyAgendaExpensiveNovelties)));
        }
        else
        {
            if(parameters[1] == "Human") playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHuman));
            else playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHeuristic(new BuyAgendaMenu(_cards, parameters[1].FindAndReplace("~","@")))));

            if(parameters[2] == "Human") playerList.PushEnd(PlayerInfo(1, "Beth", new PlayerHuman));
            else playerList.PushEnd(PlayerInfo(1, "Beth", new PlayerHeuristic(new BuyAgendaMenu(_cards, parameters[2].FindAndReplace("~","@")))));
        }

        logging = true;
        decisionText = true;
        _game.NewGame(playerList, _options);
        AIUtility::AdvanceAIs(_game);
    }
    else if(parameters[0] == "testAIs")
    {
        Vector<PlayerInfo> playerList;

        playerList.PushEnd(PlayerInfo(0, "Adam", new PlayerHeuristic(new BuyAgendaMenu(_cards, parameters[1].FindAndReplace("~","@")))));
        playerList.PushEnd(PlayerInfo(1, "Beth", new PlayerHeuristic(new BuyAgendaMenu(_cards, parameters[2].FindAndReplace("~","@")))));

        logging = true;
        decisionText = true;
        _game.NewGame(playerList, _options);
        AIUtility::AdvanceAIs(_game);

        TestChamber chamber;
        TestParameters params;
        params.minGameCount = 10000;
        params.maxGameCount = 10000;
        params.players[0] = playerList[0].controller;
        params.players[1] = playerList[1].controller;
        params.options = _game.data().options;
        TestResult result = chamber.Test(_cards, params);
        _AITestResult = result.winRatio[0];
    }
    else if(parameters[0] == "response")
    {
        auto &d = _game.state().decision;
        if(d.type == DecisionGameOver || d.type == DecisionNone)
        {
            SignalError("response when no decision ready");
            return 1;
        }
        else if(d.type == DecisionSelectCards)
        {
            Vector<String> cards;
            if(parameters.Length() > 1) cards = parameters[1].Partition("|");
            if(cards.Length() < d.minimumCards || cards.Length() > d.maximumCards)
            {
                SignalError("response with invalid number of cards");
                return 1;
            }

            DecisionResponse response;
            for(const String &s : cards)
            {
                response.cards.PushEnd(_game.data().cards->GetCard(s));
            }
            _game.state().ProcessDecision(response);
            _game.state().AdvanceToNextDecision(0);
        }
        else if(d.type == DecisionDiscreteChoice)
        {
            int choice = parameters[1].ConvertToInteger();
            if(choice < 0 || choice >= int(d.minimumCards))
            {
                SignalError("response with invalid choice");
                return 1;
            }

            DecisionResponse response;
            response.choice = choice;

            _game.state().ProcessDecision(response);
            _game.state().AdvanceToNextDecision(0);
        }
        AIUtility::AdvanceAIs(_game);
    }
    else if(parameters[0] == "debugAddCard")
    {
        Card *c = _game.data().cards->GetCard(parameters[1]);
        if(c == NULL) return 1;
        _game.state().players[_game.state().player].hand.PushEnd(c);
        _game.state().players[_game.state().player].actions++;
    }
    else if(parameters[0] == "trainAIStart")
    {
        GameOptions options;
        if(_game.data().options.supplyPiles.Length() == 0) options.RandomizeSupplyPiles(_cards);
        else options = _game.data().options;
        _chamber.StrategizeStart(_cards, options, parameters[1], parameters[2].ConvertToInteger());
    }
    else if(parameters[0] == "trainAIStep")
    {
        _chamber.StrategizeStep(_cards);
    }
    return 0;
}

int App::QueryIntegerByName(const String &s)
{
    if(s == "layerCount")
    {
        return 0;
    }
    else
    {
        SignalError("Unknown integer");
        return -1;
    }
}

double App::QueryDoubleByName(const String &s)
{
    if(s == "AITestResult")
    {
        return _AITestResult;
    }
    else
    {
        SignalError("Unknown double");
        return -1.0;
    }
}

const char* App::QueryStringByName(const String &s)
{
    _queryString.FreeMemory();

    const State &state = _game.state();
    const GameData &data = _game.data();

    //
    // Return nothing if a game is not active.
    //
    if(data.supplyCards.Length() == 0) return _queryString.CString();

    if(s.StartsWith("playerList"))
    {
        for(auto &p : data.players) _queryString += p.name + "@" + p.controller->ControllerName() + "|";
    }
    else if(s.StartsWith("supplyCards"))
    {
        for(auto &s : data.supplyCards) _queryString += s->name + "|";
    }
    else if(s.StartsWith("supplyState"))
    {
        for(UINT supplyIndex = 0; supplyIndex < data.supplyCards.Length(); supplyIndex++)
        {
            _queryString += String(state.supply[supplyIndex].count) + "|";
        }
        _queryString[_queryString.Length() - 1] = '@';
        for(UINT supplyIndex = 0; supplyIndex < data.supplyCards.Length(); supplyIndex++)
        {
            _queryString += String(state.SupplyCost(supplyIndex)) + "|";       
        }
    }
    else if(s.StartsWith("basicState"))
    {
        _queryString = String(state.player) + "|" +
            data.players[state.player].name + "|" +
            String(state.phase) + "|" +
            String(state.players[state.player].actions) + "|" + 
            String(state.players[state.player].buys) + "|" + 
            String(state.players[state.player].money);
    }
    else if(s.StartsWith("hand"))
    {
        int playerIndex = state.player;
        if(s.Contains(" ")) playerIndex = s.Partition(" ")[1].ConvertToInteger();
        auto &player = state.players[playerIndex];
        for(auto &c : player.hand) _queryString += String(c->name) + "|";
    }
    else if(s.StartsWith("playArea"))
    {
        int playerIndex = state.player;
        if(s.Contains(" ")) playerIndex = s.Partition(" ")[1].ConvertToInteger();
        auto &player = state.players[playerIndex];
        for(auto &c : player.playArea) _queryString += String(c.card->name) + "|";
    }
    else if(s.StartsWith("discard"))
    {
        int playerIndex = state.player;
        if(s.Contains(" ")) playerIndex = s.Partition(" ")[1].ConvertToInteger();
        auto &player = state.players[playerIndex];
        for(auto &c : player.discard) _queryString += String(c->name) + "|";
    }
    else if(s.StartsWith("deck"))
    {
        int playerIndex = state.player;
        if(s.Contains(" ")) playerIndex = s.Partition(" ")[1].ConvertToInteger();
        auto &player = state.players[playerIndex];
        for(auto &c : player.deck) _queryString += String(c->name) + "|";
    }
    else if(s.StartsWith("victoryPoints"))
    {
        int playerIndex = state.player;
        if(s.Contains(" ")) playerIndex = s.Partition(" ")[1].ConvertToInteger();
        _queryString += String(state.PlayerScore(playerIndex));
    }
    else if(s.StartsWith("log"))
    {
        for(const String &s : _game.data().log.Events())
        {
            _queryString += s;
            _queryString.PushEnd('\n');
        }
        if(_queryString.EndsWith("\n")) _queryString.PopEnd();
        _game.data().log.Reset();
    }
    else if(s.StartsWith("decision"))
    {
        auto &d = state.decision;
        String cardString = "phase|" + String(state.phase);
        if(d.activeCard != NULL) cardString = d.activeCard->name;
        if(d.type == DecisionType::DecisionNone)
        {
            _queryString = "none";
        }
        if(d.type == DecisionType::DecisionGameOver)
        {
            _queryString = "gameover";
        }
        if(d.type == DecisionType::DecisionDiscreteChoice)
        {
            _queryString = "choice@" + String(d.controllingPlayer) + "@" + data.players[d.controllingPlayer].name + "@" + d.text + "@" + cardString;
        }
        if(d.type == DecisionType::DecisionSelectCards)
        {
            _queryString = "selectCard@" + String(d.controllingPlayer) + "@" + data.players[d.controllingPlayer].name + "@" + d.text +
                                     "@" + String(d.minimumCards) + "@" + String(d.maximumCards) + "@" + cardString + "@";
            for(auto &c : d.cardChoices)
            {
                _queryString += c->name + "|";
            }
        }
    }
    else if(s.StartsWith("kingdomDescription"))
    {
        _queryString = _options.ToString();
    }
    else
    {
        return NULL;
    }
    if(_queryString.Length() > 0 && _queryString.Last() == '|') _queryString.PopEnd();
    return _queryString.CString();
}
