struct Gaussian1D
{
    template<class T>
    void FitToPoints(const Vector<T> &points)
    {
        const UINT pointCount = points.Length();
        if(pointCount == 1)
        {
            mu = points[0];
            sigma = 0.0;
        }
        else
        {
            mu = points.Sum() / double(points.Length());
            double sum = 0.0;
            for(UINT pointIndex = 0; pointIndex < pointCount; pointIndex++)
            {
                sum += Math::Square(points[pointIndex] - mu);
            }
            sigma = sqrt(sum / (pointCount - 1));
        }
    }

    double mu, sigma;
};
