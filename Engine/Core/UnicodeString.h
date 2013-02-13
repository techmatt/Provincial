/*
UnicodeString.h
Written by Matthew Fisher

String class and related conversions.
*/

#pragma once

//
// Intellisense for custom types
//
//http://thejefffiles.com/blog/autoexpdat-your-key-to-better-debugging-part-1/

typedef wchar_t UnicodeCharacter;

const UnicodeCharacter UnicodeNullTerminator = 0;
const UnicodeCharacter UnicodeHeader = 0xFEFF;
const UnicodeCharacter UnicodeNewline = 0x000D;
const UnicodeCharacter UnicodeSpace = 0x0020;
        

class UnicodeString
{
public:
    
    struct LexicographicComparison
    {
        bool operator()(const UnicodeString &L, const UnicodeString &R) const
        {
            SignalError("Not implemented");
        }
    };

    UnicodeString()
    {
        _Data = NULL;
        _Capacity = 0;
        _Length = 0;
    }

    ~UnicodeString()
    {
        if(_Data != NULL)
        {
            delete[] _Data;
        }
    }

    UnicodeString(const UnicodeString &S)
    {
        if(S._Data == NULL)
        {
            _Data = NULL;
            _Capacity = 0;
            _Length = 0;
        }
        else
        {
            _Length = S._Length;
            const UINT NewCapacity = _Length + 1;
            _Capacity = NewCapacity;
            _Data = new UnicodeCharacter[NewCapacity];
            memcpy(_Data, S._Data, NewCapacity * sizeof(UnicodeCharacter));
        }
    }
    UnicodeString(const UnicodeCharacter *Text)
    {
        _Data = NULL;
        _Capacity = 0;
        _Length = 0;
        *this = Text;
    }
    explicit UnicodeString(UnicodeCharacter C)
    {
        _Data = NULL;
        _Capacity = 0;
        _Length = 0;
        *this = C;
    }
    explicit UnicodeString(const Vector<UnicodeCharacter> &S)
    {
        _Length = S.Length();
        _Capacity = _Length + 1;
        _Data = new UnicodeCharacter[_Capacity];
        memcpy(_Data, S.CArray(), _Length * sizeof(UnicodeCharacter));
        _Data[_Length] = UnicodeNullTerminator;
    }
    explicit UnicodeString(const String &S)
    {
        _Length = S.Length();
        _Capacity = _Length + 1;
        _Data = new UnicodeCharacter[_Capacity];
        for(UINT Index = 0; Index < _Length; Index++)
        {
            _Data[Index] = S[Index];
        }
        _Data[_Length] = UnicodeNullTerminator;
    }
    
    //
    // Memory
    //
    __forceinline void FreeMemory()
    {
        if(_Data != NULL)
        {
            delete[] _Data;
            _Data = NULL;
        }
        _Length = 0;
        _Capacity = 0;
    }

    __forceinline void Allocate(UINT Capacity)
    {
        if(_Data != NULL)
        {
            delete[] _Data;
        }
        _Data = new UnicodeCharacter[Capacity];
        _Data[0] = UnicodeNullTerminator;
        _Length = 0;
        _Capacity = Capacity;
    }

    __forceinline void ReSize(UINT NewLength)
    {
        const UINT NewCapacity = NewLength + 1;
        _Length = Math::Min(_Length, NewLength);
        UnicodeCharacter *NewData = new UnicodeCharacter[NewCapacity];
        if(_Length > 0)
        {
            memcpy(NewData, _Data, _Length * sizeof(UnicodeCharacter));
        }
        NewData[_Length] = UnicodeNullTerminator;
        if(_Data != NULL)
        {
            delete[] _Data;
        }
        _Data = NewData;
        _Capacity = NewCapacity;
    }

    //
    // Accessors
    //
    __forceinline UnicodeCharacter* CString()
    {
        if(_Data != NULL)
        {
            return _Data;
        }
        else
        {
            return (UnicodeCharacter *)&(_Data);
        }
    }

    __forceinline const UnicodeCharacter* CString() const
    {
        if(_Data != NULL)
        {
            return _Data;
        }
        else
        {
            return (UnicodeCharacter *)&(_Data);
        }
    }

    __forceinline UINT Length() const
    {
        return _Length;
    }

    __forceinline UnicodeCharacter Last() const
    {
#ifdef VECTOR_DEBUG
        if(_Length == 0)
        {
            SignalError("Last called on zero-length string");
        }
#endif
        return _Data[_Length - 1];
    }

