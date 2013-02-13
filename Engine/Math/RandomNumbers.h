namespace Math
{
    __forceinline UINT64 RandomUInt64()
    {
        UINT64 result;
        BYTE *bytes = (BYTE *)&result;
        for(UINT byteIndex = 0; byteIndex < sizeof(UINT64); byteIndex++)
        {
            bytes[byteIndex] = rand() & 255;
        }
        return result;
    }

	template<class T>
	T UniformRandom()
	{
		// TODO: Replace with a better thing (Mersenne twister?) at some point...
		return rnd();
	}

	template<class T>
	T UniformRandom(T minval, T maxval)
	{
		T t = UniformRandom<T>();
		return (1-t)*minval + t*maxval;
	}

	template<class T>
	UINT MultinomialRandom(const Vector<T>& params)
	{
		UINT result = 0;
		T x = UniformRandom<T>();
		T probAccum = (T) 1e-6;		// Small episilon to avoid numerical issues
		UINT k = params.Length();
		for (; result < k; result++)
		{
			probAccum += params[result];
			if (x <= probAccum) break;
		}
		return result;
	}

	template<class T>
	T GaussianRandom()
	{
		// Box-Muller method
		T minval = (T)(0.0) + std::numeric_limits<T>::min();
		T maxval = (T)(1.0) - std::numeric_limits<T>::min();
		T u = UniformRandom<T>(minval, maxval);
		T v = UniformRandom<T>(minval, maxval);
		T result = (T)(sqrt(-2 * log(u)) * cos(2*PI*v));
		
		// Note: sqrt(-2 * log(u)) * sin(2*PI*v) is also a valid choice

		return result;
	}

	template<class T>
	T GaussianRandom(T mean, T sd)
	{
		return mean + sd*GaussianRandom<T>();
	}

	template<class T>
	void GaussianRandomMultivar(Vector<T>& outSample, UINT dim)
	{
		outSample.ReSize(dim);
		for (UINT i = 0; i < dim; i++)
			outSample[i] = GaussianRandom<T>();
	}

	template<class T>
	void GaussianRandomMultivar(const Vector<T>& mean, const DenseMatrix<T>& sigma, Vector<T>& outSample)
	{
		// Draw sample Z from standard normal distribution
		Vector<T> Z;
		UINT dim = mean.Length();
		GaussianRandomMultivar<T>(Z, dim);

		// Compute A = U(S^0.5), where sigma = US(U^T)
		Vector<T> eigvals; DenseMatrix<T> eigvecs;
		sigma.EigenSystem(eigvals, eigvecs);
		DenseMatrix<T> diag(dim, dim); diag.Clear(0.0);
		for (UINT i = 0; i < dim; i++)
			diag(i,i) = sqrt(eigvals[i]);
		DenseMatrix<T> A(dim, dim);
		DenseMatrix<T>::Multiply(A, eigvecs, diag);

		// Finalize sample as mean + A*Z
		DenseMatrix<T>::Multiply(outSample, A, Z);
		for (UINT i = 0; i < dim; i++)
			outSample[i] += mean[i];
	}

	template<class T>
	void GaussianRandomMultivar(const Vector<T>& mean, const DenseMatrix<T>& sigma, Vector< Vector<T> >& outSamples, UINT numSamples)
	{
		UINT dim = mean.Length();

		// Compute A = U(S^0.5), where sigma = US(U^T)
		Vector<T> eigvals; DenseMatrix<T> eigvecs;
		sigma.EigenSystem(eigvals, eigvecs);
		DenseMatrix<T> diag(dim, dim); diag.Clear(0.0);
		for (UINT i = 0; i < dim; i++)
			diag(i,i) = sqrt(eigvals[i]);
		DenseMatrix<T> A(dim, dim);
		DenseMatrix<T>::Multiply(A, eigvecs, diag);

		UINT currSize = outSamples.Length();
		outSamples.ReSize(currSize + numSamples);
		for (UINT samp = 0; samp < numSamples; samp++)
		{
			// Draw sample Z from standard normal distribution
			Vector<T> Z;
			GaussianRandomMultivar<T>(Z, dim);

			// Finalize sample as mean + A*Z
			Vector<T>& outSample = outSamples[currSize+samp];
			DenseMatrix<T>::Multiply(outSample, A, Z);
			for (UINT i = 0; i < dim; i++)
				outSample[i] += mean[i];
		}
	}
}