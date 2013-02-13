/*
SparseMatrix.cpp
Written by Matthew Fisher
*/

#pragma once

template<class T>
void SparseRow<T>::FreeMemory()
{
    Data.FreeMemory();
}

template<class T>
bool SparseRow<T>::FindElement(UINT Col, const SparseElement<T>* &Output) const
{
    for(UINT i = 0; i < Data.Length(); i++)
    {
         if(Data[i].Col == Col)
         {
             Output = &Data[i];
             return true;
         }
    }
    return false;
}

template<class T>
bool SparseRow<T>::FindElement(UINT Col, SparseElement<T>* &Output)
{
    for(UINT i = 0; i < Data.Length(); i++)
    {
         if(Data[i].Col == Col)
         {
             Output = &Data[i];
             return true;
         }
    }
    return false;
}

template<class T>
void SparseRow<T>::PushElement(UINT Col, T Entry)
{
    if(Entry == 0.0)
    {
        return;
    }

    SparseElement<T> *Element;
    if(FindElement(Col, Element))
    {
        Element->Entry += Entry;
    }
    else
    {
        SparseElement<T> NewElement;
        NewElement.Col = Col;
        NewElement.Entry = Entry;
        Data.PushEnd(NewElement);
    }
}

template<class T>
void SparseMatrix<T>::Allocate(UINT Size)
{
    Allocate(Size, Size);
}

template<class T>
void SparseMatrix<T>::Allocate(UINT NRowCount, UINT NColCount)
{
    _RowCount = NRowCount;
    _ColCount = NColCount;
    _Rows.Allocate(NRowCount);
}

template<class T>
void SparseMatrix<T>::Init(const Vector<T> &DiagonalEntries)
{
    Allocate(DiagonalEntries.Length());
    for(UINT i = 0; i < DiagonalEntries.Length(); i++)
    {
        PushElement(i, i, DiagonalEntries[i]);
    }
}

template<class T>
UINT SparseMatrix<T>::TotalElements()
{
    UINT Result = 0;
    for(UINT i = 0; i < _RowCount; i++)
    {
        Result += _Rows[i].Data.Length();
    }
    return Result;
}

template<class T>
void SparseMatrix<T>::PushElement(UINT Row, UINT Col, T Entry)
{
    Assert(Row < _RowCount && Col < _ColCount, "Invalid element added.");
    _Rows[Row].PushElement(Col, Entry);
}

template<class T>
T SparseMatrix<T>::Compare(const SparseMatrix<T> &In)
{
    Assert(In._RowCount == _RowCount && In._ColCount == _ColCount, "Invalid size.");
    T Sum = 0.0;
    for(UINT i = 0; i < _RowCount; i++)
    {
        for(UINT i2 = 0; i2 < _Rows[i].Data.Length(); i2++)
        {
            SparseElement *CurElement = &_Rows[i].Data[i2];
            const SparseElement *Output;
            bool Success = In._Rows[i].FindElement(CurElement->Col, Output);
            Assert(Success, "FindElement failed.");
            Sum += Math::Abs(Output->Entry - CurElement->Entry);
        }
    }
    return Sum;
}

template<class T>
SparseMatrix<T> SparseMatrix<T>::DiagonalInverse() const
{
    SparseMatrix<T> Result = *this;
    Assert(Square(), "DiagonalInverse failure, matrix not square.");
    for(UINT i = 0; i < _RowCount; i++)
    {
        if(Result._Rows[i].Data.Length() > 0)
        {
            Assert(Result._Rows[i].Data.Length() == 1 && Result._Rows[i].Data[0].Col == i, "DiagonalInverse failure, matrix not diagonal.");
            Result._Rows[i].Data[0].Entry = 1.0f / Result._Rows[i].Data[0].Entry;
        }
    }
    return Result;
}

