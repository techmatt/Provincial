class Player
{
public:
    virtual void MakeDecision(const State &s, DecisionResponse &response) = 0;
    virtual String ControllerName() = 0;
};

class PlayerHuman : public Player
{
public:
    void MakeDecision(const State &s, DecisionResponse &response)
    {
        SignalError("Human decisions are made from the host C# application!");
    }
    String ControllerName() { return "Human"; }
};

class PlayerRandom : public Player
{
public:
    void MakeDecision(const State &s, DecisionResponse &response);
    String ControllerName() { return "Random AI"; }
};

class PlayerHeuristic : public Player
{
public:
    PlayerHeuristic(const BuyAgenda *agenda);
    void MakeDecision(const State &s, DecisionResponse &response);
    String ControllerName() { return "Heuristic " + _agenda->Name(); }
    const BuyAgenda& Agenda() const
    {
        return *_agenda;
    }

    PlayerHeuristic* Mutate(const CardDatabase &cards, const GameOptions &options) const;

private:
    bool CardDesired(const State &s, int player, Card *c) const;
    void MakePhaseDecision(const State &s, DecisionResponse &response);
    void MakeBaseDecision(const State &s, DecisionResponse &response);
    void MakeIntrigueDecision(const State &s, DecisionResponse &response);
    void MakeSeasideDecision(const State &s, DecisionResponse &response);
    void MakeAlchemyDecision(const State &s, DecisionResponse &response);
    void MakeProsperityDecision(const State &s, DecisionResponse &response);
    void MakeCustomDecision(const State &s, DecisionResponse &response);

    void MakeCopyDecision(const State &s, DecisionResponse &response);
    void MakeDiscardDownDecision(const State &s, DecisionResponse &response);
    void MakeDeckReorderDecision(const State &s, DecisionResponse &response);
    void MakeTrashDecision(const State &s, DecisionResponse &response);
    
    Vector<Card*> TrashableCards(const State &s) const;
    UINT TrashableCardCount(const State &s) const;
    UINT DiscardableCardCount(const State &s) const;

    const BuyAgenda *_agenda;
    int _remodelGoldThreshold;
};

class PlayerSmart : public Player
{
public:
    void MakeDecision(const State &s, DecisionResponse &response);
    String ControllerName() { return "Smart AI"; }
};