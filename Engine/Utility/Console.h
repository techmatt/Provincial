/*
Console.h
Written by Matthew Fisher

Functions for interacting with the console.
*/

namespace Console
{
    ofstream& File();
    void AdvanceLine();
    
    void WriteLine(const String &S);
    void WriteLine(const char *S);
    
    void OverwriteLine(const String &S);
    void OverwriteLine(const char *S);
    
    void WriteString(const String &S);
    void WriteString(const char *S);
}