template<class T>
SparseMatrix<T> SparseMatrix<T>::Transpose() const
{
    SparseMatrix<T> O;
    const SparseElement<T> *Element;

    O.Rows().FreeMemory();
    O.Allocate(_ColCount, _RowCount);
    
    for(UINT i = 0; i < _RowCount; i++)
    {
        for(UINT i2 = 0; i2 < _Rows[i].Data.Length(); i2++)
        {
            Element = &_Rows[i].Data[i2];
            O.PushElement(Element->Col, i, Element->Entry);
        }
    }
    return O;
}

template<class T>
void SparseMatrix<T>::Dump(ostream &os, bool Sparse)
{
    if(Sparse)
    {
        for(UINT Row = 0; Row < _RowCount; Row++)
        {
            const SparseRow<T> &CurRow = _Rows[Row];
            for(UINT ColEntry = 0; ColEntry < CurRow.Data.Length(); ColEntry++)
            {
                const SparseElement &CurEntry = CurRow.Data[ColEntry];
                os << CurEntry.Entry << '\t';
            }
            os << endl;
        }
    }
    else
    {
        for(UINT Row = 0; Row < _RowCount; Row++)
        {
            for(UINT Col = 0; Col < _ColCount; Col++)
            {
                SparseElement *Element;
                T Value = 0.0;
                if(_Rows[Row].FindElement(Col, Element))
                {
                    Value = Element->Entry;
                }
                os << Value << '\t';
            }
            os << endl;
        }
    }
}

template<class T>
bool SparseMatrix<T>::Square() const
{
    return (_RowCount == _ColCount);
}

template<class T>
bool SparseMatrix<T>::Symmetric() const
{
    PersistentAssert(Square(), "Only square matrices can be symmetric.");
    for(UINT i = 0; i < _RowCount; i++)
    {
        UINT Entries = _Rows[i].Data.Length();
        for(UINT i2 = 0; i2 < Entries; i2++)
        {
            const SparseElement *Element = &_Rows[i].Data[i2];
            const SparseElement *OtherElement = NULL;
            if(Element->Entry != 0.0)
            {
                PersistentAssert(Element->Entry == Element->Entry, "Invalid entry in matrix");
                if(!_Rows[Element->Col].FindElement(i, OtherElement))
                {
                    Console::WriteLine("SparseMatrix<T>::Symmetric: Other element not found");
                    return false;
                }
                if(fabs(OtherElement->Entry - Element->Entry) > 1e-5)
                {
                    Console::WriteLine("Other element does not match.");
                    return false;
                }
            }
        }
    }
    return true;
}

template<class T>
void SparseMatrix<T>::MathematicaDump(ostream &os, bool Sparse) const
{
    os << setiosflags(ios_base::fixed);
    os << setprecision(10);
    if(Sparse)
    {
        os << "SparseArray[{";
        for(UINT Row = 0; Row < _RowCount; Row++)
        {
            for(UINT i = 0; i < _Rows[Row].Data.Length(); i++)
            {
                const SparseElement *Element = &_Rows[Row].Data[i];
                os << "{" << Row + 1 << "," << Element->Col + 1 << "}->" << Element->Entry;
                if(Row != _RowCount - 1 || i != _Rows[Row].Data.Length() - 1)
                {
                    os << ",";
                }
            }
            os << endl;
        }
        os << "}, {" << _RowCount << ", " << _ColCount << "}]";
    }
    else
    {
        os << "{";
        for(UINT Row = 0; Row < _RowCount; Row++)
        {
            os << "{";
            for(UINT Col = 0; Col < _ColCount; Col++)
            {
                const SparseElement *Element;
                T Value = 0.0;
                if(_Rows[Row].FindElement(Col, Element))
                {
                    Value = Element->Entry;
                }
                os << Value;
                if(Col != _ColCount - 1)
                {
                    os << ", ";
                }
            }
            os << "}";
            if(Row != _RowCount - 1)
            {
                os << ",";
            }
            os << endl;
        }
        os << "}";
    }
}

