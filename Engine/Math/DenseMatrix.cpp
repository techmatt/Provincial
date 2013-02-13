template<class T>
DenseMatrix<T>::DenseMatrix()
{
    _RowCount = 0;
    _ColCount = 0;
    _Data = NULL;
}

template<class T>
DenseMatrix<T>::DenseMatrix(UINT Size)
{
    _RowCount = 0;
    _ColCount = 0;
    _Data = NULL;
    Allocate(Size, Size);
}

template<class T>
DenseMatrix<T>::DenseMatrix(UINT RowCount, UINT ColCount)
{
    _RowCount = 0;
    _ColCount = 0;
    _Data = NULL;
    Allocate(RowCount, ColCount);
}

template<class T>
DenseMatrix<T>::DenseMatrix(const DenseMatrix<T>&M)
{
    _RowCount = 0;
    _ColCount = 0;
    _Data = NULL;
    Allocate(M.RowCount(), M.ColCount());
    memcpy(_Data, M._Data, sizeof(T) * M.RowCount() * M.ColCount());
}

template<class T>
DenseMatrix<T>::DenseMatrix(const SparseMatrix<T> &M)
{
    _RowCount = 0;
    _ColCount = 0;
    _Data = NULL;
    Allocate(M.RowCount(), M.ColCount());
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            Cell(Row, Col) = M.GetElement(Row, Col);
        }
    }
}

template<class T>
DenseMatrix<T>::~DenseMatrix()
{
    FreeMemory();
}

template<class T>
void DenseMatrix<T>::FreeMemory()
{
    //_Data.FreeMemory();
    if(_Data != NULL)
    {
        delete[] _Data;
        _Data = NULL;
    }
    _RowCount = 0;
    _ColCount = 0;
}

template<class T>
void DenseMatrix<T>::Allocate(UINT RowCount, UINT ColCount)
{
    if(_RowCount != RowCount || _ColCount != ColCount)
    {
        _RowCount = RowCount;
        _ColCount = ColCount;
        _Data = new T[_RowCount * _ColCount];
        //_Data.Allocate(_RowCount * _ColCount);
    }
}

template<class T>
void DenseMatrix<T>::WriteToStream(ostream &os, char Delimeter) const
{
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            os << Cell(Row, Col);
            if(Col != _ColCount - 1)
            {
                os << Delimeter;
            }
        }
        os << endl;
    }
}

template<class T>
void DenseMatrix<T>::WriteMathematicaToStream(ostream &os, const String &MatrixName) const
{
    os << setprecision(16) << setiosflags(ios::right | ios::fixed);
    os << MatrixName << "={" << endl;
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        os << " {";
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            os << Cell(Row, Col);
            if(Col != _ColCount - 1)
            {
                os << ",";
            }
        }
        if(Row == _RowCount - 1)
        {
            os << "}};" << endl;
        }
        else
        {
            os << "}," << endl;
        }
    }
}

template<class T>
void DenseMatrix<T>::SaveToMATLAB(const String &Filename) const
{
    ofstream File(Filename.CString());
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            File << Cell(Row, Col);
            if(Col != _ColCount - 1)
            {
                File << '\t';
            }
        }
        File << endl;
    }
}

template<class T>
void DenseMatrix<T>::LoadFromFile(const String &Filename)
{
    Vector<String> Lines, Words;
    Utility::GetFileLines(Filename, Lines);

    UINT TargetColCount = 0;
    Vector<T> AllData;
    for(UINT LineIndex = 0; LineIndex < Lines.Length(); LineIndex++)
    //for(UINT LineIndex = 0; LineIndex < 1000; LineIndex++)
    {
        const String &CurLine = Lines[LineIndex];
        if(CurLine.Length() > 2)
        {
            CurLine.Partition(' ', Words);
            if(TargetColCount == 0)
            {
                TargetColCount = Words.Length();
            }
            else
            {
                Assert(TargetColCount == Words.Length(), "Invalid file in DenseMatrix<T>::LoadFromFile");
            }
            for(UINT WordIndex = 0; WordIndex < TargetColCount; WordIndex++)
            {
                AllData.PushEnd(T(Words[WordIndex].ConvertToDouble()));
            }
        }
    }
    PersistentAssert(TargetColCount != 0 && AllData.Length() > 0, "Invalid file in DenseMatrix<T>::LoadFromFile");
    Allocate(AllData.Length() / TargetColCount, TargetColCount);
    memcpy(_Data, AllData.CArray(), sizeof(T) * AllData.Length());
    /*for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {

        }
    }*/
}

template<class T>
void DenseMatrix<T>::ExtractRow(UINT Row, Vector<T> &Result)
{
    Result.Allocate(_ColCount);
    for(UINT Col = 0; Col < _ColCount; Col++)
    {
        Result[Col] = _Data[Row * _ColCount + Col];
    }
}

