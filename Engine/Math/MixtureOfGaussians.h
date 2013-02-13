template<class T>
class MixtureOfGaussians
{
public:
    static const bool debuggingMixtureOfGaussians = false;

	typedef Vector<T> DataPoint;

	/** Callback declaration **/
	class Callback
	{
	public:
		virtual void operator()(UINT trialNum, UINT iterationNum) = 0;
	};

	MixtureOfGaussians();

	/** Main methods people care about **/
	void Report();
	T EM(const Vector<DataPoint>& data, UINT numGaussians, UINT numTrials = 1, T minDelta = 0.0, bool verbose = false);
	T EvaluateAt(const Vector<T>& point) const;
	void Sample(Vector<T>& outSample) const;

	/** Accessors, etc. **/
	UINT Dimension() const { return _dimension; }
	UINT NumGaussians() const { return _numGaussians; }
	const Vector<T>& Phis() const { return _phis; }
	const Vector< Vector<T> >& Mus() const { return _mus; }
	const Vector < DenseMatrix<T> >& Sigmas() const { return _sigmas; }
	void RegisterTrainingCallback(Callback* callback) { _trainingCallback = callback; }
	void DeregisterTrainingCallback() { _trainingCallback = NULL; }

	/** A bunch of static utilities for vector arithmetic **/
	static void AddInPlace(Vector<T>& v1, const Vector<T>& v2);
	static void Subtract(const Vector<T>& v1, const Vector<T>& v2, Vector<T>& outVec);
	static T InnerProduct(const Vector<T>& v1, const Vector<T>& v2);
	static void OuterProduct(const Vector<T>& v1, const Vector<T>& v2, DenseMatrix<T>& outMat);

	/** Probability 'n stuff **/
	static T GaussianMultivar(const Vector<T>& x, const Vector<T>& mu, const DenseMatrix<T>& sigma);
	T LogLikelihood(const Vector< Vector<T> >& data);

	friend OutputDataStream& operator << <T> (OutputDataStream& s, const MixtureOfGaussians<T>& gmm);
	friend InputDataStream& operator >> <T> (InputDataStream& s, MixtureOfGaussians<T>& gmm);

private:

	T GaussianMultivar(const Vector<T>& x, UINT k) const;
    T GaussianMultivarFast(const Vector<T>& x, UINT k) const;
	T EvalDensity(const DataPoint& dataPoint, UINT gaussianI) const;
	void UpdateSigmaQuantities();
	void InitializeParams();
	void EStep(Vector< Vector<T> >& ws);
	void MStep(const Vector< Vector<T> >& ws);
	void InitializeWeights(Vector< Vector<T> >& ws);
	T LogLikelihood();

	Vector<DataPoint> _data;

	UINT _numDataPoints;
	UINT _dimension;
	UINT _numGaussians;

	Vector<T> _phis;
	Vector< Vector<T> > _mus;
	Vector< DenseMatrix<T> > _sigmas;
	Vector< DenseMatrix<T> > _sigmaInvs;
	//Vector<T> _sigmaDets;
    Vector<T> _sigmaCoefficients;

	Callback* _trainingCallback;
};



template <class T>
MixtureOfGaussians<T>::MixtureOfGaussians()
	: _numDataPoints(0), _dimension(0), _numGaussians(0), _trainingCallback(NULL)
{
}

template <class T>
void MixtureOfGaussians<T>::AddInPlace(Vector<T>& v1, const Vector<T>& v2)
{
	if (v1.Length() != v2.Length())
	{
		String error = "MixtureOfGaussians::Add - attempt to add two vectors of different length";
		error += "\nv1.Length() = " + String(v1.Length());
		error += "\nv2.Length() = " + String(v2.Length());
		PersistentSignalError(error);
	}

	for (UINT i = 0; i < v1.Length(); i++)
		v1[i] += v2[i];
}

template <class T>
void MixtureOfGaussians<T>::Subtract(const Vector<T>& v1, const Vector<T>& v2, Vector<T>& outVec)
{
	if (v1.Length() != v2.Length())
	{
		String error = "MixtureOfGaussians::Subtract - attempt to subtract two vectors of different length";
		error += "\nv1.Length() = " + String(v1.Length());
		error += "\nv2.Length() = " + String(v2.Length());
		PersistentSignalError(error);
	}

    const UINT length = v1.Length();
	if(outVec.Length() != length) outVec.Allocate(length);
	for (UINT i = 0; i < length; i++)
		outVec[i] = v1[i] - v2[i];
}

