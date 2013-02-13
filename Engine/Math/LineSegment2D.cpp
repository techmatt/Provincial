/*
LineSegment2D.cpp
Written by Matthew Fisher

a 2D line segment represented by an origin point and a direction vector.
*/

void LineSegment2D::Expand(float Dist)
{
    Vec3f Extension = Vec3f::Normalize(D) * Dist;
    P0 -= Extension;
    D += Extension * 2.0f;
}

float LineSegment2D::FindLineT(const Vec3f &P) const
{
    float r_numerator = (P.x-P0.x)*(D.x) + (P.y-P0.y)*(D.y);
    float r_denomenator = (D.x)*(D.x) + (D.y)*(D.y);
    return r_numerator / r_denomenator;
}

float LineSegment2D::DistToPoint(const Vec3f &P) const
{
    float r_numerator = (P.x-P0.x)*(D.x) + (P.y-P0.y)*(D.y);
    float r_denomenator = (D.x)*(D.x) + (D.y)*(D.y);
    float r = r_numerator / r_denomenator;

    float px = P0.x + r*(D.x);
    float py = P0.y + r*(D.y);

    float s =  ((P0.y-P.y)*(D.x)-(P0.x-P.x)*(D.y) ) / r_denomenator;

    float distanceLine = fabs(s)*sqrt(r_denomenator);

    if ( (r >= 0) && (r <= 1) )
    {
        return distanceLine;
    }
    else
    {
        float dist1 = Vec3f::Dist(P, P0);
        float dist2 = Vec3f::Dist(P, P0 + D);
        return Math::Min(dist1, dist2);
    }
}

bool LineSegment2D::Intersect(const LineSegment2D &A, const LineSegment2D &B, Vec3f &Intersection)
{
    float denominator = B.D.y * A.D.x - B.D.x * A.D.y;
    if(denominator == 0.0f)
    {
        return false;
    }
    float numeratorA = (B.D.x * (A.P0.y - B.P0.y) - B.D.y * (A.P0.x - B.P0.x)) / denominator;
    float numeratorB = (A.D.x * (A.P0.y - B.P0.y) - A.D.y * (A.P0.x - B.P0.x)) / denominator;

    if(numeratorA >= 0.0f && numeratorA <= 1.0f &&
       numeratorB >= 0.0f && numeratorB <= 1.0f)
    {
        Intersection = A.Evaluate(numeratorA);
        return true;
    }
    else
    {
        return false;
    }
}