template<class T>
void DenseMatrix<T>::ExtractCol(UINT Col, Vector<T> &Result)
{
    Result.Allocate(_RowCount);
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        Result[Row] = _Data[Row * _ColCount + Col];
    }
}

template<class T>
Vector<T> DenseMatrix<T>::ExtractRow(UINT Row)
{
    Vector<T> Result(_ColCount);
    for(UINT Col = 0; Col < _ColCount; Col++)
    {
        Result[Col] = _Data[Row * _ColCount + Col];
    }
    return Result;
}

template<class T>
Vector<T> DenseMatrix<T>::ExtractCol(UINT Col)
{
    Vector<T> Result(_RowCount);
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        Result[Row] = _Data[Row * _ColCount + Col];
    }
    return Result;
}

template<class T>
void DenseMatrix<T>::Clear(T Value)
{
    for(UINT Index = 0; Index < _RowCount * _ColCount; Index++)
    {
        _Data[Index] = Value;
    }
}

template<class T>
DenseMatrix<T>& DenseMatrix<T>::operator = (const DenseMatrix<T>&M)
{
    Allocate(M.RowCount(), M.ColCount());
    memcpy(_Data, M._Data, sizeof(T) * M.RowCount() * M.ColCount());
    return (*this);
}

template<class T>
DenseMatrix<T> DenseMatrix<T>::OuterProduct(const Vector<T> &A, const Vector<T> &B)
{
    Assert(A.Length() == B.Length() && A.Length() != 0, "Invalid vector dimensions in DenseMatrix<T>::OuterProduct");
    const UINT Size = A.Length();

    DenseMatrix Result, AMat(Size, 1), BMat(1, Size);
    for(UINT Index = 0; Index < Size; Index++)
    {
        AMat[Index][0] = A[Index];
        BMat[0][Index] = B[Index];
    }
    DenseMatrix::Multiply(Result, AMat, BMat);
    return Result;
}

template<class T>
T DenseMatrix<T>::CompareMatrices(const DenseMatrix<T> &Left, const DenseMatrix<T> &Right)
{
    T Result = T(0.0);
    UINT RowCount = Left.RowCount();
    UINT ColCount = Left.ColCount();
    for(UINT Row = 0; Row < RowCount; Row++)
    {
        for(UINT Col = 0; Col < ColCount; Col++)
        {
            Result += Math::Abs(Left[Row][Col] - Right[Row][Col]);
        }
    }
    return Result;
}

template<class T>
void DenseMatrix<T>::LUSolve(Vector<T> &x, const Vector<T> &b)
{
    Assert(Square() && b.Length() == _RowCount, "Invalid paramater DenseMatrix<T>::LUSolve");

    Vector<T> y;
    x.Allocate(b.Length());
    y.Allocate(b.Length());

    DenseMatrix<T> L, U, Copy;
    Copy = *this;
    Copy.LUDecomposition(L, U);

    for(UINT i = 0; i < _RowCount; i++)
    {
        T Sum = 0.0f;
        for(UINT j = 0; j < i; j++)
        {
            Sum += L[i][j] * y[j];
        }
        y[i] = b[i] - Sum;
    }

    for(int i = _RowCount - 1; i >= 0; i--)
    {
        T Sum = 0.0;
        for(int j = i + 1; j < int(_RowCount); j++)
        {
            Sum += U[i][j] * x[j];
        }
        x[i] = (y[i] - Sum) / U[i][i];
    }
}

template<class T>
void DenseMatrix<T>::operator += (const DenseMatrix &M)
{
    Assert(M._RowCount == _RowCount && M._ColCount == _ColCount, "Invalid matrix dimensions");
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            (*this)[Row][Col] += M[Row][Col];
        }
    }
}

template<class T>
void DenseMatrix<T>::LUDecomposition(DenseMatrix<T> &L, DenseMatrix<T> &U)
{
    Assert(Square(), "DenseMatrix<T>::LUDecomposition called on non-square matrix");
    L.Identity(_RowCount);
    U.Identity(_RowCount);
    for(UINT k = 0; k < _RowCount; k++)
    {
        U[k][k] = Cell(k, k);
        for(UINT i = k; i < _RowCount; i++)
        {
            L[i][k] = Cell(i, k) / U[k][k];
            U[k][i] = Cell(k, i);
        }
        for(UINT i = k; i < _RowCount; i++)
        {
            for(UINT j = k; j < _RowCount; j++)
            {
                Cell(i, j) -= L[i][k] * U[k][j];
            }
        }
    }
}

