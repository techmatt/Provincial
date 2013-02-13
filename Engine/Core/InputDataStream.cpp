/*
InputDataStream.cpp
Written by Matthew Fisher
*/

InputDataStream::InputDataStream()
{
    _data = NULL;
}

InputDataStream::~InputDataStream()
{
    
}

void InputDataStream::LoadFromFile(const String &filename, bool expectBufferSizeHeader)
{
    if(expectBufferSizeHeader)
    {
        //
        // This used to assume the first 4 bytes was the size, which is unnecessary.
        // However this change may have broken some existing saves.
        //
        FILE *file = Utility::CheckedFOpen(filename.CString(), "rb");
        UINT length;
        Utility::CheckedFRead(&length, sizeof(UINT), 1, file);
        _storage.Allocate(length);
        Utility::CheckedFRead(_storage.CArray(), sizeof(BYTE), length, file);
        fclose(file);
    }
    else
    {
        Utility::GetFileData(filename, _storage);
    }
    
    _data = _storage.CArray();
    _dataLength = _storage.Length();
    _readPtr = 0;
}

void InputDataStream::LoadFromCompressed(const String &filename)
{
    Compression::DecompressStreamFromFile(filename, _storage);
    _data = _storage.CArray();
    _dataLength = _storage.Length();
    _readPtr = 0;
}

void InputDataStream::WrapMemory(const Vector<BYTE> &stream)
{
    _data = stream.CArray();
    _dataLength = stream.Length();
    _readPtr = 0;
}

void InputDataStream::ReadData(BYTE *result, UINT bytesToRead)
{
    Assert(_dataLength >= _readPtr + bytesToRead, "Read past end of stream");
    if(bytesToRead > 0)
    {
        memcpy(result, _data + _readPtr, bytesToRead);
        _readPtr += bytesToRead;
    }
}

InputDataStream& operator >> (InputDataStream &S, String &V)
{
    UINT Length;
    S >> Length;
    V.AllocateLength(Length);
    S.ReadData((BYTE *)V.CString(), Length);
    return S;
}
