template <class T>
class PCA
{
public:
    void InitFromDensePoints(const Vector<const T*> &Points, UINT Dimension);
    //void InitFromPointMatrix(const SparseMatrix<T> &B);
    void InitFromPointMatrix(DenseMatrix<T> &B);
    void InitFromMATLAB(UINT Dimension, const String &EigenvectorFilename, const String &EigenvalueFilename, const String &MeanFilename);
	
    void SaveToFile(const String &filename) const;
    void LoadFromFile(const String &filename);

    UINT ReducedDimension(double EnergyPercent);
	
    void Transform(Vector<T> &Result, const Vector<T> &Input, UINT ReducedDimension);
	void InverseTransform(Vector<T> &Result, const Vector<T> &Input);
	
    void Transform(T *Result, const T *Input, UINT ReducedDimension);
	void InverseTransform(T *Result, const T *Input, UINT ReducedDimension);

private:
    void InitFromCorrelationMatrix(const DenseMatrix<T> &M);
    void FinalizeFromEigenSystem();

	Vector<T> _Means;
	Vector<T> _Eigenvalues;
	DenseMatrix<T> _Eigenvectors;
};

#include "PCA.cpp"