template<class T>
bool DenseMatrix<T>::EigenSystemVictor(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const
{
    PersistentAssert(Square(), "Eigensystem requires a square matrix");
    const UINT N = _RowCount;
    std::complex<double> *A = new std::complex<double>[N * N];
	
    for(UINT Col = 0; Col < N; Col++)
	{
        for(UINT Row = 0; Row < N; Row++)
        {
			A[Col * N + Row] = std::complex<double>(Cell(Row, Col), 0.0);
		}
	}

    std::complex<double> *evals = new std::complex<double>[N];
	std::complex<double> *evecs = new std::complex<double>[N * N];
	size_t nrot;
	size_t MaxIterations = (int)(2 + 2.8 * log((double)N) / log(2.0));
	if(MaxIterations < 8)
    {
        MaxIterations = 8;
    }
    Console::WriteLine(String("Eigensystem solve, sweeps: ") + String(MaxIterations));
	RNP::Eigensystem(N, A, N, evals, evecs, N, MaxIterations, &nrot);
	
	Eigenvalues.Allocate(N);
    Eigenvectors.Allocate(N, N);
    for(UINT Col = 0; Col < N; Col++)
	{
        Eigenvalues[Col] = evals[Col].real();
        for(UINT Row = 0; Row < N; Row++)
        {
            Eigenvectors.Cell(Row, Col) = evecs[Col * N + Row].real();
        }
    }

    delete [] A;
	delete [] evecs;
	delete [] evals;

    return true;
}

template<class T>
bool DenseMatrix<T>::EigenSystemTNT(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const
{
    PersistentAssert(Square(), "Eigensystem requires a square matrix");
    const UINT N = _RowCount;
    
    TNT::Array2D<T> TNTMatrix(N, N);
    for(UINT Row = 0; Row < N; Row++)
    {
        for(UINT Col = 0; Col < N; Col++)
        {
            TNTMatrix[Row][Col] = Cell(Row, Col);
        }
    }
    JAMA::Eigenvalue<T> eigensystem(TNTMatrix);

    eigensystem.getV(TNTMatrix);
    Eigenvectors.Allocate(N, N);
    for(UINT Row = 0; Row < N; Row++)
    {
        for(UINT Col = 0; Col < N; Col++)
        {
            Eigenvectors(Row, N - 1 - Col) = TNTMatrix[Row][Col];
        }
    }

    TNT::Array1D<T> TNTArray(N);
    eigensystem.getRealEigenvalues(TNTArray);
    
    Eigenvalues.Allocate(N);
    for(UINT eigenvalueIndex = 0; eigenvalueIndex < N; eigenvalueIndex++)
    {
        Eigenvalues[eigenvalueIndex] = TNTArray[N - 1 - eigenvalueIndex];
    }

    return true;
}

#define VTK_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau)

#define VTK_MAX_ROTATIONS 40

template<class T>
bool DenseMatrix<T>::EigenSystem(Vector<T> &Eigenvalues, DenseMatrix<T> &Eigenvectors) const
{
    const UINT Dimension = _RowCount;
	Eigenvalues.Allocate(Dimension);
	Eigenvectors.Allocate(Dimension, Dimension);
	Vector<T*> EigenvectorList(Dimension);
	for(UINT DimensionIndex = 0; DimensionIndex < Dimension; DimensionIndex++)
	{
		EigenvectorList[DimensionIndex] = Eigenvectors[DimensionIndex];
	}
    return EigenSystem(Eigenvalues.CArray(), EigenvectorList.CArray());
}

template<class T>
String DenseMatrix<T>::EigenTest(const Vector<T> &Eigenvalues, const DenseMatrix<T> &Eigenvectors) const
{
    const UINT Dimension = _RowCount;
	Vector<const T*> EigenvectorList(Dimension);
	for(UINT DimensionIndex = 0; DimensionIndex < Dimension; DimensionIndex++)
	{
		EigenvectorList[DimensionIndex] = Eigenvectors[DimensionIndex];
	}
    return EigenTest(Eigenvalues.CArray(), EigenvectorList.CArray());
}

//
// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in Eigenvectors. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
//
// Code modified from VTK vtkJacobiN function
//
template<class T>
bool DenseMatrix<T>::EigenSystem(T *Eigenvalues, T **Eigenvectors) const
{
    PersistentAssert(Square() && _RowCount >= 2, "Invalid matrix dimensions");
    int i, j, k, iq, ip, numPos, n = int(_RowCount);
    double tresh, theta, tau, t, sm, s, h, g, c, tmp;
    double bspace[4], zspace[4];
    double *b = bspace;
    double *z = zspace;

    //
    // Jacobi iteration destroys the matrix so create a temporary copy
    //
    DenseMatrix<double> a(_RowCount);
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            a.Cell(Row, Col) = Cell(Row, Col);
        }
    }

    //
    // only allocate memory if the matrix is large
    //
    if (n > 4)
    {
        b = new double[n];
        z = new double[n]; 
    }

    //
    // initialize
    //
    for (ip=0; ip<n; ip++) 
    {
        for (iq=0; iq<n; iq++)
        {
            Eigenvectors[ip][iq] = 0.0;
        }
        Eigenvectors[ip][ip] = 1.0;
    }
    for (ip=0; ip<n; ip++) 
    {
        b[ip] = a[ip][ip];
        Eigenvalues[ip] = T(a[ip][ip]);
        z[ip] = 0.0;
    }

    // begin rotation sequence
    for (i=0; i<VTK_MAX_ROTATIONS; i++) 
    {
        sm = 0.0;
        for (ip=0; ip<n-1; ip++) 
        {
            for (iq=ip+1; iq<n; iq++)
            {
                sm += fabs(a[ip][iq]);
            }
        }
        if (sm == 0.0)
        {
            break;
        }

        if (i < 3)                                // first 3 sweeps
        {
            tresh = 0.2*sm/(n*n);
        }
        else
        {
            tresh = 0.0;
        }

        for (ip=0; ip<n-1; ip++) 
        {
            for (iq=ip+1; iq<n; iq++) 
            {
                g = T(100.0*fabs(a[ip][iq]));

                // after 4 sweeps
                if (i > 3 && (fabs(Eigenvalues[ip])+g) == fabs(Eigenvalues[ip])
                          && (fabs(Eigenvalues[iq])+g) == fabs(Eigenvalues[iq]))
                {
                    a[ip][iq] = 0.0;
                }
                else if (fabs(a[ip][iq]) > tresh) 
                {
                    h = Eigenvalues[iq] - Eigenvalues[ip];
                    if ( (fabs(h)+g) == fabs(h))
                    {
                        t = (a[ip][iq]) / h;
                    }
                    else 
                    {
                        theta = 0.5*h / (a[ip][iq]);
                        t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
                        if (theta < 0.0)
                        {
                            t = -t;
                        }
                    }
                    c = 1.0 / sqrt(1+t*t);
                    s = t*c;
                    tau = s/(1.0+c);
                    h = t*a[ip][iq];
                    z[ip] -= h;
                    z[iq] += h;
                    Eigenvalues[ip] -= T(h);
                    Eigenvalues[iq] += T(h);
                    a[ip][iq] = 0.0;

                    // ip already shifted left by 1 unit
                    for (j = 0;j <= ip-1;j++) 
                    {
                        VTK_ROTATE(a,j,ip,j,iq);
                    }
                    // ip and iq already shifted left by 1 unit
                    for (j = ip+1;j <= iq-1;j++) 
                    {
                        VTK_ROTATE(a,ip,j,j,iq);
                    }
                    // iq already shifted left by 1 unit
                    for (j=iq+1; j<n; j++) 
                    {
                        VTK_ROTATE(a,ip,j,iq,j);
                    }
                    for (j=0; j<n; j++) 
                    {
#pragma warning ( disable : 4244 )
                        VTK_ROTATE(Eigenvectors,j,ip,j,iq);
#pragma warning ( default : 4244 )
                    }
                }
            }
        }

        for (ip=0; ip<n; ip++) 
        {
            b[ip] += z[ip];
            Eigenvalues[ip] = T(b[ip]);
            z[ip] = 0.0;
        }
    }

    if ( i >= VTK_MAX_ROTATIONS )
    {
        Utility::MessageBox("VTK_MAX_ROTATIONS exceeded");
        //SignalError("VTK_MAX_ROTATIONS exceeded");
        //return false;
    }

    // sort eigenfunctions                 these changes do not affect accuracy 
    for (j=0; j<n-1; j++)                  // boundary incorrect
    {
        k = j;
        tmp = Eigenvalues[k];
        for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
        {
            if (Eigenvalues[i] >= tmp)                   // why exchage if same?
            {
                k = i;
                tmp = Eigenvalues[k];
            }
        }
        if (k != j) 
        {
            Eigenvalues[k] = Eigenvalues[j];
            Eigenvalues[j] = T(tmp);
            for (i=0; i<n; i++) 
            {
                tmp = Eigenvectors[i][j];
                Eigenvectors[i][j] = Eigenvectors[i][k];
                Eigenvectors[i][k] = T(tmp);
            }
        }
    }

    //
    // insure eigenvector consistency (i.e., Jacobi can compute vectors that
    // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
    // reek havoc in hyperstreamline/other stuff. We will select the most
    // positive eigenvector.
    //
    int ceil_half_n = (n >> 1) + (n & 1);
    for (j=0; j<n; j++)
    {
        for (numPos=0, i=0; i<n; i++)
        {
            if ( Eigenvectors[i][j] >= 0.0 )
            {
                numPos++;
            }
        }
        //    if ( numPos < ceil(double(n)/double(2.0)) )
        if ( numPos < ceil_half_n)
        {
            for(i=0; i<n; i++)
            {
                Eigenvectors[i][j] *= -1.0;
            }
        }
    }

    if (n > 4)
    {
        delete [] b;
        delete [] z;
    }
    return true;
}

