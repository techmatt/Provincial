bool DialogInterface::GetSaveFilename(String &Result, const String &Title)
{
    Result.ReSize(1024);
    Result[0] = '\0';

    OPENFILENAME Info;
    Info.lStructSize = sizeof(OPENFILENAME);
    Info.hwndOwner = NULL;
    Info.hInstance = NULL;
    Info.lpstrFilter = NULL;
    Info.lpstrCustomFilter = NULL;
    Info.nMaxCustFilter = 0;
    Info.nFilterIndex = 0;
    Info.lpstrFile = Result.CString();
    Info.nMaxFile = 1023;
    Info.lpstrFileTitle = NULL;
    Info.nMaxFileTitle = 0;
    Info.lpstrInitialDir = NULL;
    Info.lpstrTitle = Title.CString();
    Info.Flags = 0;
    Info.nFileOffset = 0;
    Info.nFileExtension = 0;
    Info.lpstrDefExt = NULL;
    Info.lCustData = NULL;
    Info.lpfnHook = NULL;
    Info.lpTemplateName = NULL;
    Info.pvReserved = NULL;
    Info.dwReserved = NULL;
    Info.FlagsEx = 0;

    char SavedWorkingDirectory[512];
    GetCurrentDirectory(512, SavedWorkingDirectory);
    BOOL Return = GetSaveFileName(&Info);
    SetCurrentDirectory(SavedWorkingDirectory);
    return (Return != 0);
}

bool DialogInterface::GetOpenFilename(String &Result, const String &Title)
{
    Result.ReSize(1024);
    Result[0] = '\0';

    OPENFILENAME Info;
    Info.lStructSize = sizeof(OPENFILENAME);
    Info.hwndOwner = NULL;
    Info.hInstance = NULL;
    Info.lpstrFilter = NULL;
    Info.lpstrCustomFilter = NULL;
    Info.nMaxCustFilter = 0;
    Info.nFilterIndex = 0;
    Info.lpstrFile = Result.CString();
    Info.nMaxFile = 1023;
    Info.lpstrFileTitle = NULL;
    Info.nMaxFileTitle = 0;
    Info.lpstrInitialDir = NULL;
    Info.lpstrTitle = Title.CString();
    Info.Flags = 0;
    Info.nFileOffset = 0;
    Info.nFileExtension = 0;
    Info.lpstrDefExt = NULL;
    Info.lCustData = NULL;
    Info.lpfnHook = NULL;
    Info.lpTemplateName = NULL;
    Info.pvReserved = NULL;
    Info.dwReserved = NULL;
    Info.FlagsEx = 0;

    char SavedWorkingDirectory[512];
    GetCurrentDirectory(512, SavedWorkingDirectory);
    BOOL Return = GetOpenFileName(&Info);
    SetCurrentDirectory(SavedWorkingDirectory);
    return (Return != 0);
}
