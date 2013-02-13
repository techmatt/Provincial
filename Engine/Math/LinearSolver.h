/*
LinearSolver.h
Written by Matthew Fisher

LinearSolver is an abstract class used to solve sparse linear systems.
These are problems of the form Mx = b, where M is a known matrix, b is a known column vector,
and x is the unknown column vector.  This class is implemented using conjugate gradient descent,
bi-conjugate gradient descent, and TAUCS.
*/

template<class T>
class LinearSolver
{
public:
    LinearSolver();
    virtual void FreeMemory() = 0;

    virtual void Factor() = 0;
    virtual void Solve(Vector<T> &x, const Vector<T> &b) = 0;
    
    double ComputeError(const Vector<T> &x, const Vector<T> &b) const;
    
    void LoadMatrix(const SparseMatrix<T> *NewM);
    const String& GetOutputString() const;

protected:
    const SparseMatrix<T> *M;
    String OutputString;
};

template<class T>
class CGLinearSolver : public LinearSolver<T>
{
public:
    CGLinearSolver();
    ~CGLinearSolver();
    void FreeMemory();

    void SetParamaters(UINT MaxIters, double Tolerance);

    void Factor();
    void Solve(Vector<T> &x, const Vector<T> &b);

private:
    void LoadDiagonalTerms();
    void AllocateSpace();

    UINT _MaxIters;
    double _Tolerance;
    Vector<T> p, q, r, z, Temp, DiagTerms;
};

template<class T>
class BiCGLinearSolver : public LinearSolver<T>
{
public:
    BiCGLinearSolver();
    ~BiCGLinearSolver();
    void FreeMemory();

    void SetParamaters(UINT MaxIters, double Tolerance);

    void Factor();
    void Solve(Vector<T> &x, const Vector<T> &b);

private:
    void LoadDiagonalTerms();
    void AllocateSpace();
    
    UINT _MaxIters;
    double _Tolerance;
    Vector<T> p, p2, q, q2, r, r2, z, z2, Temp, x, b, DiagTerms;
};

#ifdef USE_TAUCS
template<class T>
class TaucsSymmetricLinearSolver : public LinearSolver<T> 
{
public:
    TaucsSymmetricLinearSolver();
    ~TaucsSymmetricLinearSolver();
    void FreeMemory();
    void Factor();
    void Solve(Vector<T> &x, const Vector<T> &b);

private:
    void LoadDiagonalTerms();
    void AllocateSpace();

    taucs_ccs_matrix    *PermutedM, *TaucsM;
    void                *ETree;
    taucs_ccs_matrix    *LTriangular;
    int                 *Permutation, *InversePermutation;
};
#endif

#include "LinearSolver.cpp"