template <class T>
T MixtureOfGaussians<T>::InnerProduct(const Vector<T>& v1, const Vector<T>& v2)
{
	if (v1.Length() != v2.Length())
	{
		String error = "MixtureOfGaussians::InnerProduct - attempt to inner product two vectors of different length";
		error += "\nv1.Length() = " + String(v1.Length());
		error += "\nv2.Length() = " + String(v2.Length());
		PersistentSignalError(error);
	}

    const UINT length = v1.Length();
	T result = 0.0;
	for (UINT i = 0; i < length; i++)
		result += v1[i]*v2[i];
	return result;
}

template <class T>
void MixtureOfGaussians<T>::OuterProduct(const Vector<T>& v1, const Vector<T>& v2, DenseMatrix<T>& outMat)
{
	if (v1.Length() != v2.Length())
	{
		String error = "MixtureOfGaussians::OuterProduct - attempt to outer product two vectors of different length";
		error += "\nv1.Length() = " + String(v1.Length());
		error += "\nv2.Length() = " + String(v2.Length());
		PersistentSignalError(error);
	}

	outMat.Allocate(v1.Length(), v1.Length());
	for (UINT i = 0; i < v1.Length(); i++) for (UINT j = 0; j < v1.Length(); j++)
	{
		outMat(i, j) = v1[i]*v2[j];
	}
}

template <class T>
T MixtureOfGaussians<T>::GaussianMultivar(const Vector<T>& x, const Vector<T>& mu, const DenseMatrix<T>& sigma)
{
	UINT n = x.Length();
	T coeff = (T) (1.0 / ( pow(2*Math::PI, n/2.0) * sqrt(sigma.Determinant()) ) );
	Vector<T> xMinusMu;
	Subtract(x, mu, xMinusMu);
	Vector<T> xMinusMuOverSigma;
	DenseMatrix<T>::Multiply(xMinusMuOverSigma, sigma.Inverse(), xMinusMu);
	T exponent = (T)(-0.5 * InnerProduct(xMinusMu, xMinusMuOverSigma));
	return coeff * exp(exponent);
}

template <class T>
T MixtureOfGaussians<T>::GaussianMultivar(const Vector<T>& x, UINT k) const
{
	Vector<T> xMinusMu;
	Vector<T> xMinusMuOverSigma;
	UINT n = x.Length();
	//T coeff = (T) (1.0 / ( pow(2*Math::PI, n/2.0) * sqrt(_sigmaDets[k]) ) );
	Subtract(x, _mus[k], xMinusMu);
	DenseMatrix<T>::Multiply(xMinusMuOverSigma, _sigmaInvs[k], xMinusMu);
	T exponent = (T)(-0.5 * InnerProduct(xMinusMu, xMinusMuOverSigma));
	return _sigmaCoefficients[k] * exp(exponent);
}

template <class T>
T MixtureOfGaussians<T>::GaussianMultivarFast(const Vector<T>& x, UINT k) const
{
    const UINT MaxN = 5;
    const UINT n = x.Length();

	T xMinusMu[MaxN];
	T xMinusMuOverSigma[MaxN];
	
    const T *mu = _mus[k].CArray();
    const T *xArray = x.CArray();
    for (UINT i = 0; i < n; i++)
		xMinusMu[i] = xArray[i] - mu[i];

	DenseMatrix<T>::Multiply(xMinusMuOverSigma, _sigmaInvs[k], xMinusMu);

	T exponent = 0.0;
    for (UINT i = 0; i < n; i++)
		exponent += xMinusMu[i] * xMinusMuOverSigma[i];
	
	return _sigmaCoefficients[k] * exp(-0.5 * exponent);
}

template <class T>
T MixtureOfGaussians<T>::EvalDensity(const DataPoint& dataPoint, UINT gaussianI) const
{
	UINT j = gaussianI;

	T val = GaussianMultivar(dataPoint, j) * _phis[j];

	if (val != val || !_finite(val))
	{
		/*String err = "EM: NaN/infinity in gaussian density!!\nDeterminant of covariance matrix (currently coefficient): " + String(_sigmaDets[j]);
		err += "\nCovariance matrix:\n";
		for (UINT di = 0; di < _dimension; di++)
		{
			err += "\t";
			for (UINT dj = 0; dj < _dimension; dj++)
			{
				err += String(_sigmas[j](di,dj)) + " ";
			}
			err += "\n";
		}*/
		//PersistentSignalError(err);
		//throw err;
        
        //T debugVal = GaussianMultivar(dataPoint, j) * _phis[j];
        //PersistentSignalError("NaN!");

        throw String("NaN");
	}

	return val;
}

