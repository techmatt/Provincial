/*
OutputDataStream.cpp
Written by Matthew Fisher
*/

OutputDataStream::OutputDataStream()
{
    
}

OutputDataStream::~OutputDataStream()
{
    
}

void OutputDataStream::SaveToFile(const String &Filename)
{
    FILE *File = Utility::CheckedFOpen(Filename.CString(), "wb");
    
    const UINT MyLength = _Data.Length();

    //
    // This used to save the size of a file as the first four bytes,
    // but this is unncessary for uncompressed streams.
    //
    //Utility::CheckedFWrite(&MyLength, sizeof(UINT), 1, File);
    //

    Utility::CheckedFWrite(_Data.CArray(), sizeof(BYTE), MyLength, File);
    fclose(File);
}

void OutputDataStream::SaveToCompressedFile(const String &Filename)
{
    Compression::CompressStreamToFile(_Data, Filename);
}

void OutputDataStream::SaveToFileNoHeader(const String &Filename)
{
    FILE *File = Utility::CheckedFOpen(Filename.CString(), "wb");
    Utility::CheckedFWrite(_Data.CArray(), sizeof(BYTE), _Data.Length(), File);
    fclose(File);
}

void OutputDataStream::WriteData(const BYTE *Data, UINT BytesToWrite)
{
    if(BytesToWrite > 0)
    {
        UINT StartLength = _Data.Length();
        _Data.ReSize(StartLength + BytesToWrite);
        memcpy(_Data.CArray() + StartLength, Data, BytesToWrite);
    }
}

OutputDataStream& operator << (OutputDataStream &S, const String &V)
{
    UINT Length = V.Length();
    S << Length;
    S.WriteData((const BYTE *)V.CString(), Length);
    return S;
}
