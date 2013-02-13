/*
FileCollection.cpp
Written by Matthew Fisher

A FileCollection stores a large number of files as a single file.  It is similar to a tar file in functionality.
*/

void FileCollectionFile::GetFileLines(Vector<String> &Lines)
{
    FILE *TempFile = Utility::CheckedFOpen("Temp.txt", "wb");
    Utility::CheckedFWrite(Data.CArray(), sizeof(BYTE), Data.Length(), TempFile);
    fclose(TempFile);

    Utility::GetFileLines("Temp.txt", Lines);

    ofstream File("Temp.txt");
    File.close();
}

FileCollection::FileCollection()
{
    
}

FileCollection::~FileCollection()
{
    FreeMemory();
}

void FileCollection::FreeMemory()
{
    _FileListMutex.Acquire();
    for(UINT FileIndex = 0; FileIndex < _FileList.Length(); FileIndex++)
    {
        delete _FileList[FileIndex];
    }
    _FileList.FreeMemory();
    _FileMap.clear();
    _FileListMutex.Release();
}

void FileCollection::MuddleData(Vector<BYTE> &Data)
{
    BYTE *DataArray = Data.CArray();
    UINT DataLength = Data.Length();
    for(UINT Index = 0; Index < DataLength; Index++)
    {
        DataArray[Index] = 255 - DataArray[Index];
    }
}

void FileCollection::LoadCompressed(const String &filename)
{
    _FileListMutex.Acquire();

    InputDataStream stream;
    stream.LoadFromCompressed(filename);
    
    UINT fileCount = 0;
    stream >> fileCount;

    _FileList.Allocate(fileCount);
    
    for(UINT fileIndex = 0; fileIndex < _FileList.Length(); fileIndex++)
    {
        FileCollectionFile *newFile = new FileCollectionFile;
        stream >> newFile->Filename;
        stream.ReadSimpleVector(newFile->Data);
            
        _FileList[fileIndex] = newFile;
        _FileMap[newFile->Filename] = newFile;
    }

    _FileListMutex.Release();
}

void FileCollection::SaveCompressed(const String &filename) const
{
    _FileListMutex.Acquire();

    OutputDataStream stream;
    
    stream << _FileList.Length();
    
    for(UINT fileIndex = 0; fileIndex < _FileList.Length(); fileIndex++)
    {
        FileCollectionFile &curFile = *(_FileList[fileIndex]);

        stream << curFile.Filename;
        stream.WriteSimpleVector(curFile.Data);
    }

    Compression::CompressStreamToFile(stream.Data(), filename);
    
    _FileListMutex.Release();
}

void FileCollection::DumpCollectionToDisk()
{
    _FileListMutex.Acquire();
    for(UINT FileIndex = 0; FileIndex < _FileList.Length(); FileIndex++)
    {
        const FileCollectionFile &CurFile = *(_FileList[FileIndex]);
        FILE *File = Utility::CheckedFOpen(CurFile.Filename.CString(), "wb");
        Utility::CheckedFWrite(CurFile.Data.CArray(), sizeof(BYTE), CurFile.Data.Length(), File);
        fclose(File);
    }
    _FileListMutex.Release();
}

void FileCollection::RemoveFile(const String &FileCollectionName)
{
    _FileListMutex.Acquire();
    for(UINT FileIndex = 0; FileIndex < _FileList.Length(); FileIndex++)
    {
        FileCollectionFile *CurFile = _FileList[FileIndex];
        if(CurFile->Filename == FileCollectionName)
        {
            delete CurFile;
            _FileList.RemoveSwap(FileIndex);
            PersistentAssert(_FileMap.find(FileCollectionName.CString()) != _FileMap.end(), "File not found in map");
            _FileMap.erase(_FileMap.find(FileCollectionName.CString()));
            _FileListMutex.Release();
            return;
        }
    }
    _FileListMutex.Release();
}

void FileCollection::AddFileFromDisk(const String &FileCollectionName, const String &ExistingFilename)
{
    _FileListMutex.Acquire();
    Vector<BYTE> Data;
    RemoveFile(FileCollectionName);
    if(!Utility::FileExists(ExistingFilename))
    {
        SignalError(String("Required file not found: ") + FileCollectionName);
    }
    Utility::GetFileData(ExistingFilename, Data);
    AddFileFromMemory(FileCollectionName, Data);
    _FileListMutex.Release();
}

void FileCollection::AddFileFromMemory(const String &FileCollectionName, const Vector<BYTE> &Data)
{
    _FileListMutex.Acquire();
    RemoveFile(FileCollectionName);
    FileCollectionFile *NewFile = new FileCollectionFile;
    NewFile->Filename = FileCollectionName;
    NewFile->Data = Data;
    _FileList.PushEnd(NewFile);
    _FileMap[NewFile->Filename.CString()] = NewFile;
    _FileListMutex.Release();
}

FileCollectionFile* FileCollection::FindFile(const String &FileCollectionName)
{
    _FileListMutex.Acquire();
    FileCollectionFile *Result = NULL;

    map<String, FileCollectionFile *, String::LexicographicComparison>::const_iterator FileSearchResult = _FileMap.find(FileCollectionName.CString());
    if(FileSearchResult != _FileMap.end())
    {
        Result = FileSearchResult->second;
    }
    _FileListMutex.Release();
    return Result;
}

const FileCollectionFile* FileCollection::FindFile(const String &FileCollectionName) const
{
    _FileListMutex.Acquire();
    FileCollectionFile *Result = NULL;

    map<String, FileCollectionFile *, String::LexicographicComparison>::const_iterator FileSearchResult = _FileMap.find(FileCollectionName.CString());
    if(FileSearchResult != _FileMap.end())
    {
        Result = FileSearchResult->second;
    }
    _FileListMutex.Release();
    return Result;
}

FileCollectionFile* FileCollection::AddAndUpdateFile(const String &FileCollectionName, const String &ExistingFilename)
{
    if(Utility::FileExists(ExistingFilename))
    {
        AddFileFromDisk(FileCollectionName, ExistingFilename);
    }
    return FindFile(FileCollectionName);
}

void FileCollection::GetFileLines(const String &FileCollectionName, Vector<String> &Lines)
{
    _FileListMutex.Acquire();
    
    FileCollectionFile* CurFile = FindFile(FileCollectionName);
    PersistentAssert(CurFile != NULL, String("Failed to find ") + String(FileCollectionName) + " in database.");
    CurFile->GetFileLines(Lines);

    _FileListMutex.Release();
}
