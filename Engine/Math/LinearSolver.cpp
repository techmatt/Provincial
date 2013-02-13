/*
LinearSolver.cpp
Written by Matthew Fisher

LinearSolver is an abstract class used to solve sparse linear systems.
These are problems of the form Mx = b, where M is a known matrix, b is a known column vector,
and x is the unknown column vector.  This class is implemented using conjugate gradient descent,
bi-conjugate gradient descent, and TAUCS.
*/

template<class T>
LinearSolver<T>::LinearSolver()
{
    M = NULL;
}

template<class T>
double LinearSolver<T>::ComputeError(const Vector<T> &x, const Vector<T> &b) const
{
    Assert(M != NULL && M->Square() && x.Length() == M->RowCount() && b.Length() == M->RowCount(), "Invalid Matrix in linear solve");
    for(UINT i = 0; i < x.Length(); i++)
    {
        Assert(x[i] == x[i] && b[i] == b[i], "Invalid term in solution");
    }
    Vector<T> r = (*M) * x - b;
    return RealVector::DotProduct(r, r) / x.Length();
}

template<class T>
void LinearSolver<T>::LoadMatrix(const SparseMatrix<T> *NewM)
{
    Assert(NewM != NULL && NewM->Square(), "Invalid matrix");
    M = NewM;
}

template<class T>
const String& LinearSolver<T>::GetOutputString() const
{
    return OutputString;
}

template<class T>
CGLinearSolver<T>::CGLinearSolver()
{
    _MaxIters = 1000;
    _Tolerance = 1e-5f;
}

template<class T>
CGLinearSolver<T>::~CGLinearSolver()
{
    FreeMemory();
}

template<class T>
void CGLinearSolver<T>::FreeMemory()
{
    p.FreeMemory();
    q.FreeMemory();
    r.FreeMemory();
    z.FreeMemory();
    Temp.FreeMemory();
    DiagTerms.FreeMemory();
}

template<class T>
void CGLinearSolver<T>::SetParamaters(UINT MaxIters, double Tolerance)
{
    _MaxIters = MaxIters;
    _Tolerance = Tolerance;
}

template<class T>
void CGLinearSolver<T>::AllocateSpace()
{
    Assert(M != NULL, "M not specified prior to AllocateSpace.");
    UINT n = M->RowCount();
    if(r.Length() != n)
    {
        r.Allocate(n);
        q.Allocate(n);
        p.Allocate(n);
        z.Allocate(n);

        Temp.Allocate(n);
        DiagTerms.Allocate(n);
    }
}

template<class T>
void CGLinearSolver<T>::Factor()
{
    AllocateSpace();
    LoadDiagonalTerms();
}

template<class T>
void CGLinearSolver<T>::LoadDiagonalTerms()
{
    UINT n = DiagTerms.Length();
    for(UINT i = 0; i < n; i++)
    {
        const SparseElement *E;
        DiagTerms[i] = 1.0;
        if(M->Rows()[i].FindElement(i, E) && E->Entry != 0.0f)
        {
            DiagTerms[i] = E->Entry;
        }
    }
}

template<class T>
void CGLinearSolver<T>::Solve(Vector<T> &x, const Vector<T> &b)
{
    bool Converged = false;
    double Result, Numerator, Denominator, Alpha, Beta, Error;
    UINT Iter = 0;

    Assert(M != NULL && M->Square(), "M invalid");
    Assert(b.Length() == M->RowCount(), "Invalid size for paramater b");
    Assert(r.Length() == M->RowCount(), "Memory not allocated or matrix changed between factor and solve");

    if(x.Length() != M->RowCount())
    {
        x.Allocate(M->RowCount());
        for(UINT i = 0; i < x.Length(); i++)
        {
            x[i] = 0.0f;
        }
    }

    SparseMatrix::Multiply(Temp, *M, x);
    RealVector::Subtract(r, b, Temp);
    for(UINT i = 0; i < p.Length(); i++)
    {
        p[i] = r[i];
    }
    
    while(!Converged)
    {
        Iter++;

        Numerator = RealVector::Multiply(r, r);
        SparseMatrix::Multiply(q, *M, p);
        Denominator = RealVector::Multiply(p, q);

        if(Denominator == 0.0)
        {
            OutputString += String("Zero denominator\n");
            return;
        }
        if(Numerator == 0.0)
        {
            OutputString += String("Zero numerator\n");
            return;
        }

        Alpha = Numerator / Denominator;

        RealVector::Multiply(Temp, Alpha, p);
        RealVector::Add(x, x, Temp);

        if(Iter % 50 == 49)
        {
            SparseMatrix::Multiply(Temp, *M, x);
            RealVector::Subtract(r, b, Temp);
        }
        else
        {
            RealVector::Multiply(Temp, Alpha, q);
            RealVector::Subtract(r, r, Temp);
        }

        Result = RealVector::Multiply(r, r);
        Beta = Result / Numerator;
        
        RealVector::Multiply(Temp, Beta, p);
        RealVector::Add(p, r, Temp);

        Numerator = RealVector::Multiply(r, r);
        Denominator = RealVector::Multiply(b, b);
        if(Denominator == 0.0)
        {
            OutputString += String("Zero denominator\n");
            return;
        }
        Error = Numerator / Denominator;
        OutputString += String("Iteration ") + String(Iter) + String(" Error ") + String(Error) + String("\n");
        if(Iter > _MaxIters || Error < _Tolerance)
        {
            Converged = true;
        }
    }
}

