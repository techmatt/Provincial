/*
DialogBoxes.h
Written by Matthew Fisher
*/

class DialogBoxes
{
public:
    static bool Load(String &Result, const String &FileTypeDescription, const String &FileTypeExtension);
    static bool Load(String &Result, const String &Filter);
};