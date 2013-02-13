/*
InputDataStream.h
Written by Matthew Fisher

InputDataStream class.  An istream but for binary data.
*/

#pragma once

class InputDataStream
{
public:
    InputDataStream();
    ~InputDataStream();
    void FreeMemory();

    //
    // Reads stream from file
    //
    void LoadFromFile(const String &filename, bool expectBufferSizeHeader = false);
    void LoadFromCompressed(const String &filename);
    void WrapMemory(const Vector<BYTE> &stream);

    //
    // Reads data represented by T as binary data from the stream
    //
    template<class type> void ReadData(type &t)
    {
        const UINT byteCount = sizeof(t);
        BYTE *tOffset = (BYTE *)&t;
        const BYTE *streamOffset = _data + _readPtr;
        for(UINT byteIndex = 0; byteIndex < byteCount; byteIndex++)
        {
            *tOffset = *streamOffset;
            streamOffset++;
            tOffset++;
            _readPtr++;
        }
    }

    //
    // Reads raw binary data from the stream
    //
    void ReadData(BYTE *Data, UINT BytesToRead);

    template<class type> void ReadSimpleVector(Vector<type> &v)
    {
        UINT length;
        *this >> length;
        v.Allocate(length);
        if(length > 0)
        {
            ReadData((BYTE *)v.CArray(), length * sizeof(type));
        }
    }

    __forceinline UINT BytesLeft() const
    {
        return _dataLength - _readPtr;
    }

protected:
    UINT _readPtr;
    const BYTE *_data;
    UINT _dataLength;
    Vector<BYTE> _storage;
};

//
// Input for several basic types
//

template<class A, class B>
__forceinline InputDataStream& operator >> (InputDataStream &S, pair<A,B> &P)
{
    S >> P.first;
    S >> P.second;
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, BYTE &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, UINT &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, RGBColor &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, int &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, unsigned short &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, float &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, double &A)
{
    S.ReadData(A);
    return S;
}

__forceinline InputDataStream& operator >> (InputDataStream &S, UINT64 &A)
{
    S.ReadData(A);
    return S;
}

template<class type> InputDataStream& operator >> (InputDataStream &S, Vector<type> &V)
{
    UINT Length;
    S >> Length;
    V.Allocate(Length);
    for(UINT Index = 0; Index < Length; Index++)
    {
        S >> V[Index];
    }
    return S;
}

#ifdef __MULTIGRID_H
template<class type> InputDataStream& operator >> (InputDataStream &S, MultiGrid<type> &G)
{
    Vector<UINT> dimensions;
    S >> dimensions;
    G.Allocate(dimensions);
    type *data = G.CArray();
    const UINT totalEntries = G.TotalEntries();
    for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        S >> data[entryIndex];
    }
    return S;
}
#endif

InputDataStream& operator >> (InputDataStream &S, String &V);