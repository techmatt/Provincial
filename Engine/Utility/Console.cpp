/*
Console.cpp
Written by Matthew Fisher

Functions for interacting with the console.
*/

namespace Console
{
#ifdef SUPPRESS_CONSOLE
    ofstream ConsoleFile;
#else
    ofstream ConsoleFile("Console.txt");
#endif

    ofstream& File()
    {
        return ConsoleFile;
    }

    void AdvanceLine()
    {
        WriteLine("");
    }

    void WriteLine(const String &S)
    {
        HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD BytesWritten;
        WriteConsole(StdHandle, S.CString(), S.Length(), &BytesWritten, NULL);
        WriteConsole(StdHandle, "\n", 1, &BytesWritten, NULL);
        ConsoleFile << S << endl;
        ConsoleFile.flush();
    }

    void OverwriteLine(const String &S)
    {
        const UINT ConsoleWidth = 79;
        HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO CursorInfo;
        GetConsoleScreenBufferInfo(StdHandle, &CursorInfo);
        CursorInfo.dwCursorPosition.X = 0;
        CursorInfo.dwCursorPosition.Y = Math::Max(CursorInfo.dwCursorPosition.Y - 1, 0);
        SetConsoleCursorPosition(StdHandle, CursorInfo.dwCursorPosition);
        DWORD BytesWritten;
        String FinalString = S;
        while(FinalString.Length() < ConsoleWidth)
        {
            FinalString.PushEnd(' ');
        }
        FinalString.PushEnd('\n');
        WriteConsole(StdHandle, FinalString.CString(), FinalString.Length(), &BytesWritten, NULL);
        ConsoleFile << S << endl;
        ConsoleFile.flush();
    }

    void WriteString(const String &S)
    {
        HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD BytesWritten;
        WriteConsole(StdHandle, S.CString(), S.Length(), &BytesWritten, NULL);
        ConsoleFile << S;
        ConsoleFile.flush();
    }

    void WriteLine(const char *S)
    {
        HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD BytesWritten;
        WriteConsole(StdHandle, S, DWORD(strlen(S)), &BytesWritten, NULL);
        WriteConsole(StdHandle, "\n", 1, &BytesWritten, NULL);
        ConsoleFile << S << endl;
        ConsoleFile.flush();
    }

    void OverwriteLine(const char *S)
    {
        OverwriteLine(String(S));
    }

    void WriteString(const char *S)
    {
        HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD BytesWritten;
        WriteConsole(StdHandle, S, DWORD(strlen(S)), &BytesWritten, NULL);
        ConsoleFile << S;
        ConsoleFile.flush();
    }
}