template<class T>
String DenseMatrix<T>::EigenTest(const T *Eigenvalues, const T **Eigenvectors) const
{
    const bool Verbose = false;
    String Description;
    PersistentAssert(Square() && _RowCount >= 2, "Invalid matrix dimensions");
    Vector<T> Eigenvector(_RowCount), Result;
    double MaxError = 0.0;
    for(UINT EigenIndex = 0; EigenIndex < _RowCount; EigenIndex++)
    {
        for(UINT ElementIndex = 0; ElementIndex < _RowCount; ElementIndex++)
        {
            Eigenvector[ElementIndex] = Eigenvectors[ElementIndex][EigenIndex];
        }
        DenseMatrix<T>::Multiply(Result, *this, Eigenvector);
        
        double Error = 0.0;
        T CurEigenvalue = Eigenvalues[EigenIndex];
        for(UINT ElementIndex = 0; ElementIndex < _RowCount; ElementIndex++)
        {
            Error += fabs(Eigenvector[ElementIndex] * CurEigenvalue - Result[ElementIndex]);
        }
        if(Verbose)
        {
            Description += String("Eigenvector ") + String(EigenIndex) + String(" absolute error: ") + String(Error) + String("\n");
        }
        MaxError = Math::Max(Error, MaxError);
    }
    Description += String("Max eigenvector error: ") + String(MaxError) + String("\n");
    return Description;
}

