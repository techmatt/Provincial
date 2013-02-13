/*
DenseMatrix.h
Written by Matthew Fisher

a dense matrix structure.
*/


template<class T> class SparseMatrix;

template<class T>
class DenseMatrix
{
public:
    DenseMatrix();
    explicit DenseMatrix(UINT Size);
    explicit DenseMatrix(UINT RowCount, UINT ColCount);
    explicit DenseMatrix(const SparseMatrix<T> &M);
    DenseMatrix(const DenseMatrix<T> &M);

    ~DenseMatrix();

    //
    // Memory
    //
    void Allocate(UINT RowCount, UINT ColCount);
    void Clear(T Value = 0.0);
    void FreeMemory();
    DenseMatrix<T>& operator = (const DenseMatrix<T> &M);

    //
    // Files
    //
    void LoadFromFile(const String &Filename);
    void SaveToMATLAB(const String &Filename) const;
    void WriteToStream(ostream &os, char Delimeter) const;
    void WriteMathematicaToStream(ostream &os, const String &MatrixName) const;

    //
    // Accessors
    //
    __forceinline T* operator [] (int Row)
    {
        return &_Data[Row * _ColCount];
    }
    __forceinline const T* operator [] (int Row) const
    {
        return &_Data[Row * _ColCount];
    }
    __forceinline T& operator()(int Row, int Col)
    {
        return _Data[Row * _ColCount + Col];
    }

    __forceinline T operator()(int Row, int Col) const
    {
        return _Data[Row * _ColCount + Col];
    }
    __forceinline T& Cell(int Row, int Col)
    {
        return _Data[Row * _ColCount + Col];
    }

    __forceinline T Cell(int Row, int Col) const
    {
        return _Data[Row * _ColCount + Col];
    }

    __forceinline UINT RowCount() const
    {
        return _RowCount;
    }

    __forceinline UINT ColCount() const
    {
        return _ColCount;
    }

    //
    // Operators
    //
    void operator += (const DenseMatrix &M);
    
    //
    // In-place modifiers
    //
    void Identity();
    void Identity(UINT Size);
    void InvertInPlace();

    //
    // Query
    //
    bool Square() const;
    bool ElementsValid();
    T Determinant() const;
	T DeterminantLaplaceExpansion() const;
	void MinorMatrix(UINT Row, UINT Col, DenseMatrix<T>& Result) const;
    void ExtractRow(UINT Row, Vector<T> &Result);
    void ExtractCol(UINT Col, Vector<T> &Result);
    Vector<T> ExtractRow(UINT Row);
    Vector<T> ExtractCol(UINT Col);
    
    //
    // Linear algebra
    //
    DenseMatrix<T> Inverse() const;
    DenseMatrix<T> Transpose();
    void LUDecomposition(DenseMatrix<T> &L, DenseMatrix<T> &U);
    void LUSolve(Vector<T> &x, const Vector<T> &b);
    bool EigenSystem(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const;
    bool EigenSystem(T *Eigenvalues, T **Eigenvectors) const;
    String EigenTest(const Vector<T> &Eigenvalues, const DenseMatrix<T> &Eigenvectors) const;
    String EigenTest(const T *Eigenvalues, const T **Eigenvectors) const;

    bool EigenSystemVictor(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const;
    bool EigenSystemTNT(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const;
    
    //
    // Conversion
    //
    /*void ToColumnVector(Vector<T> &Result)
    {
        Assert(_ColCount == 1, "ToColumnVector called with more than 1 column");
        Result.ReSize(_RowCount);
        for(UINT RowIndex = 0; RowIndex < _RowCount; RowIndex++)
        {
            Result[RowIndex] = Cell(RowIndex, 0);
        }
    }*/

    //
    // Static helper functions for functional-style sparse matrix manipulation.  Since the copy constructor
    // for a dense matrix can be costly, this approach may be more efficient that using operator overloading.
    //
    static void Multiply(DenseMatrix<T> &Result, const DenseMatrix<T> &Left, const DenseMatrix<T> &Right);
    static void Multiply(DenseMatrix<T> &Result, const SparseMatrix<T> &Left, const DenseMatrix<T> &Right);
    static void Multiply(DenseMatrix<T> &Result, const DenseMatrix<T> &Left, const SparseMatrix<T> &Right);
    static void MultiplyMMTranspose(DenseMatrix<T> &Result, const DenseMatrix<T> &M);
    static void MultiplyMMTranspose(DenseMatrix<T> &Result, const SparseMatrix<T> &M);
    static void Multiply(Vector<T> &Result, const DenseMatrix<T> &Left, const Vector<T> &Right);
    static void Multiply(T *result, const DenseMatrix<T>&left, const T *right);
    static void MultiplyInPlace(DenseMatrix<T> &Result, T Right);
    static DenseMatrix<T> OuterProduct(const Vector<T> &A, const Vector<T> &B);
    static T CompareMatrices(const DenseMatrix<T> &Left, const DenseMatrix<T> &Right);

private:
    UINT _RowCount, _ColCount;
    //Vector<T> _Data;
    T *_Data;
};

//
// Overloaded operators
//
template<class T> ostream& operator << (ostream &os, const DenseMatrix<T> &m);

template<class T> DenseMatrix<T> operator * (const DenseMatrix<T> &Left, const DenseMatrix<T> &Right);
template<class T> DenseMatrix<T> operator * (const DenseMatrix<T> &Left, const SparseMatrix<T> &Right);
template<class T> DenseMatrix<T> operator * (const SparseMatrix<T> &Left, const DenseMatrix<T> &Right);
template<class T> DenseMatrix<T> operator * (const DenseMatrix<T> &Left, T Right);
template<class T> DenseMatrix<T> operator * (T Left, const DenseMatrix<T> &Right);
template<class T> DenseMatrix<T> operator + (const DenseMatrix<T> &Left, const DenseMatrix<T> &Right);
template<class T> DenseMatrix<T> operator - (const DenseMatrix<T> &Left, const DenseMatrix<T> &Right);

template<class T> OutputDataStream& operator << (OutputDataStream &S, const DenseMatrix<T> &M)
{
    UINT rowCount = M.RowCount();
    UINT colCount = M.ColCount();
    S << rowCount << colCount;
    UINT totalEntries = rowCount * colCount;
    const T *data = M[0];
    for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        S << data[entryIndex];
    }
    return S;
}

template<class T> InputDataStream& operator >> (InputDataStream &S, DenseMatrix<T> &M)
{
    UINT rowCount, colCount;
    S >> rowCount >> colCount;
    M.Allocate(rowCount, colCount);
    UINT totalEntries = rowCount * colCount;
    T *data = M[0];
    for(UINT entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        S >> data[entryIndex];
    }
    return S;
}

#include "DenseMatrix.cpp"