#include "Main.h"

void MetaTestChamber::StrategizeStart(const CardDatabase &cards, const GameOptions &options, const String &directory, int chamberCount)
{
    Console::WriteLine("Strategizing for kingdom piles:");
    Console::WriteLine(options.ToString());

    Console::WriteLine("Using " + String(chamberCount) + " testing chambers");

    _chambers.Allocate(chamberCount);
    for(UINT chamberIndex = 0; chamberIndex < _chambers.Length(); chamberIndex++)
    {
        String chamberSuffix = "A";
        chamberSuffix[0] += chamberIndex;
        if(_chambers.Length() == 1) chamberSuffix = "";
        else chamberSuffix = "_" + chamberSuffix;
        _chambers[chamberIndex].StrategizeStart(cards, options, directory, chamberSuffix);
    }
}

void MetaTestChamber::StrategizeStep(const CardDatabase &cards)
{
    for(UINT chamberIndex = 0; chamberIndex < _chambers.Length(); chamberIndex++)
    {
        _chambers[chamberIndex].StrategizeStep(cards);
    }

    TestChamber &chamber = _chambers[0];
    if((chamber._generation - 1) % 4 == 0 && _chambers.Length() > 1)
    {
        Console::WriteLine("Generating inter-chamber leaderboard comparison for generation " + String(chamber._generation - 1));
        Vector<TestPlayer*> metaLeaders;
        for(TestChamber &c : _chambers) metaLeaders.PushEnd(c._leaders[0]);
        chamber.ComputeLeaderboard(cards, metaLeaders, chamber._directory + "leaderboard/" + String::ZeroPad(chamber._generation - 1, 3) + ".txt", 20000);
    }
}