template<class T>
T DenseMatrix<T>::Determinant() const
{
    Assert(Square(), "Determinant called on non-square matrix");
    if(_RowCount == 1)
    {
        return Cell(0, 0);
    }
    else if(_RowCount == 2)
    {
        return Cell(0, 0) * Cell(1, 1) - Cell(1, 0) * Cell(0, 1);
    }
    else if(_RowCount == 3)
    {
        return (Cell(0, 0) * Cell(1, 1) * Cell(2, 2) + Cell(0, 1) * Cell(1, 2) * Cell(2, 0) + Cell(0, 2) * Cell(1, 0) * Cell(2, 1)) -
               (Cell(2, 0) * Cell(1, 1) * Cell(0, 2) + Cell(2, 1) * Cell(1, 2) * Cell(0, 0) + Cell(2, 2) * Cell(1, 0) * Cell(0, 1));
    }
    else
    {
		return DeterminantLaplaceExpansion();
    }
}

template<class T>
T DenseMatrix<T>::DeterminantLaplaceExpansion() const
{
	// Recursive computation via Laplace Expansion along the first row
	T det = 0.0;
	UINT i = 0;
	DenseMatrix<T> minor;
	for (UINT j = 0; j < _ColCount; j++)
	{
		T b = Cell(i, j);
		if (b == (T)0.0) continue;
		int coeff = ((i+j) % 2 == 0 ? 1 : -1);
		MinorMatrix(i, j, minor);
		det += b * coeff * minor.Determinant();
	}
	return det;
}

template<class T>
void DenseMatrix<T>::MinorMatrix(UINT Row, UINT Col, DenseMatrix<T>& Result) const
{
	Result.Allocate(_RowCount - 1, _ColCount - 1);
	UINT i = 0;
	for (UINT ii = 0; ii < _RowCount; ii++)
	{
		if (ii == Row) continue;
		UINT j = 0;
		for (UINT jj = 0; jj < _ColCount; jj++)
		{
			if (jj == Col) continue;
			Result(i,j) = Cell(ii,jj);
			j++;
		}
		i++;
	}
}

template<class T>
DenseMatrix<T>DenseMatrix<T>::Inverse() const
{
    DenseMatrix<T>Result = *this;
    Result.InvertInPlace();
    return Result;
}

template<class T>
void DenseMatrix<T>::InvertInPlace()
{
    Assert(Square(), "DenseMatrix<T>::Invert called on non-square matrix");
    for (UINT i = 1; i < _RowCount; i++)
    {
        Cell(0, i) /= Cell(0, 0);
    }

    for (UINT i = 1; i < _RowCount; i++)
    {
        //
        // do a column of L
        //
        for (UINT j = i; j < _RowCount; j++)
        {
            T Sum = 0.0f;
            for (UINT k = 0; k < i; k++)  
            {
                Sum += Cell(j, k) * Cell(k, i);
            }
            Cell(j, i) -= Sum;
        }
        if (i == _RowCount - 1)
        {
            continue;
        }

        //
        // do a row of U
        //
        for (UINT j = i + 1; j < _RowCount; j++)
        {
            T Sum = 0.0f;
            for (UINT k = 0; k < i; k++)
                Sum += Cell(i, k) * Cell(k, j);
            Cell(i, j) = (Cell(i, j) - Sum) / Cell(i, i);
        }
    }

    //
    // invert L
    //
    for (UINT i = 0; i < _RowCount; i++)
        for (UINT j = i; j < _RowCount; j++)
        {
            T Sum = 1.0f;
            if ( i != j )
            {
                Sum = 0.0f;
                for (UINT k = i; k < j; k++ ) 
                {
                    Sum -= Cell(j, k) * Cell(k, i);
                }
            }
            Cell(j, i) = Sum / Cell(j, j);
        }

    //
    // invert U
    //
    for (UINT i = 0; i < _RowCount; i++)
        for (UINT j = i; j < _RowCount; j++)
        {
            if ( i == j )
            {
                continue;
            }
            T Sum = 0.0f;
            for (UINT k = i; k < j; k++)
            {
                T Val = 1.0f;
                if(i != k)
                {
                    Val = Cell(i, k);
                }
                Sum += Cell(k, j) * Val;
            }
            Cell(i, j) = -Sum;
        }
    
    //
    // final inversion
    //
    for (UINT i = 0; i < _RowCount; i++)
    {
        for (UINT j = 0; j < _RowCount; j++)
        {
            T Sum = 0.0f;
            UINT Larger = j;
            if(i > j)
            {
                Larger = i;
            }
            for (UINT k = Larger; k < _RowCount; k++)
            {
                T Val = 1.0f;
                if(j != k)
                {
                    Val = Cell(j, k);
                }
                Sum += Val * Cell(k, i);
            }
            Cell(j, i) = Sum;
        }
    }
    //Assert(ElementsValid(), "Degenerate Matrix inversion.");
}

