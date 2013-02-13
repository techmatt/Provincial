/*
OutputDataStream.h
Written by Matthew Fisher

OutputDataStream class.  An ostream but for binary data.
*/

#pragma once

class OutputDataStream
{
public:
    OutputDataStream();
    ~OutputDataStream();
    void FreeMemory();

    //
    // Saves stream to a file
    //
    void SaveToFile(const String &Filename);
    void SaveToCompressedFile(const String &Filename);
    void SaveToFileNoHeader(const String &Filename);

    //
    // Writes data represented by T as binary data to the stream
    //
    template<class type> __forceinline void WriteData(const type &T)
    {
        const UINT ByteCount = sizeof(T);
        const UINT StartLength = _Data.Length();
        _Data.ReSize(StartLength + ByteCount);
        BYTE *StreamOffset = _Data.CArray() + StartLength;
        const BYTE *TOffset = (const BYTE *)&T;
        for(UINT ByteIndex = 0; ByteIndex < ByteCount; ByteIndex++)
        {
            *StreamOffset = *TOffset;
            StreamOffset++;
            TOffset++;
        }
    }

    //
    // Writes raw binary data to the stream
    //
    void WriteData(const BYTE *Data, UINT BytesToWrite);
    
    template<class type> void WriteSimpleVector(const Vector<type> &v)
    {
        const UINT length = v.Length();
        *this << length;
        if(length > 0)
        {
            WriteData((BYTE *)v.CArray(), length * sizeof(type));
        }
    }

    __forceinline void InlineUnicodeString(const UnicodeString &S)
    {
        WriteData((const BYTE *)S.CString(), S.Length() * sizeof(UnicodeCharacter));
    }

    __forceinline const Vector<BYTE>& Data() const
    {
        return _Data;
    }

protected:
    Vector<BYTE> _Data;
};

//
// Output for several basic types
//
template<class A, class B>
__forceinline OutputDataStream& operator << (OutputDataStream &S, const pair<A,B> &P)
{
    S << P.first;
    S << P.second;
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, BYTE A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, UINT A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, int A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, float A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, double A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, char A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, RGBColor A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, unsigned short A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, UnicodeCharacter A)
{
    S.WriteData(A);
    return S;
}

__forceinline OutputDataStream& operator << (OutputDataStream &S, UINT64 A)
{
    S.WriteData(A);
    return S;
}

template<class type> OutputDataStream& operator << (OutputDataStream &S, const Vector<type> &V)
{
    UINT Length = V.Length();
    S << Length;
    for(UINT Index = 0; Index < Length; Index++)
    {
        S << V[Index];
    }
    return S;
}

#ifdef __MULTIGRID_H
template<class type> OutputDataStream& operator << (OutputDataStream &S, const MultiGrid<type> &G)
{
    S << G.Dimensions();
    const type *data = G.CArray();
    const UINT totalEntries = G.TotalEntries();
    for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        S << data[entryIndex];
    }
    return S;
}
#endif

OutputDataStream& operator << (OutputDataStream &S, const String &V);