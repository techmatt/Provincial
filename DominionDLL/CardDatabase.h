class CardEffect;
struct Card
{
    Card()
    {
        effect = NULL;
    }

    String PrettyName() const
    {
        String result = name;
        result[0] = toupper(name[0]);
        
        //
        // The extra spaces look better in a rich-text box.
        //
        return " " + result + " ";
    }

    bool IsPureVictory() const
    {
        return (isVictory && !isTreasure && !isAction && !isDuration);
    }

    String name;
    int cost;

    int supply;
    
    //
    // Type(s)
    //
    bool isVictory;
    bool isTreasure;
    bool isAction;
    bool isReaction;
    bool isDuration;
    bool isAttack;
    bool isPermanent;

    //
    // Action
    //
    int actions;
    int cards;
    int buys;
    int money;
    int victoryTokens;
    
    //
    // Victory or curse
    //
    int victoryPoints;

    //
    // Treasure
    //
    int treasure;

    CardEffect *effect;

    //
    // Used by AIs
    //
    String expansion;
    int priority;
    int strength;
};

class CardDatabase
{
public:
    void Init(bool useCustomCards);

    Card* GetCard(const String &s) const;
    Card* RandomSupplyCard() const;

private:
    void RegisterCardEffects(bool useCustomCards);
    void RegisterCard(const String &cardName, CardEffect *effect);
    void LoadCardList(const String &filename, const String &expansion);
    bool IsSupplyCard(const Card *c) const;
    void LoadPriorityList();
    void LoadStrengthList();

    map<String, Card*> _cards;
    Vector<Card*> _cardList;
    Vector<String> _skipList;
};