template<class T>
void DenseMatrix<T>::Identity()
{
    Assert(Square(), "DenseMatrix<T>::Identity called on non-square Matrix4.");
    for(UINT i = 0; i < _RowCount; i++)
    {
        for(UINT i2 = 0; i2 < _RowCount; i2++)
        {
            if(i == i2)
            {
                Cell(i, i2) = 1.0f;
            }
            else
            {
                Cell(i, i2) = 0.0f;
            }
        }
    }
}

template<class T>
void DenseMatrix<T>::Identity(UINT Size)
{
    Allocate(Size, Size);
    Identity();
}

template<class T>
bool DenseMatrix<T>::Square() const
{
    return (_RowCount == _ColCount);
}

template<class T>
bool DenseMatrix<T>::ElementsValid()
{
    for(UINT Row = 0; Row < _RowCount; Row++)
    {
        for(UINT Col = 0; Col < _ColCount; Col++)
        {
            if(Cell(Row, Col) != Cell(Row, Col))
            {
                return false;
            }
        }
    }
    return true;
}

template<class T>
DenseMatrix<T> DenseMatrix<T>::Transpose()
{
    DenseMatrix<T> Result;
    Result.Allocate(_ColCount, _RowCount);
    for(UINT i = 0; i < _RowCount; i++)
    {
        for(UINT i2 = 0; i2 < _ColCount; i2++)
        {
            Result.Cell(i2, i) = Cell(i, i2);
        }
    }
    return Result;
}

/*ostream& operator << (ostream &os, const DenseMatrix<T>&m)
{
    UINT n = m.RowCount();
    //os << setprecision(8) << setiosflags(ios::right | ios::showpoint);
    os << "{";
    for(UINT i=0;i<n;i++)
    {
        os << "{";
        for(UINT i2=0;i2<n;i2++)
        {
            os << m[i][i2];
            if(i2 != n - 1) os << ", ";
        }
        
        os << "}";
        if(i != n - 1) os << ", ";
        os << endl;
    }
    os << "}";
    return os;
}*/

template<class T>
ostream& operator << (ostream &os, const DenseMatrix<T>&m)
{
    Assert(m.Square(), "Output not correct for non-square matrices.");
    UINT n = m.RowCount();
    os << setprecision(8) << setiosflags(ios::right | ios::showpoint);
    for(UINT i = 0; i < n; i++)
    {
        for(UINT i2 = 0; i2 < n; i2++)
        {
            os << setw(12) << m[i][i2];
        }
        os << endl;
    }
    return os;
}

template<class T>
void DenseMatrix<T>::Multiply(Vector<T> &Result, const DenseMatrix<T>&Left, const Vector<T> &Right)
{
    Assert(Left.ColCount() == Right.Length(), "Invalid dimensions in DenseMatrix<T>::Multiply");
    Result.Allocate(Left.RowCount());

    for(UINT Row = 0; Row < Result.Length(); Row++)
    {
        T Total = 0.0;
        for(UINT Index = 0; Index < Left.ColCount(); Index++)
        {
            Total += Left.Cell(Row, Index) * Right[Index];
        }
        Result[Row] = Total;
    }
}

template<class T>
void DenseMatrix<T>::Multiply(T *result, const DenseMatrix<T>&left, const T *right)
{
    const UINT colCount = left.ColCount();
    const UINT rowCount = left.RowCount();
    for(UINT row = 0; row < rowCount; row++)
    {
        T total = 0.0;
        for(UINT col = 0; col < colCount; col++)
        {
            total += left(row, col) * right[col];
        }
        result[row] = total;
    }
}