template<class T>
SparseMatrix<T>::SparseMatrix()
{
    _RowCount = 0;
    _ColCount = 0;
}

template<class T>
SparseMatrix<T>::SparseMatrix(UINT Size)
{
    Allocate(Size, Size);
}

template<class T>
SparseMatrix<T>::SparseMatrix(UINT RowCount, UINT ColCount)
{
    Allocate(RowCount, ColCount);
}

template<class T>
SparseMatrix<T>::SparseMatrix(const SparseMatrix<T> &M)
{
    Allocate(M._RowCount, M._ColCount);
    for(UINT i = 0; i < _RowCount; i++)
    {
        _Rows[i] = M._Rows[i];
    }
}

template<class T>
SparseMatrix<T>::~SparseMatrix()
{
    FreeMemory();
}

template<class T>
void SparseMatrix<T>::FreeMemory()
{
    for(UINT i = 0; i < _Rows.Length(); i++)
    {
        _Rows[i].FreeMemory();
    }
    _Rows.FreeMemory();
    _RowCount = 0;
    _ColCount = 0;
}

template<class T>
SparseMatrix<T>& SparseMatrix<T>::operator = (const SparseMatrix<T> &M)
{
    Allocate(M._RowCount, M._ColCount);
    for(UINT i = 0; i < _RowCount; i++)
    {
        _Rows[i] = M._Rows[i];
    }
    return *this;
}

template<class T>
SparseMatrix<T>& SparseMatrix<T>::operator *= (T Scale)
{
    Multiply(*this, Scale);
    return *this;
}

template<class T>
void SparseMatrix<T>::Add(SparseMatrix<T> &A, const SparseMatrix<T> &B, const SparseMatrix<T> &C)
{
    Assert(B.RowCount() == C.RowCount() &&
             B.ColCount() == C.ColCount(), "Invalid matrix addition dimensions.");
    A.Allocate(B.RowCount(), C.ColCount());

    for(UINT i = 0; i < B.RowCount(); i++)
    {
        for(UINT i2 = 0; i2 < B.Rows()[i].Data.Length(); i2++)
        {
            const SparseElement *Element = &B.Rows()[i].Data[i2];
            A.PushElement(i, Element->Col, Element->Entry);
        }

        for(UINT i2 = 0; i2 < C.Rows()[i].Data.Length(); i2++)
        {
            const SparseElement *Element = &C.Rows()[i].Data[i2];
            A.PushElement(i, Element->Col, Element->Entry);
        }
    }
}

template<class T>
void SparseMatrix<T>::Subtract(SparseMatrix<T> &A, const SparseMatrix<T> &B, const SparseMatrix<T> &C)
{
    Assert(B.RowCount() == C.RowCount() &&
             B.ColCount() == C.ColCount(), "Invalid matrix addition dimensions.");
    A.Allocate(B.RowCount(), C.ColCount());

    for(UINT i = 0; i < B.RowCount(); i++)
    {
        for(UINT i2 = 0; i2 < B.Rows()[i].Data.Length(); i2++)
        {
            const SparseElement *Element = &B.Rows()[i].Data[i2];
            A.PushElement(i, Element->Col, Element->Entry);
        }

        for(UINT i2 = 0; i2 < C.Rows()[i].Data.Length(); i2++)
        {
            const SparseElement *Element = &C.Rows()[i].Data[i2];
            A.PushElement(i, Element->Col, -Element->Entry);
        }
    }
}

template<class T>
void SparseMatrix<T>::Multiply(SparseMatrix<T> &A, T B)
{
    UINT _RowCount = A.RowCount();
    for(UINT i = 0; i < _RowCount; i++)
    {
        UINT Entries = A.Rows()[i].Data.Length();
        for(UINT i2 = 0; i2 < Entries; i2++)
        {
            A.Rows()[i].Data[i2].Entry *= B;
        }
    }
}