template<class T>
BiCGLinearSolver<T>::BiCGLinearSolver()
{
    _MaxIters = 1000;
    _Tolerance = 1e-5f;
}

template<class T>
BiCGLinearSolver<T>::~BiCGLinearSolver()
{
    FreeMemory();
}

template<class T>
void BiCGLinearSolver<T>::FreeMemory()
{
    p.FreeMemory();
    q.FreeMemory();
    r.FreeMemory();
    z.FreeMemory();
    p2.FreeMemory();
    q2.FreeMemory();
    r2.FreeMemory();
    z2.FreeMemory();
    Temp.FreeMemory();
    DiagTerms.FreeMemory();
}

template<class T>
void BiCGLinearSolver<T>::SetParamaters(UINT MaxIters, double Tolerance)
{
    _MaxIters = MaxIters;
    _Tolerance = Tolerance;
}

template<class T>
void BiCGLinearSolver<T>::AllocateSpace()
{
    Assert(M != NULL, "M not specified prior to AllocateSpace");
    UINT n = M->RowCount();
    if(r.Length() != n)
    {
        r.Allocate(n);
        q.Allocate(n);
        p.Allocate(n);
        z.Allocate(n);

        r2.Allocate(n);
        q2.Allocate(n);
        p2.Allocate(n);
        z2.Allocate(n);

        Temp.Allocate(n);
        DiagTerms.Allocate(n);
    }
}

template<class T>
void BiCGLinearSolver<T>::Factor()
{
    AllocateSpace();
    LoadDiagonalTerms();
}

template<class T>
void BiCGLinearSolver<T>::LoadDiagonalTerms()
{
    UINT n = DiagTerms.Length();
    for(UINT i = 0; i < n; i++)
    {
        const SparseElement<T> *E;
        DiagTerms[i] = 1.0;
        if(M->Rows()[i].FindElement(i, E) && E->Entry != 0.0f)
        {
            DiagTerms[i] = E->Entry;
        }
    }
}

template<class T>
void BiCGLinearSolver<T>::Solve(Vector<T> &x, const Vector<T> &b)
{
    bool Converged = false;
    bool First = true;
    double Beta, Alpha, Row = 0.0, PrevRow, Numerator, Denominator, Error, BMag;
    UINT Iter = 0, N = M->Rows().Length();

    Assert(M != NULL, "M invalid");
    Assert(b.Length() == M->RowCount(), "Invalid size for paramater b");
    Assert(r.Length() == M->RowCount(), "Memory not allocated or Matrix4 changed between factor and solve");

    if(x.Length() != M->RowCount())
    {
        x.Allocate(M->RowCount());
        for(UINT i = 0; i < x.Length(); i++)
        {
            x[i] = 0.0f;
        }
    }

    BMag = RealVector::DotProduct(b, b);
    if(BMag == 0.0)
    {
        OutputString += String("BMag == 0.0\n");
        return;
    }

    SparseMatrix<T> M_T = M->Transpose();

    SparseMatrix<T>::Multiply(Temp, *M, x);
    RealVector::Subtract(r, b, Temp);
    for(UINT i = 0; i < N; i++)
    {
        r2[i] = r[i];
    }
    
    while(!Converged)
    {
        Iter++;

        if(Iter % 50 == 49)
        {
            SparseMatrix<T>::Multiply(Temp, *M, x);
            RealVector::Subtract(r, b, Temp);
            for(UINT i = 0; i < N; i++)
            {
                r2[i] = r[i];
            }
            First = true;
        }

        for(UINT i = 0; i < N; i++)
        {
            z[i] = r[i] / DiagTerms[i];
            z2[i] = r2[i] / DiagTerms[i];
        }

        PrevRow = Row;
        Row = RealVector::DotProduct(z, r2);
        if(Row == 0.0)
        {
            OutputString += String("Row Zero; Algorithm fails.");
            return;
        }

        if(First)
        {
            for(UINT i = 0; i < N; i++)
            {
                p[i] = z[i];
                p2[i] = z2[i];
            }
            First = false;
        }
        else
        {
            Beta = Row / PrevRow;
            for(UINT i = 0; i < N; i++)
            {
                p[i] = z[i] + Beta * p[i];
                p2[i] = z2[i] + Beta * p2[i];
            }
        }

        SparseMatrix<T>::Multiply(q, *M, p);
        SparseMatrix<T>::Multiply(q2, M_T, p2);

        Denominator = RealVector::DotProduct(p2, q);
        if(Denominator == 0.0)
        {
            OutputString += String("Denominator zero.\n");
            return;
        }
        Alpha = Row / Denominator;

        for(UINT i = 0; i < N; i++)
        {
            x[i] += Alpha * p[i];
            r[i] -= Alpha * q[i];
            r2[i] -= Alpha * q2[i];
        }

        Numerator = RealVector::DotProduct(r, r);
        Error = Numerator / BMag;
        //Error = ComputeError(x, b) * x.Length();
        if(Iter % 50 == 0)
        {
            OutputString += String("BiCGLinearSolver Iteration ") + String(Iter) + String(" Error ") + String(Error) + String("\n");
        }
        if(Iter > _MaxIters || Error < _Tolerance)
        {
            Converged = true;
        }
    }
}