template<class T>
void DenseMatrix<T>::Multiply(DenseMatrix<T>&Result, const DenseMatrix<T>&Left, const DenseMatrix<T>&Right)
{
    Assert(Left.ColCount() == Right.RowCount(), "Invalid matrix dimensions on SetProduct");
    const UINT RowCount = Left.RowCount();
    const UINT ColCount = Right.ColCount();
    Result.Allocate(RowCount, ColCount);
    for(UINT RowIndex = 0; RowIndex < RowCount; RowIndex++)
    {
        for(UINT ColIndex = 0; ColIndex < ColCount; ColIndex++)
        {
            T Total = 0.0;
            for(UINT InnerIndex = 0; InnerIndex < Left.ColCount(); InnerIndex++)
            {
                Total += Left.Cell(RowIndex, InnerIndex) * Right.Cell(InnerIndex, ColIndex);
            }
            Result[RowIndex][ColIndex] = Total;
        }
    }
}

/*void SparseMatrix::Multiply(SparseMatrix &A, const SparseMatrix &B, const SparseMatrix &C)
{
    Assert(B.ColCount() == C.RowCount(), "Invalid matrix multiply dimensions.");
    A.Allocate(B.RowCount(), C.ColCount());
    

    for(UINT BRow = 0; BRow < B.RowCount(); BRow++)
    {
        UINT BRowLength = B.Rows()[BRow].Data.Length();
        for(UINT BColIndex = 0; BColIndex < BRowLength; BColIndex++)
        {
            const SparseElement *BElement = &B.Rows()[BRow].Data[BColIndex];
            UINT j = BElement->Col;
            T BEntry = BElement->Entry;

            UINT CRowLength = C.Rows()[j].Data.Length();
            for(UINT CRowIndex = 0; CRowIndex < CRowLength; CRowIndex++)
            {
                const SparseElement *CElement = &C.Rows()[j].Data[CRowIndex];
                UINT k = CElement->Col;
                T CEntry = CElement->Entry;

                A.PushElement(BRow, k, BEntry * CEntry);
            }
        }
    }
}*/

template<class T>
void DenseMatrix<T>::Multiply(DenseMatrix<T>&Result, const SparseMatrix<T> &Left, const DenseMatrix<T> &Right)
{
    Assert(Left.ColCount() == Right.RowCount(), "Invalid matrix dimensions");
    const UINT RowCount = Left.RowCount();
    const UINT ColCount = Right.ColCount();
    const UINT InnerCount = Left.ColCount();
    Result.Allocate(RowCount, ColCount);
    Result.Clear(0.0);

    for(UINT RowIndex = 0; RowIndex < RowCount; RowIndex++)
    {
        const Vector< SparseElement<T> > &LeftRowEntries = Left.Rows()[RowIndex].Data;
        const UINT LeftRowEntryCount = LeftRowEntries.Length();
        for(UINT LeftColIndex = 0; LeftColIndex < LeftRowEntryCount; LeftColIndex++)
        {
            const SparseElement<T> &CurLeftEntry = LeftRowEntries[LeftColIndex];

            for(UINT RightColIndex = 0; RightColIndex < ColCount; RightColIndex++)
            {
                Result.Cell(RowIndex, ColCount) += CurLeftEntry.Entry * Right.Cell(CurLeftEntry.Col, RightColIndex);
            }
        }
    }
}

template<class T>
void DenseMatrix<T>::Multiply(DenseMatrix<T>&Result, const DenseMatrix<T>&Left, const SparseMatrix<T> &Right)
{
    Assert(Left.ColCount() == Right.RowCount(), "Invalid matrix dimensions");
    const UINT RowCount = Left.RowCount();
    const UINT ColCount = Right.ColCount();
    const UINT InnerCount = Left.ColCount();
    Result.Allocate(RowCount, ColCount);
    Result.Clear(0.0);

    for(UINT RowIndex = 0; RowIndex < RowCount; RowIndex++)
    {
        for(UINT LeftColIndex = 0; LeftColIndex < InnerCount; LeftColIndex++)
        {
            T LeftEntry = Left.Cell(RowIndex, LeftColIndex);

            const Vector<SparseElement> &RightRowEntries = Right.Rows()[LeftColIndex].Data;
            const UINT RightRowEntryCount = RightRowEntries.Length();
            for(UINT RightRowIndex = 0; RightRowIndex < RightRowEntryCount; RightRowIndex++)
            {
                const SparseElement &CurRightEntry = RightRowEntries[RightRowIndex];
                Result.Cell(RowIndex, CurRightEntry.Col) += LeftEntry * CurRightEntry.Entry;
            }
        }
    }
}

template<class T>
void DenseMatrix<T>::MultiplyMMTranspose(DenseMatrix<T>&Result, const DenseMatrix<T>&M)
{
    const UINT RowCount = M.RowCount();
    const UINT ColCount = M.ColCount();
    Result.Allocate(RowCount, RowCount);
    for(UINT i = 0; i < RowCount; i++)
    {
        for(UINT i2 = 0; i2 < RowCount; i2++)
        {
            T Total = 0.0;
            for(UINT i3 = 0; i3 < ColCount; i3++)
            {
                Total += M.Cell(i, i3) * M.Cell(i2, i3);
            }
            Result.Cell(i, i2) = Total;
        }
    }
}