    __forceinline UnicodeCharacter& operator [] (UINT k)
    {
#ifdef VECTOR_DEBUG
        if(k >= _Length)
        {
            SignalError("Out-of-bounds string access");
        }
#endif
        return _Data[k];
    }

    __forceinline UnicodeCharacter& operator [] (int k) 
    {
#ifdef VECTOR_DEBUG
        if(k < 0 || k >= int(_Length))
        {
            SignalError("Out-of-bounds string access");
        }
#endif
        return _Data[k];
    }

    __forceinline const UnicodeCharacter& operator [] (UINT k) const
    {
#ifdef VECTOR_DEBUG
        if(k >= _Length)
        {
            SignalError("Out-of-bounds string access");
        }
#endif
        return _Data[k];
    }

    __forceinline const UnicodeCharacter& operator [] (int k) const
    {
#ifdef VECTOR_DEBUG
        if(k < 0 || k >= int(_Length))
        {
            SignalError("Out-of-bounds string access");
        }
#endif
        return _Data[k];
    }

    //
    // Assignment
    //
    UnicodeString& operator = (UnicodeCharacter Character);
    UnicodeString& operator = (const UnicodeCharacter *S);
    UnicodeString& operator = (const UnicodeString &S);

    //
    // Query
    //
    bool ExactMatchAtOffset(const UnicodeString &Find, UINT Offset) const;
    bool Contains(const UnicodeString &Find) const;
    bool IsNumeric() const;
    bool IsSuffix(const UnicodeString &EndCanidate) const;
    bool IsPrefix(const UnicodeString &StartCanidate) const;
    void Partition(UnicodeCharacter Seperator, Vector<UnicodeString> &Output) const;
    Vector<UnicodeString> Partition(UnicodeCharacter Seperator) const;
    void PartitionAboutIndex(UINT Index, UnicodeString &Left, UnicodeString &Right) const;
    void Partition(const UnicodeString &Seperator, Vector<UnicodeString> &Output) const;
    int FindFirstIndex(UnicodeCharacter Seperator) const;
    int FindLastIndex(UnicodeCharacter Seperator) const;
    UnicodeString FindAndReplace(const UnicodeString &Find, const UnicodeString &Replace) const;
    UnicodeString RemoveSuffix(const UnicodeString &EndCandidate) const;
    UINT Hash() const;
    
    //
    // Modifiers
    //
    UnicodeString& operator += (const UnicodeString &S);
    __forceinline void operator += (UnicodeCharacter C)
    {
        PushEnd(C);
    }

    __forceinline void PushEnd(UnicodeCharacter C)
    {
        if(_Length + 1 >= _Capacity)
        {
            ReSize(_Capacity * 2 + 3);
        }
        _Data[_Length] = C;
        _Length++;
        _Data[_Length] = UnicodeNullTerminator;
    }

    __forceinline void PopEnd()
    {
#ifdef VECTOR_DEBUG
        if(_Length == 0)
        {
            SignalError("Pop called on empty string");
        }
#endif
        _Length--;
        _Data[_Length] = UnicodeNullTerminator;
    }

    void PopFront()
    {
#ifdef VECTOR_DEBUG
        if(_Length == 0)
        {
            SignalError("Pop called on empty string");
        }
#endif
        _Length--;
        for(UINT Index = 0; Index < _Length; Index++)
        {
            _Data[Index] = _Data[Index + 1];
        }
    }

private:
    void ResizeToCStringLength();
    friend UnicodeString operator + (const UnicodeString &L, const UnicodeString &R);

    UnicodeCharacter *_Data;
    UINT _Capacity;
    UINT _Length;
};

//
// String Comparison
//
bool operator == (const UnicodeString &L, const UnicodeString &R);
bool operator == (const UnicodeCharacter *L, const UnicodeString &R);
bool operator == (const UnicodeString &R, const UnicodeCharacter *L);
__forceinline bool operator != (const UnicodeString &L, const UnicodeString &R) {return !(L == R);}
__forceinline bool operator != (const UnicodeCharacter *L, const UnicodeString &R) {return !(L == R);}
__forceinline bool operator != (const UnicodeString &R, const UnicodeCharacter *L) {return !(L == R);}

//
// String Operations
//
UnicodeString operator + (const UnicodeString &L, const UnicodeString &R);
ostream& operator << (ostream &os, const UnicodeString &S);