#ifdef USE_TAUCS
template<class T>
TaucsSymmetricLinearSolver<T>::TaucsSymmetricLinearSolver()
{
    ETree = NULL;
    PermutedM = NULL;
    LTriangular = NULL;
    Permutation = NULL;
    InversePermutation = NULL;
    TaucsM = NULL;
}

template<class T>
TaucsSymmetricLinearSolver<T>::~TaucsSymmetricLinearSolver()
{
    FreeMemory();
}

template<class T>
void TaucsSymmetricLinearSolver<T>::FreeMemory()
{
    if(PermutedM)
    {
        taucs_ccs_free(PermutedM);
        PermutedM = NULL;
    }
    if(TaucsM)
    {
        taucs_ccs_free(TaucsM);
        TaucsM = NULL;
    }
    if(LTriangular)
    {
        taucs_ccs_free(LTriangular);
        LTriangular = NULL;
    }
    if(Permutation)
    {
        free(Permutation);
        Permutation = NULL;
    }
    if(InversePermutation)
    {
        free(InversePermutation);
        InversePermutation = NULL;
    }
}

template<class T>
void TaucsSymmetricLinearSolver<T>::Factor()
{
    FreeMemory();

    SparseMatrix<T> MT = M->Transpose();

    Assert(MT.Symmetric(), "M symmetric test failed.");
    taucs_logfile("c:\\scenereconstruction\\TaucsDebug.txt");

    UINT nnz = 0;
    for(UINT i = 0; i < MT.RowCount(); i++)
    {
        nnz += MT.Rows()[i].Data.Length();
    }

    bool UseSymmetry = true;

    int flags = 0;
    if(sizeof(T) == sizeof(double))
    {
        flags |= TAUCS_DOUBLE;
    }
    else
    {
        flags |= TAUCS_SINGLE;
    }

    if(UseSymmetry)
    {
        flags |= TAUCS_LOWER | TAUCS_SYMMETRIC;
    }
    TaucsM = taucs_ccs_create(MT.ColCount(), MT.RowCount(), nnz, flags);

    UINT CurValueIndex = 0;
    for(UINT CurRow = 0; CurRow < MT.RowCount(); CurRow++)
    {
        TaucsM->colptr[CurRow] = CurValueIndex;
        UINT Entries = MT.Rows()[CurRow].Data.Length();
        for(UINT EntryIndex = 0; EntryIndex < Entries; EntryIndex++)
        {
            const SparseElement *Element = &MT.Rows()[CurRow].Data[EntryIndex];
            if(!UseSymmetry || Element->Col >= int(CurRow))
            {
                if(sizeof(T) == sizeof(double))
                {
                    TaucsM->values.d[CurValueIndex] = Element->Entry;
                }
                else
                {
                    TaucsM->values.s[CurValueIndex] = float(Element->Entry);
                }
                TaucsM->rowind[CurValueIndex] = Element->Col;
                CurValueIndex++;
            }
        }
    }
    TaucsM->colptr[MT.RowCount()] = CurValueIndex;

    //
    // bandlimitation
    //
    taucs_ccs_order(TaucsM, &Permutation, &InversePermutation, "metis");
    PermutedM = taucs_ccs_permute_symmetrically(TaucsM, Permutation, InversePermutation);
    Assert(PermutedM != NULL, "taucs_ccs_permute_symmetrically failed.");

    //
    // Cholesky factorization
    //
    //LTriangular = taucs_ccs_factor_llt_mf(PermutedM);
    LTriangular = taucs_ccs_factor_llt(PermutedM, 0.0, 0);
    Assert(LTriangular != NULL, "taucs_ccs_factor_llt failed");
}

template<class T>
void TaucsSymmetricLinearSolver<T>::Solve(Vector<T> &x, const Vector<T> &b)
{
    const unsigned int N = M->RowCount();
    Vector<T>  PB(N), PX(N);

    if(x.Length() != M->RowCount())
    {
        x.Allocate(M->RowCount());
        x.Clear(0.0f);
    }

    //
    // permute rhs
    //
    for (unsigned int i = 0; i < N; i++)
    {
        PB[i] = b[Permutation[i]];
    }

    //
    // solve by back-substitution
    //
    //int Result = taucs_supernodal_solve_llt(LTriangular, PX.CArray(), PB.CArray());
    int Result = taucs_ccs_solve_llt(LTriangular, PX.CArray(), PB.CArray());
    Assert(Result == TAUCS_SUCCESS, "taucs_ccs_solve_llt failed");

    //
    // re-permute x
    //
    for (unsigned int i = 0; i < N; i++)
    {
        x[i] = PX[InversePermutation[i]];
    }
}
#endif