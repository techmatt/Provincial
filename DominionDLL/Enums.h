
enum Phase
{
    PhaseAction,
    PhaseTreasure,
    PhaseBuy,
    PhaseCleanup
};

enum DecisionType
{
    DecisionNone,
    DecisionSelectCards,
    DecisionDiscreteChoice,
    DecisionGameOver,
};

enum GainZone
{
    GainToDiscard,
    GainToDiscardIronworks,
    GainToHand,
    GainToDeckTop,
    GainToTrash,
};

enum DiscardZone
{
    DiscardFromHand,
    DiscardFromSideZone,
};

enum StartingCondition
{
    StartingCondition34Split,
    StartingCondition25Split,
    StartingConditionRandom,
};

enum CardFilter
{
    FilterAny,
    FilterTreasure,
};