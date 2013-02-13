/*
FileCollection.h
Written by Matthew Fisher

A FileCollection stores a large number of files as a single file.  It is similar to a tar file in functionality.
*/

struct FileCollectionFile
{
    void GetFileLines(Vector<String> &Lines);
    String Filename;
    Vector<BYTE> Data;
};

class FileCollection
{
public:
    FileCollection();
    ~FileCollection();
    void FreeMemory();

    //
    // Save/Load collection to disk
    //
    void LoadCompressed(const String &Filename);
    void SaveCompressed(const String &Filename) const;
    void DumpCollectionToDisk();

    //
    // Modify collection
    //
    void AddFileFromMemory(const String &FileCollectionName, const Vector<BYTE> &Data);
    void AddFileFromDisk(const String &FileCollectionName, const String &ExistingFilename);
    
    void RemoveFile(const String &FileCollectionName);
    
    FileCollectionFile* AddAndUpdateFile(const String &FileCollectionName, const String &ExistingFilename);
    
    FileCollectionFile*       FindFile(const String &FileCollectionName);
    const FileCollectionFile* FindFile(const String &FileCollectionName) const;
    
    void GetFileLines(const String &Filename, Vector<String> &Lines);

    __forceinline UINT FileCount() const
    {
        return _FileList.Length();
    }
    __forceinline const FileCollectionFile& GetFile(UINT Index) const
    {
        return *(_FileList[Index]);
    }

private:
    void MuddleData(Vector<BYTE> &Data);

    mutable Mutex _FileListMutex;
    Vector<FileCollectionFile *> _FileList;
    map<String, FileCollectionFile *, String::LexicographicComparison> _FileMap;
};
