class BuyAgenda
{
public:
    virtual Card* Buy(const State &s, UINT player, const Vector<Card*> &choices) const = 0;
    virtual Card* ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const = 0;
    virtual String Name() const = 0;
    virtual BuyAgenda* Mutate(const CardDatabase &cards, const GameOptions &options) const
    {
        SignalError("This BuyAgenda does not support mutation");
        return NULL;
    }
};

struct BuyMenuEntry
{
    BuyMenuEntry() {}
    BuyMenuEntry(Card *_c)
    {
        c = _c;
        count = 1;
    }
    BuyMenuEntry(Card *_c, int _count, int _minCost, int _maxCost)
    {
        c = _c;
        count = _count;
        minCost = _minCost;
        maxCost = _maxCost;
    }
    String ToString()
    {
        if(count == 0) return "";
        if(count == 1) return c->name;
        return c->name + "(" + String(count) + ")";
    }
    bool CardValidInSlot(Card *c) const
    {
        if(c->name == "peddler") return true;
        return (c->cost >= minCost && c->cost <= maxCost);
    }
    Card *c;
    int count;
    int minCost, maxCost;
};

struct BuyMenu
{
    void RecordItem(Card *c);
    void Cleanup();
    void Merge();
    void Bound();

    Vector<BuyMenuEntry> entries;
    int provinceBuyThreshold;
    int duchyBuyThreshold;
    int estateBuyThreshold;
};

class BuyAgendaMenu : public BuyAgenda
{
public:
    BuyAgendaMenu(const CardDatabase &cards, const String &s);
    BuyAgendaMenu(const CardDatabase &cards, const GameOptions &options);
    BuyAgendaMenu(const CardDatabase &cards, const GameOptions &options, Card *a, Card *b);
    BuyAgendaMenu(BuyMenu &m);
    Card* Buy(const State &s, UINT player, const Vector<Card*> &choices) const;
    Card* ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const;
    BuyAgenda* Mutate(const CardDatabase &cards, const GameOptions &options) const;
    String Name() const
    {
        String s = "e" + String(_m.estateBuyThreshold) + "-d" + String(_m.duchyBuyThreshold) + "-p" + String(_m.provinceBuyThreshold) + ":";
        for(BuyMenuEntry entry : _m.entries)
        {
            s += entry.ToString() + "-";
        }
        s.PopEnd();
        return s;
    }
    __forceinline const BuyMenu& GetMenu() const
    {
        return _m;
    }

private:
    void InitMenu(const CardDatabase &cards, const GameOptions &options);
    BuyMenu _m;
};

class BuyAgendaBigMoney : public BuyAgenda
{
public:
    Card* Buy(const State &s, UINT player, const Vector<Card*> &choices) const;
    Card* ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const;
    String Name() const
    {
        return "BigMoney";
    }
};

class BuyAgendaExpensiveNovelties : public BuyAgenda
{
public:
    Card* Buy(const State &s, UINT player, const Vector<Card*> &choices) const;
    Card* ForceBuy(const State &s, UINT player, const Vector<Card*> &choices) const;
    String Name() const
    {
        return "ExpensiveNovelties";
    }
};