template <class T>
T MixtureOfGaussians<T>::LogLikelihood(const Vector< Vector<T> >& data)
{
	T total = 0.0;
	for (UINT i = 0; i < data.Length(); i++)
	{
		T subtotal = 0.0;
		for (UINT j = 0; j < _numGaussians; j++)
		{
			T val = EvalDensity(data[i],j);
			subtotal += val;
		}
		T logsub = log(subtotal);
		total += logsub;
	}

	// Normalize by the number of data points, so likelihood values are comparable
	// across different dataset sizes
	total /= data.Length();

	return total;
}

template <class T>
void MixtureOfGaussians<T>::UpdateSigmaQuantities()
{
	const UINT n = _sigmas.Length();
    _sigmaInvs.Allocate(n);
	//_sigmaDets.Allocate(n);
    _sigmaCoefficients.Allocate(n);

    for (UINT i = 0; i < n; i++)
	{
        const UINT dimensionCount = _sigmas[i].RowCount();
		_sigmaInvs[i] = _sigmas[i].Inverse();
		//_sigmaDets[i] = _sigmas[i].Determinant();
        T det = _sigmas[i].Determinant();
        _sigmaCoefficients[i] = (T) (1.0 / ( pow(2.0*Math::PI, dimensionCount/2.0) * sqrt(det) ) );
	}
}

template <class T>
void MixtureOfGaussians<T>::InitializeParams()
{
	// Initialize params.
	//   * means are initialized to random data points
	//   * covariance matrices are initialized based on the stddev of
	//     each dimension of the data
	//   * phis (mixing parameters) are initialized uniformly at random

	// phis
	_phis.Allocate(_numGaussians);
	T totalProbDens = 0.0;
	T one = (T)1.0;
	for (UINT j = 0; j < _numGaussians-1; j++)
	{
		T x = Math::UniformRandom<T>(0.0, one - totalProbDens);
		_phis[j] = x;
		totalProbDens += x;
	}
	_phis[_numGaussians - 1] = one - totalProbDens;

	// mus
	_mus.Allocate(_numGaussians);
	for (UINT j = 0; j < _numGaussians; j++)
		_mus[j] = _data.RandomElement();

	// sigmas
	Vector<T> means(_dimension, 0.0);
	Vector<T> sumSqs(_dimension, 0.0);
	for (UINT i = 0; i < _numDataPoints; i++)
	{
		AddInPlace(means, _data[i]);
		for (UINT j = 0; j < _dimension; j++)
			sumSqs[j] += _data[i][j]*_data[i][j];
	}
	means.Scale((T)(1.0/_numDataPoints));
	Vector<T> stddevs(_dimension);
	for (UINT j = 0; j < _dimension; j++)
		stddevs[j] = sqrt((sumSqs[j] / _numDataPoints) - means[j]*means[j]);
	_sigmas.Allocate(_numGaussians);
	for (UINT k = 0; k < _numGaussians; k++)
	{
		_sigmas[k].Allocate(_dimension,_dimension);
		_sigmas[k].Clear(0.0);
		for (UINT j = 0; j < _dimension; j++)
		{
			_sigmas[k](j,j) = stddevs[j] * Math::UniformRandom<T>(5.0, 10.0);
		}
	}
	UpdateSigmaQuantities();
}

template <class T>
void MixtureOfGaussians<T>::EStep(Vector< Vector<T> >& ws)
{
	/**
	ws[i][j] = p(x_i|z_i == j) --> Evaluation gaussian
	* p(z_i == j) --> just the phi(j) parameter
	/ sum_over_k ( p(x_i | z_i == k) * p(z_i == k) )
	**/
	Vector<T> pXandZisJ(_numGaussians);
	for (UINT i = 0; i < _numDataPoints; i++)
	{
		T pX = 0.0;
		for (UINT k = 0; k < _numGaussians; k++)
		{
			T val = EvalDensity(_data[i],k);
			pXandZisJ[k] = val;
			pX += val;
		}

		for (UINT j = 0; j < _numGaussians; j++)
		{
			// If a point has zero probability of being generated,
			// give it zero weight for all gaussians
			if (pX == 0.0)
			{
				ws[i][j] = 0.0;
			}
			else
			{
				ws[i][j] = pXandZisJ[j] / pX;
			}

			if (ws[i][j] != ws[i][j])
			{
				String err = "EM E-step: nans in EM weights!!!!\ni,j: " + String(i) + "," + String(j) + "\npXandZisJ: " + String(pXandZisJ[j]) + ", pX: " + String(pX);
				//PersistentSignalError(err);
				throw err;
			}
		}
	}
}

