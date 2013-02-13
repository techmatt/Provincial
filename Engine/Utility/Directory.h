/*
Directory.h
Written by Matthew Fisher
*/

class Directory
{
public:
    Directory() {}
    Directory(const String &DirectoryPath);
    void Load(const String &DirectoryPath);

    __forceinline const String& DirectoryPath() const
    {
        return _DirectoryPath;
    }
    __forceinline const Vector<String>& Files() const
    {
        return _Files;
    }
    __forceinline const Vector<String>& Directories() const
    {
        return _Directories;
    }
    Vector<String> FilesWithSuffix(const String &suffix) const;

private:
    String _DirectoryPath;
    Vector<String> _Files;
    Vector<String> _Directories;
};

