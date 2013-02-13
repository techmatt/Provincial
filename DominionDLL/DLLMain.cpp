// dllmain.cpp : Defines the entry point for the DLL application.
#include "Main.h"

#ifdef _DLL
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpszCmdLine, int nCmdShow)
{
    Utility::MessageBox("App Start");
    return 0;
}

int main(int argc, char *argv[])
{
    App *app = new App;
    app->Init();

    srand(timeGetTime());

    bool useTournamentFile = true;
    String directory;
    int generationCount = 64;
    int chamberCount = 3;

    BYTE bytes[16];
    for(UINT byteIndex = 0; byteIndex < 16; byteIndex++) bytes[byteIndex] = rand() % 255;

    String kingdomPileHash = String::ByteStreamToHexString(bytes, 16);

    if(useTournamentFile)
    {
        Vector<String> lines = Utility::GetFileLines("TournamentParameters.txt");
        if(lines[0] != "random") app->ProcessCommand("newKingdomCards@" + lines[0]);

        if(lines[1] == "random")
        {
            Utility::MakeDirectory("kingdomsIntermediate");

            directory = "kingdomsIntermediate/auto_" + kingdomPileHash + "/";

            Utility::MakeDirectory(directory);
            //Utility::MakeDirectory(directory + "counters/");
            Utility::MakeDirectory(directory + "generations/");
            Utility::MakeDirectory(directory + "leaderboard/");
            Utility::MakeDirectory(directory + "progression/");
        }
        else
        {
            directory = lines[1];
        }

        generationCount = lines[2].RemovePrefix("generations=").ConvertToInteger();
        chamberCount = lines[3].RemovePrefix("chambers=").ConvertToInteger();
    }
    else
    {
        directory = "kingdomsIntermediate/auto_" + kingdomPileHash + "/";

        Utility::MakeDirectory(directory);
        //Utility::MakeDirectory(directory + "counters/");
        Utility::MakeDirectory(directory + "generations/");
        Utility::MakeDirectory(directory + "leaderboard/");
        Utility::MakeDirectory(directory + "progression/");
    }

    app->ProcessCommand("trainAIStart@" + directory + "@" + String(chamberCount));

    for (int generationIndex = 0; generationIndex < generationCount; generationIndex++)
    {
        app->ProcessCommand("trainAIStep@" + directory);
    }

    return 0;
}