template <class T>
void MixtureOfGaussians<T>::MStep(const Vector< Vector<T> >& ws)
{
	Vector<T> newMu(_dimension);
	DenseMatrix<T> newSigma(_dimension, _dimension);
	Vector<T> xMinusMu;
	DenseMatrix<T> outerProd;
	for (UINT j = 0; j < _numGaussians; j++)
	{
		/** Compute new parameters **/

		T totalWeight = 0.0;
		for (UINT i = 0; i < _numDataPoints; i++)
			totalWeight += ws[i][j];
		T invTotalWeight = (T)1.0 / totalWeight;

		// New phis
		T newPhi = totalWeight / _numDataPoints;

		// New mus
		newMu.Clear(0.0);
		for (UINT i = 0; i < _numDataPoints; i++)
		{
			DataPoint dp = _data[i];
			dp.Scale(ws[i][j]);
			AddInPlace(newMu, dp);
		}
		newMu.Scale(invTotalWeight);

		// New sigmas
		newSigma.Clear(0.0);
		for (UINT i = 0; i < _numDataPoints; i++)
		{
			Subtract(_data[i], _mus[j], xMinusMu);
			OuterProduct(xMinusMu, xMinusMu, outerProd);
			newSigma += (ws[i][j] * outerProd);
		}
		DenseMatrix<T>::MultiplyInPlace(newSigma, invTotalWeight);


		/** Update parameter vectors **/

		_phis[j] = newPhi;
		_mus[j] = newMu;
		_sigmas[j] = newSigma;
	}

	UpdateSigmaQuantities();

	//// If the determinant of sigma is super small
	//// for any gaussian, randomly reinitialize the parameters
	//// This is essentially the case where a gaussian 'cluster' has no
	//// points 'assigned to it'
	//for (UINT j = 0; j < _numGaussians; j++)
	//{
	//	if (_sigmaDets[j] < 1e-6)
	//	{
	//		InitializeParams();
	//		return;
	//	}
	//}
}

template <class T>
void MixtureOfGaussians<T>::InitializeWeights(Vector< Vector<T> >& ws)
{
	ws.ReSize(_numDataPoints);
	for (UINT i = 0; i < _numDataPoints; i++)
		ws[i].ReSize(_numGaussians);
}

template <class T>
T MixtureOfGaussians<T>::LogLikelihood()
{
	return LogLikelihood(_data);
}

template <class T>
void MixtureOfGaussians<T>::Report()
{
	for (UINT j = 0; j < _numGaussians; j++)
	{
		Console::WriteLine("------------------------------");
		Console::WriteLine("GAUSSIAN " + String(j+1) +  " / " + String(_numGaussians) + ":");
		Console::WriteLine("phi: " + String(_phis[j]));
		Console::WriteString("mu: ");
		for (UINT x = 0; x < _dimension; x++)
			Console::WriteString(String(_mus[j][x]) + " ");
		Console::WriteLine("");
		Console::WriteLine("sigma:");
		for (UINT x = 0; x < _dimension; x++)
		{
			Console::WriteString("\t");
			for (UINT y = 0; y < _dimension; y++)
				Console::WriteString(String(_sigmas[j](x,y)) + " ");
			Console::WriteLine("");
		}
		Console::WriteLine("------------------------------");
	}
	if (_numDataPoints)
	{
		Console::WriteLine("Log-likelihood: " + String(LogLikelihood()));
	}
}

