/*
ColorGenerator.cpp
Written by Matthew Fisher
*/

bool DialogBoxes::Load(String &Result, const String &FileTypeDescription, const String &FileTypeExtension)
{
    String Filter = FileTypeDescription;
    Filter.PushEnd('\0');
    Filter += "*.";
    Filter += FileTypeExtension;
    Filter.PushEnd('\0');
    return Load(Result, Filter);
}

bool DialogBoxes::Load(String &Result, const String &Filter)
{
    OPENFILENAME ofn;
    char Filename[512];
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = Filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(Filename);
    ofn.lpstrFilter = Filter.CString();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if(GetOpenFileName(&ofn) == TRUE)
    {
        Result = Filename;
        return true;
    }
    else
    {
        return false;
    }
}