template<class T>
void DenseMatrix<T>::MultiplyMMTranspose(DenseMatrix<T> &Result, const SparseMatrix<T> &M)
{
    const UINT RowCount = M.RowCount();
    const UINT ColCount = M.ColCount();
    Result.Allocate(RowCount, RowCount);
    for(UINT i = 0; i < RowCount; i++)
    {
        for(UINT i2 = 0; i2 < RowCount; i2++)
        {
            T Total = 0.0;
            for(UINT i3 = 0; i3 < ColCount; i3++)
            {
                Total += M.GetElement(i, i3) * M.GetElement(i2, i3);
            }
            Result.Cell(i, i2) = Total;
        }
    }
}

template<class T>
void DenseMatrix<T>::MultiplyInPlace(DenseMatrix<T>&Result, T Right)
{
    for(UINT Row = 0; Row < Result.RowCount(); Row++)
    {
        for(UINT Col = 0; Col < Result.ColCount(); Col++)
        {
            Result[Row][Col] *= Right;
        }
    }
}

template<class T>
DenseMatrix<T>operator * (const DenseMatrix<T>&Left, const DenseMatrix<T>&Right)
{
    DenseMatrix<T>Result;
    DenseMatrix<T>::Multiply(Result, Left, Right);
    return Result;
}

template<class T>
DenseMatrix<T>operator * (const SparseMatrix<T> &Left, const DenseMatrix<T>&Right)
{
    DenseMatrix<T>Result;
    DenseMatrix<T>::Multiply(Result, Left, Right);
    return Result;
}

template<class T>
DenseMatrix<T>operator * (const DenseMatrix<T>&Left, const SparseMatrix<T> &Right)
{
    DenseMatrix<T>Result;
    DenseMatrix<T>::Multiply(Result, Left, Right);
    return Result;
}

template<class T>
DenseMatrix<T>operator * (const DenseMatrix<T>&Left, T Right)
{
    DenseMatrix<T>Return(Left.RowCount(), Left.ColCount());
    for(UINT Row = 0; Row < Left.RowCount(); Row++)
    {
        for(UINT Col = 0; Col < Left.ColCount(); Col++)
        {
            Return[Row][Col] = Left[Row][Col] * Right;
        }
    }
    return Return;
}

template<class T>
DenseMatrix<T>operator * (T Right, const DenseMatrix<T>&Left)
{
    DenseMatrix<T>Return(Left.RowCount(), Left.ColCount());
    for(UINT Row = 0; Row < Left.RowCount(); Row++)
    {
        for(UINT Col = 0; Col < Left.ColCount(); Col++)
        {
            Return[Row][Col] = Left[Row][Col] * Right;
        }
    }
    return Return;
}

template<class T>
DenseMatrix<T>operator + (const DenseMatrix<T>&Left, const DenseMatrix<T>&Right)
{
    Assert(Left.RowCount() == Right.RowCount() && Left.ColCount() == Right.ColCount(), "Invalid dimensions on Matrix4 addition.");
    DenseMatrix<T>Return(Left.RowCount(), Left.ColCount());
    for(UINT Row = 0; Row < Left.RowCount(); Row++)
    {
        for(UINT Col = 0; Col < Left.ColCount(); Col++)
        {
            Return[Row][Col] = Left[Row][Col] + Right[Row][Col];
        }
    }
    return Return;
}

template<class T>
DenseMatrix<T>operator - (const DenseMatrix<T>&Left, const DenseMatrix<T>&Right)
{
    Assert(Left.RowCount() == Right.RowCount() && Left.ColCount() == Right.ColCount(), "Invalid dimensions on Matrix4 addition.");
    DenseMatrix<T>Return(Left.RowCount(), Left.ColCount());
    for(UINT Row = 0; Row < Left.RowCount(); Row++)
    {
        for(UINT Col = 0; Col < Left.ColCount(); Col++)
        {
            Return[Row][Col] = Left[Row][Col] - Right[Row][Col];
        }
    }
    return Return;
}

namespace Math
{
    __forceinline void ReorientBasis(Vec3f &_V1, const Vec3f &_V2, const Vec3f &_V3)
    {
        Vec3f V1 = Vec3f::Normalize(_V1);
        Vec3f V2 = Vec3f::Normalize(_V2);
        Vec3f V3 = Vec3f::Normalize(_V3);

        DenseMatrix<float>M(3);
        
        M[0][0] = V1.x;
        M[1][0] = V1.y;
        M[2][0] = V1.z;

        M[0][1] = V2.x;
        M[1][1] = V2.y;
        M[2][1] = V2.z;

        M[0][2] = V3.x;
        M[1][2] = V3.y;
        M[2][2] = V3.z;

        if(M.Determinant() < 0.0f)
        {
            _V1 = -V1;
        }
    }
}