class CardEffect
{
public:
    virtual void PlayAction(State &s) const {}
    virtual bool CanProcessDecisions() const
    {
        return false;
    }
    virtual void ProcessDecision(State &s, const DecisionResponse &response) const
    {
        SignalError("Card does not support decisions");
    }
    virtual void ProcessDuration(State &s) const {}
    virtual int VictoryPoints(const State &s, UINT playerIndex) const
    {
        return 0;
    }
    Card *c;
};
