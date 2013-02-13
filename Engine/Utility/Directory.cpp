/*
Directory.cpp
Written by Matthew Fisher
*/

Directory::Directory(const String &DirectoryPath)
{
    Load(DirectoryPath);
}

void Directory::Load(const String &DirectoryPath)
{
    _DirectoryPath = DirectoryPath + "\\";
    _Files.FreeMemory();
    _Directories.FreeMemory();

    WIN32_FIND_DATA FindResult;
    //LARGE_INTEGER FileSize;
    
    HANDLE hFind = FindFirstFile((DirectoryPath + String("\\*")).CString(), &FindResult);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    } 

    do
    {
        if (FindResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            String DirectoryName(FindResult.cFileName);
            if(DirectoryName != String(".") &&
               DirectoryName != String("..") &&
               DirectoryName != String(".svn"))
            {
                _Directories.PushEnd(DirectoryName);
            }
        }
        else
        {
            //FileSize.LowPart = FindResult.nFileSizeLow;
            //FileSize.HighPart = FindResult.nFileSizeHigh;
            _Files.PushEnd(String(FindResult.cFileName));
        }
    }
    while (FindNextFile(hFind, &FindResult) != 0);

    FindClose(hFind);
}

Vector<String> Directory::FilesWithSuffix(const String &suffix) const
{
    Vector<String> result;
    for(UINT fileIndex = 0; fileIndex < _Files.Length(); fileIndex++)
    {
        const String &curFilename = _Files[fileIndex];
        if(curFilename.EndsWith(suffix))
        {
            result.PushEnd(curFilename);
        }
    }
    return result;
}
