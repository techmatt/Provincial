/*
SparseMatrix.h
Written by Matthew Fisher

a sparse matrix structure.  For now this assumes all matrix elements are of type T,
defined in DenseMatrix.h.  It should instead be templatized.
*/

//
// Namespace for helper functions related to Vector<T>, such as multiplying or adding two
// vectors.
//
namespace RealVector
{
    template<class T> static void Multiply(Vector<T> &Result, T Scale, const Vector<T> &V);
    template<class T> static void Multiply(Vector<T> &Result, const Vector<T> &V, T Scale);
    template<class T> static T DotProduct(const Vector<T> &L, const Vector<T> &R);
    template<class T> static void Add(Vector<T> &Result, const Vector<T> &L, const Vector<T> &R);
    template<class T> static void Subtract(Vector<T> &Result, const Vector<T> &L, const Vector<T> &R);
}

template<class T>
struct SparseElement
{
    int Col;
    T Entry;
};

template<class T>
struct SparseRow
{
    SparseRow() {}
    SparseRow(const SparseRow &S)
    {
        Data = S.Data;
    }
    SparseRow& operator = (const SparseRow &S)
    {
        Data = S.Data;
        return *this;
    }
    ~SparseRow() { FreeMemory(); }

    void FreeMemory();

    void PushElement(UINT Col, T Entry);
    bool FindElement(UINT Col, const SparseElement<T>* &Output) const;
    bool FindElement(UINT Col, SparseElement<T>* &Output);

    Vector<SparseElement<T> > Data;
};

template<class T>
class SparseMatrix
{
public:
    //
    // Constructors
    //
    SparseMatrix();
    explicit SparseMatrix(UINT Size);
    explicit SparseMatrix(UINT RowCount, UINT ColCount);
    SparseMatrix(const SparseMatrix<T> &M);
    SparseMatrix& operator = (const SparseMatrix<T> &M);

    //
    // Destructors
    //
    ~SparseMatrix();
    void FreeMemory();

    //
    // Memory
    //
    void Allocate(UINT Size);
    void Allocate(UINT _RowCount, UINT _ColCount);
    void Init(const Vector<T> &DiagonalEntries);

    //
    // Helper functions
    //
    UINT TotalElements();
    bool Square() const;
    bool Symmetric() const;
    SparseMatrix<T> Transpose() const;
    SparseMatrix<T> DiagonalInverse() const;
    T Compare(const SparseMatrix<T> &In);

    //
    // Output
    //
    void Dump(ostream &os, bool Sparse);
    void MathematicaDump(ostream &os, bool Sparse) const;

    //
    // Modifiers
    //
    SparseMatrix& operator *= (T B);
    void PushElement(UINT Row, UINT Col, T Entry);
    __forceinline void PushDiagonalElement(UINT Row, T Entry)
    {
        PushElement(Row, Row, Entry);
    }

    //
    // Accessors
    //
    __forceinline UINT RowCount() const
    {
        return _RowCount;
    }
    __forceinline UINT ColCount() const
    {
        return _ColCount;
    }
    __forceinline Vector<SparseRow<T> >& Rows()
    {
        return _Rows;
    }
    __forceinline const Vector<SparseRow<T> >& Rows() const
    {
        return _Rows;
    }
    __forceinline T GetElement(UINT Row, UINT Col) const
    {
        Assert(Row < _RowCount && Col < _ColCount, "Invalid Element");
        const SparseElement<T> *Output;
        bool Found = _Rows[Row].FindElement(Col, Output);
        if(Found)
        {
            return Output->Entry;
        }
        else
        {
            return 0.0;
        }
    }
    
    //
    // Static helper functions for functional-style sparse matrix manipulation.  Since the copy constructor
    // for a sparse matrix can be costly, this approach may be more efficient that using operator overloading.
    //
    static void Multiply(SparseMatrix<T> &A, T C);
    static void Multiply(Vector<T> &Output, const SparseMatrix<T> &M, const Vector<T> &V);
    static void MultiplyTranspose(SparseMatrix<T> &Output, const SparseMatrix<T> &A, const SparseMatrix<T> &B);
    static void Multiply(SparseMatrix<T> &Output, const SparseMatrix<T> &A, const SparseMatrix<T> &B);
    static T Multiply(const SparseRow<T> &L, const Vector<T> &R);
    static void Add(SparseMatrix<T> &A, const SparseMatrix<T> &B, const SparseMatrix<T> &C);
    static void Subtract(SparseMatrix<T> &A, const SparseMatrix<T> &B, const SparseMatrix<T> &C);

private:
    Vector<SparseRow<T> > _Rows;
    UINT _RowCount, _ColCount;
};

template<class T>
__forceinline SparseMatrix<T> operator + (const SparseMatrix<T> &A, const SparseMatrix<T> &B)
{
    SparseMatrix<T> Result;
    SparseMatrix<T>::Add(Result, A, B);
    return Result;
}

template<class T>
__forceinline SparseMatrix<T> operator - (const SparseMatrix<T> &A, const SparseMatrix<T> &B)
{
    SparseMatrix<T> Result;
    SparseMatrix<T>::Subtract(Result, A, B);
    return Result;
}

template<class T>
__forceinline SparseMatrix<T> operator * (const SparseMatrix<T> &A, const SparseMatrix<T> &B)
{
    SparseMatrix<T> Result;
    SparseMatrix<T>::Multiply(Result, A, B);
    return Result;
}

template<class T>
__forceinline SparseMatrix<T> operator * (const SparseMatrix<T> &A, T B)
{
    SparseMatrix<T> Result;
    Result = A;
    SparseMatrix<T>::Multiply(Result, B);
    return Result;
}

template<class T>
__forceinline Vector<T> operator * (const SparseMatrix<T> &M, const Vector<T> &V)
{
    Vector<T> Result(M.RowCount());
    SparseMatrix<T>::Multiply(Result, M, V);
    return Result;
}

template<class T>
__forceinline Vector<T> operator + (const Vector<T> &L, const Vector<T> &R)
{
    Assert(L.Length() == R.Length(), "Invalid vector dimensions");
    Vector<T> Result(L.Length());
    for(UINT i = 0; i < L.Length(); i++)
    {
        Result[i] = L[i] + R[i];
    }
    return Result;
}

template<class T>
__forceinline Vector<T> operator - (const Vector<T> &L, const Vector<T> &R)
{
    Assert(L.Length() == R.Length(), "Invalid vector dimensions");
    Vector<T> Result(L.Length());
    for(UINT i = 0; i < L.Length(); i++)
    {
        Result[i] = L[i] - R[i];
    }
    return Result;
}

#include "SparseMatrix.cpp"