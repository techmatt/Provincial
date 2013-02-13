
class MetaTestChamber
{
public:
    void StrategizeStart(const CardDatabase &cards, const GameOptions &options, const String &directory, int chamberCount);
    void StrategizeStep(const CardDatabase &cards);

private:

    Vector<TestChamber> _chambers;
};
