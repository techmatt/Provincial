/*
Vector.h
Written by Matthew Fisher

Template vector library.  Extends functionality of std::vector
*/

#pragma once

#include "Asserts.h"

//
// Intellisense for custom types
//
//http://thejefffiles.com/blog/autoexpdat-your-key-to-better-debugging-part-1/

#pragma warning(disable : 4700)

template <class T> class Vector
{
public:
    Vector()
    {
        _Data = NULL;
        _Length = 0;
        _Capacity = 0;
    }

    explicit Vector(unsigned int Size)
    {
        _Data = new T[Size];
        _Length = Size;
        _Capacity = Size;
    }

    Vector(unsigned int Size, const T &ClearValue)
    {
        _Data = new T[Size];
        _Length = Size;
        _Capacity = Size;
        Clear(ClearValue);
    }

    Vector(const Vector<T> &V)
    {
        const unsigned int Length = V._Length;
        _Length = Length;
        _Capacity = Length;
        if(Length == 0)
        {
            _Data = NULL;
        }
        else
        {
            _Data = new T[Length];
            for(unsigned int Index = 0; Index < Length; Index++)
            {
                _Data[Index] = V._Data[Index];
            }
        }
    }

    Vector(Vector<T> &&V)
    {
        _Length = V._Length;
        _Capacity = V._Capacity;
        _Data = V._Data;
        V._Length = 0;
        V._Capacity = 0;
        V._Data = NULL;

        //
        // Alternatively, we could invoke the move constructor
        //
        // *this = std::move(V);
    }

    ~Vector()
    {
        if(_Data != NULL)
        {
            delete[] _Data;
        }
    }
    
    //
    // Memory
    //
    void FreeMemory()
    {
        if(_Data != NULL)
        {
            delete[] _Data;
            _Data = NULL;
        }
        _Length = 0;
        _Capacity = 0;
    }

    void DeleteMemory()
    {
        if(_Data != NULL)
        {
            for(UINT Index = 0; Index < _Length; Index++)
            {
                delete _Data[Index];
            }
            delete[] _Data;
            _Data = NULL;
        }
        _Length = 0;
        _Capacity = 0;
    }

    //
    // Windows has claimed the nice name "ZeroMemory" for its own uses
    //
    void ZeroMem()
    {
        if(_Length > 0)
        {
            memset(_Data, 0, _Length * sizeof(T));
        }
    }

    void operator = (const Vector<T> &V)
    {
        const unsigned int Length = V._Length;
        _Length = Length;
        _Capacity = Length;
        if(_Data != NULL)
        {
            delete[] _Data;
        }
        if(Length == 0)
        {
            _Data = NULL;
        }
        else
        {
            _Data = new T[Length];
            for(unsigned int Index = 0; Index < Length; Index++)
            {
                _Data[Index] = V._Data[Index];
            }
        }
    }

    void operator=(Vector<T>&& V)
    {
        if(this != &V)
        {
            if(_Data != NULL)
            {
                delete[] _Data;
            }
            _Length = V._Length;
            _Capacity = V._Capacity;
            _Data = V._Data;
            V._Length = 0;
            V._Capacity = 0;
            V._Data = NULL;
        }
    }

    void Allocate(unsigned int Size)
    {
        if(Size == 0)
        {
            FreeMemory();
        }
        /*else if(_Length == Size)
        {
            if(typeid(T) != typeid(int) &&
               typeid(T) != typeid(unsigned int) &&
               typeid(T) != typeid(double) &&
               typeid(T) != typeid(float) &&
               typeid(T) != typeid(char) &&
               typeid(T) != typeid(unsigned char))
            {
                T Value;
                for(unsigned int Index = 0; Index < Size; Index++)
                {
                    _Data[Index] = Value;
                }
            }
        }*/
        else
        {
            if(_Data != NULL)
            {
                delete[] _Data;
            }
            _Data = new T[Size];
            _Length = Size;
            _Capacity = Size;
        }
    }

    void Allocate(unsigned int Size, const T &ClearValue)
    {
        Allocate(Size);
        Clear(ClearValue);
    }

    void ReSize(unsigned int Size)
    {
        if(Size == 0)
        {
            FreeMemory();
        }
        else if(Size <= _Capacity)
        {
            _Length = Size;
            if(Size == 0)
            {
                if(_Data != NULL)
                {
                    delete[] _Data;
                    _Capacity = 0;
                }
            }
        }
        else
        {
            unsigned int NewCapacity = Size * 3 / 2 + 4;
            T *NewData = new T[NewCapacity];
            //const unsigned int Length = _Length;
            /*for(unsigned int Index = 0; Index < Length; Index++)
            {
                NewData[Index] = (_Data[Index]);
            }*/
            if(_Data != NULL)
            {
                std::move(_Data, _Data + _Length, NewData);
                delete[] _Data;
            }
            _Data = NewData;
            _Length = Size;
            _Capacity = NewCapacity;
        }
    }

    void Reserve(unsigned int Size)
    {
        if(Size > _Capacity)
        {
            T *NewData = new T[Size];
            if(_Data != NULL)
            {
                //memcpy(NewData, _Data, sizeof(T) * _Length);
                //for(UINT index = 0; index < _Length; index++)
                //{
                //    new (_Data + index) T;
                //}
                std::move(_Data, _Data + _Length, NewData);
                delete[] _Data;
            }
            _Data = NewData;
            _Capacity = Size;
        }
    }

    //
    // Accessors
    //
    __forceinline T& operator [] (unsigned int k)
    {
#ifdef VECTOR_DEBUG
        if(k >= _Length)
        {
            SignalError("Out-of-bounds vector access");
        }
#endif
        return _Data[k];
    }
    __forceinline T& operator [] (int k) 
    {
#ifdef VECTOR_DEBUG
        if(k < 0 || k >= int(_Length))
        {
            SignalError("Out-of-bounds vector access");
        }
#endif
        return _Data[k];
    }
    __forceinline const T& operator [] (unsigned int k) const
    {
#ifdef VECTOR_DEBUG
        if(k >= _Length)
        {
            SignalError("Out-of-bounds vector access");
        }
#endif
        return _Data[k];
    }
    __forceinline const T& operator [] (int k) const
    {
#ifdef VECTOR_DEBUG
        if(k < 0 || k >= int(_Length))
        {
            SignalError("Out-of-bounds vector access");
        }
#endif
        return _Data[k];
    }
    __forceinline unsigned int Length() const
    {
        return _Length;
    }
    __forceinline T* CArray()
    {
#ifdef VECTOR_DEBUG
        if(_Length == 0)
        {
            SignalError("CArray called on zero-length vector");
        }
#endif
        return _Data;
    }
    __forceinline const T* CArray() const
    {
#ifdef VECTOR_DEBUG
        if(_Length == 0)
        {
            SignalError("CArray called on zero-length vector");
        }
#endif
        return _Data;
    }
    __forceinline T& RandomElement()
    {
        Assert(_Length > 0, "RandomElement called with no elements");
        return _Data[rand() % _Length];
    }
    __forceinline const T& RandomElement() const
    {
        Assert(_Length > 0, "RandomElement called with no elements");
        return _Data[rand() % _Length];
    }
    __forceinline T& Last()
    {
        Assert(_Length > 0, "Last called with no elements");
        return _Data[_Length - 1];
    }
    __forceinline const T& Last() const
    {
        Assert(_Length > 0, "Last called with no elements");
        return _Data[_Length - 1];
    }
    __forceinline T& First()
    {
        Assert(_Length > 0, "First called with no elements");
        return _Data[0];
    }
    __forceinline const T& First() const
    {
        Assert(_Length > 0, "First called with no elements");
        return _Data[0];
    }
    __forceinline T* begin()
    {
        return _Data;
    }
    __forceinline const T* begin() const
    {
        return _Data;
    }
    __forceinline T* end()
    {
        return _Data + _Length;
    }
    __forceinline const T* end() const
    {
        return _Data + _Length;
    }

    //
    // Modifiers
    //
    void Append(const Vector<T> &V);
    void PushEnd(const T &t);
    void PushEnd(T &&t);
    void PushEnd();
    void PopEnd();
    void PopFront();
    void RemoveSlow(unsigned int Index);
    void RemoveSwap(unsigned int Index);
    void Randomize();
    void Sort();
    void Scale(const T &t);
    template<class orderingType> void Sort(orderingType Function)
    {
        if(_Length >= 2)
        {
            std::sort(_Data, _Data + _Length, Function);
        }
    }
    void Clear(const T &T);

    //
    // Query
    //
    T Sum() const;
    T Product() const;
    const T& MaxValue() const;
    unsigned int MaxIndex() const;
    const T& MinValue() const;
    unsigned int MinIndex() const;
    bool Contains(const T &t) const;
    UINT Count(const T &t) const;
    int FindFirstIndex(const T &t) const;
    unsigned int Hash32() const;
    unsigned __int64 Hash64() const;

    //
    // File
    //
    void SaveToASCIIFile(const String &Filename)
    {
        ofstream File(Filename.CString());
        PersistentAssert(!File.fail(), "Failed to open file");
        File << _Length << endl;
        for(unsigned int Index = 0; Index < _Length; Index++)
        {
            File << _Data[Index] << '\n';
        }
    }

    void LoadFromASCIIFile(const String &Filename)
    {
        ifstream File(Filename.CString());
        PersistentAssert(!File.fail(), "Failed to open file");
        unsigned int Length;
        File >> Length;
        Allocate(Length);
        for(unsigned int Index = 0; Index < Length; Index++)
        {
            File >> _Data[Index];
        }
    }

    //
    // Wrapping stack-allocated memory
    //
    void WrapMemory(T *Data, UINT Length)
    {
        _Data = Data;
        _Length = Length;
        _Capacity = _Capacity;
    }
    void UnwrapMemory()
    {
        _Data = NULL;
    }

protected:
    //vector<T> Data;
    T *_Data;
    unsigned int _Length;
    unsigned int _Capacity;
};

#pragma warning(default : 4700)

#include "Vector.cpp"