template<class T>
void SparseMatrix<T>::Multiply(SparseMatrix<T> &A, const SparseMatrix<T> &B, const SparseMatrix<T> &C)
{
    Assert(B.ColCount() == C.RowCount(), "Invalid matrix multiply dimensions.");
    A.Allocate(B.RowCount(), C.ColCount());
    
    for(UINT BRow = 0; BRow < B.RowCount(); BRow++)
    {
        UINT BRowLength = B.Rows()[BRow].Data.Length();
        for(UINT BColIndex = 0; BColIndex < BRowLength; BColIndex++)
        {
            const SparseElement<T> *BElement = &B.Rows()[BRow].Data[BColIndex];
            UINT j = BElement->Col;
            T BEntry = BElement->Entry;

            UINT CRowLength = C.Rows()[j].Data.Length();
            for(UINT CRowIndex = 0; CRowIndex < CRowLength; CRowIndex++)
            {
                const SparseElement<T> *CElement = &C.Rows()[j].Data[CRowIndex];
                UINT k = CElement->Col;
                T CEntry = CElement->Entry;

                A.PushElement(BRow, k, BEntry * CEntry);
            }
        }
    }
}

template<class T>
void SparseMatrix<T>::MultiplyTranspose(SparseMatrix<T> &Output, const SparseMatrix<T> &A, const SparseMatrix<T> &B)
{
    Assert(Output.RowCount() == A.RowCount() &&
             Output.ColCount() == B.RowCount() &&
             A.ColCount() == B.ColCount(),
             "Invalid Matrix4 multiply dimensions.");
    SignalError("Not implemented");
}

template<class T>
void SparseMatrix<T>::Multiply(Vector<T> &Output, const SparseMatrix<T> &M, const Vector<T> &V)
{
    const SparseElement<T> *E;

    // RxC * Vx1 = Ox1
    Assert(Output.Length() == M.RowCount() && V.Length() == M.ColCount(), "Invalid vector multiply dimensions.");

    for(UINT i = 0; i < M.RowCount(); i++)
    {
        T Total = 0.0;
        for(UINT i2 = 0; i2 < M.Rows()[i].Data.Length(); i2++)
        {
            E = &M.Rows()[i].Data[i2];
            Total += E->Entry * V[E->Col];
        }
        Output[i] = Total;
    }
}

template<class T>
void RealVector::Multiply(Vector<T> &Output, T Scale, const Vector<T> &V)
{
    for(UINT i = 0; i < V.Length(); i++)
    {
        Output[i] = V[i] * Scale;
    }
}

template<class T>
void RealVector::Multiply(Vector<T> &Output, const Vector<T> &V, T Scale)
{
    Multiply(Output, Scale, V);
}

/*template<class T>
T RealVector::DotProduct(const SparseRow<T> &L, const Vector<T> &R)
{
    T Result = 0.0;
    for(UINT Index = 0; Index < L.Data.Length(); Index++)
    {
        const SparseElement *E = &L.Data[Index];
        Result += E->Entry * R[E->Col];
    }
    return Result;
}*/

template<class T>
T RealVector::DotProduct(const Vector<T> &L, const Vector<T> &R)
{
    T Result = 0.0;
    for(UINT Index = 0; Index < L.Length(); Index++)
    {
        Result += L[Index] * R[Index];
    }
    return Result;
}

template<class T>
void RealVector::Add(Vector<T> &Output, const Vector<T> &L, const Vector<T> &R)
{
    for(UINT i = 0; i < L.Length(); i++)
    {
        Output[i] = L[i] + R[i];
    }
}

template<class T>
void RealVector::Subtract(Vector<T> &Output, const Vector<T> &L, const Vector<T> &R)
{
    for(UINT i = 0; i < L.Length(); i++)
    {
        Output[i] = L[i] - R[i];
    }
}