template <class T>
T MixtureOfGaussians<T>::EM(const Vector<DataPoint>& data, UINT numGaussians, UINT numTrials = 1, T minDelta = 0.0, bool verbose = false)
{
	if (data.Length())
		_dimension = data[0].Length();
	else
		PersistentSignalError("MixtureOfGaussians::MixtureOfGaussians - data vector is empty!");
	_data = data;
	_numDataPoints = data.Length();

	_numGaussians = numGaussians;

	Vector<T> bestPhis;
	Vector< Vector<T> > bestMus;
	Vector< DenseMatrix<T> > bestSigmas;
	T bestLikelihood = -std::numeric_limits<T>::max();
	bool madeUpdate = false;

	if (verbose) Console::WriteLine("MixtureOfGaussians: Running EM for " + String(numTrials) + " trials");
	for (UINT trial = 0; trial < numTrials; trial++)
	{
		if (verbose)
		{
			Console::WriteLine("=======================================");
			Console::WriteLine("EM TRIAL # " + String(trial));
			Console::WriteLine("=======================================");
		}

		InitializeParams();

		Vector< Vector<T> > ws;
		InitializeWeights(ws);
		UINT iterations = 0;
		T logLikelihood = LogLikelihood();
		if (logLikelihood > bestLikelihood)
		{
			// This'll only ever be called once, to set the intial
			// bestLikelihood
			bestLikelihood = logLikelihood;
			bestPhis = _phis;
			bestMus = _mus;
			bestSigmas = _sigmas;
		}
		T likelihoodDelta;
		do
		{
			if (_trainingCallback) (*_trainingCallback)(trial, iterations);

			iterations++;
			if (verbose) Console::WriteLine("EM iteration " + String(iterations));

			try
			{
				EStep(ws);
				MStep(ws);

				T newLogLikelihood = LogLikelihood();
				likelihoodDelta = newLogLikelihood - logLikelihood;
				logLikelihood = newLogLikelihood;
			}
			catch (String err)
			{
				// Immediately kill this trial
				likelihoodDelta = (T)-1.0;
				InitializeParams();
			}
			
			if (verbose) Console::WriteLine("Log-likelihood improved by: " + String(likelihoodDelta));
		}
		while (likelihoodDelta > minDelta);

		if (logLikelihood > bestLikelihood)
		{
			madeUpdate = true;
			bestLikelihood = logLikelihood;
			bestPhis = _phis;
			bestMus = _mus;
			bestSigmas = _sigmas;
		}

		if (verbose) Report();
	}

	// Keep the best params
	_phis = bestPhis;
	_mus = bestMus;
	_sigmas = bestSigmas;
	UpdateSigmaQuantities();

	if (debuggingMixtureOfGaussians && !madeUpdate)
	{
		String err = "EM: Training finished without ever improving the initial params!";
		throw err;
	}

	if (verbose)
	{
		Console::WriteLine("=======================================");
		Console::WriteLine("FINAL STATE OF MODEL AFTER ALL TRIALS:");
		Console::WriteLine("=======================================");
		Report();
	}

	// Clean stuff up
	_data.FreeMemory();
	_numDataPoints = 0;

	return bestLikelihood;
}

template <class T>
T MixtureOfGaussians<T>::EvaluateAt(const Vector<T>& point) const
{
	T totalMass = (T)0.0;
	for (UINT k = 0; k < _numGaussians; k++)
	{
        const bool checkFastVersion = false;
        if(checkFastVersion)
        {
            T a = GaussianMultivar(point, k);
            T b = GaussianMultivarFast(point, k);
            if(fabs(a - b) > 0.0) PersistentSignalError("Gaussian multivar mismatch");
        }
		totalMass += GaussianMultivarFast(point, k) * _phis[k];
	}
	return totalMass;
}

template <class T>
void MixtureOfGaussians<T>::Sample(Vector<T>& outSample) const
{
	UINT whichGaussian = Math::MultinomialRandom<T>(_phis);
	Math::GaussianRandomMultivar(_mus[whichGaussian], _sigmas[whichGaussian], outSample);
}

template<class T>
OutputDataStream& operator << (OutputDataStream& s, const MixtureOfGaussians<T>& gmm)
{
	s << gmm._numGaussians;

	for (UINT i = 0; i < gmm._numGaussians; i++)
	{
		s << gmm._phis[i];
		s << gmm._mus[i];
		s << gmm._sigmas[i];
	}

	return s;
}

template<class T>
InputDataStream& operator >> (InputDataStream& s, MixtureOfGaussians<T>& gmm)
{
	s >> gmm._numGaussians;
	gmm._phis.Allocate(gmm._numGaussians);
	gmm._mus.Allocate(gmm._numGaussians);
	gmm._sigmas.Allocate(gmm._numGaussians);

	for (UINT i = 0; i < gmm._numGaussians; i++)
	{
		s >> gmm._phis[i];
		s >> gmm._mus[i];
		s >> gmm._sigmas[i];
	}

	gmm.UpdateSigmaQuantities();

	return s;
}