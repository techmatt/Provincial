/*
Distance.cpp
Written by Matthew Fisher

Code for computing geometric object distances.
http://www.geometrictools.com/LibMathematics/Distance/Distance.html
*/

float Math::DistanceLineSegmentSq(const Vec3f &l0, const Vec3f &l1, const Vec3f &seg0, const Vec3f &seg1, DistanceLineSegmentResult *result)
{
    Vec3f segmentCenter = (seg0 + seg1) * 0.5f;
    Vec3f diff = l0 - segmentCenter;
    Vec3f lineDirection = Vec3f::Normalize(l1 - l0);
    Vec3f segmentDirection = Vec3f::Normalize(seg1 - seg0);
    float segmentExtent = (seg1 - seg0).Length() * 0.5f;
    float a01 = -Vec3f::Dot(lineDirection, segmentDirection);
    float b0 = Vec3f::Dot(diff, lineDirection);
    float c = diff.LengthSq();
    float det = fabsf(1.0f - a01*a01);
    float b1, s0, s1, sqrDist, extDet;

    if (det >= 1e-7f)
    {
        // The line and segment are not parallel.
        b1 = -Vec3f::Dot(diff, segmentDirection);
        s1 = a01*b0 - b1;
        extDet = segmentExtent*det;

        if (s1 >= -extDet)
        {
            if (s1 <= extDet)
            {
                // Two interior points are closest, one on the line and one
                // on the segment.
                float invDet = ((float)1)/det;
                s0 = (a01*b1 - b0)*invDet;
                s1 *= invDet;
                sqrDist = s0*(s0 + a01*s1 + ((float)2)*b0) +
                    s1*(a01*s0 + s1 + ((float)2)*b1) + c;
            }
            else
            {
                // The endpoint e1 of the segment and an interior point of
                // the line are closest.
                s1 = segmentExtent;
                s0 = -(a01*s1 + b0);
                sqrDist = -s0*s0 + s1*(s1 + ((float)2)*b1) + c;
            }
        }
        else
        {
            // The end point e0 of the segment and an interior point of the
            // line are closest.
            s1 = -segmentExtent;
            s0 = -(a01*s1 + b0);
            sqrDist = -s0*s0 + s1*(s1 + ((float)2)*b1) + c;
        }
    }
    else
    {
        // The line and segment are parallel.  Choose the closest pair so that
        // one point is at segment center.
        s1 = (float)0;
        s0 = -b0;
        sqrDist = b0*s0 + c;
    }

    if(result != NULL)
    {
        result->lineParameter = s0;
        result->segmentParameter = s1;
    }
    //mClosestPoint0 = l0 + s0*lineDirection;
    //mClosestPoint1 = segmentCenter + s1*segmentDirection;

    // Account for numerical round-off errors.
    if (sqrDist < (float)0)
    {
        sqrDist = (float)0;
    }
    return sqrDist;

}

float Math::DistancePointTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &p)
{
    Vec3f diff = t0 - p;
    Vec3f edge0 = t1 - t0;
    Vec3f edge1 = t2 - t0;
    float a00 = edge0.LengthSq();
    float a01 = Vec3f::Dot(edge0, edge1);
    float a11 = edge1.LengthSq();
    float b0 = Vec3f::Dot(diff, edge0);
    float b1 = Vec3f::Dot(diff, edge1);
    float c = diff.LengthSq();
    float det = fabsf(a00*a11 - a01*a01);
    float s = a01*b1 - a11*b0;
    float t = a01*b0 - a00*b1;
    float sqrDistance;

    if (s + t <= det)
    {
        if (s < (float)0)
        {
            if (t < (float)0)  // region 4
            {
                if (b0 < (float)0)
                {
                    t = (float)0;
                    if (-b0 >= a00)
                    {
                        s = (float)1;
                        sqrDistance = a00 + ((float)2)*b0 + c;
                    }
                    else
                    {
                        s = -b0/a00;
                        sqrDistance = b0*s + c;
                    }
                }
                else
                {
                    s = (float)0;
                    if (b1 >= (float)0)
                    {
                        t = (float)0;
                        sqrDistance = c;
                    }
                    else if (-b1 >= a11)
                    {
                        t = (float)1;
                        sqrDistance = a11 + ((float)2)*b1 + c;
                    }
                    else
                    {
                        t = -b1/a11;
                        sqrDistance = b1*t + c;
                    }
                }
            }
            else  // region 3
            {
                s = (float)0;
                if (b1 >= (float)0)
                {
                    t = (float)0;
                    sqrDistance = c;
                }
                else if (-b1 >= a11)
                {
                    t = (float)1;
                    sqrDistance = a11 + ((float)2)*b1 + c;
                }
                else
                {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        }
        else if (t < (float)0)  // region 5
        {
            t = (float)0;
            if (b0 >= (float)0)
            {
                s = (float)0;
                sqrDistance = c;
            }
            else if (-b0 >= a00)
            {
                s = (float)1;
                sqrDistance = a00 + ((float)2)*b0 + c;
            }
            else
            {
                s = -b0/a00;
                sqrDistance = b0*s + c;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            float invDet = ((float)1)/det;
            s *= invDet;
            t *= invDet;
            sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                t*(a01*s + a11*t + ((float)2)*b1) + c;
        }
    }
    else
    {
        float tmp0, tmp1, numer, denom;

        if (s < (float)0)  // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((float)2)*a01 + a11;
                if (numer >= denom)
                {
                    s = (float)1;
                    t = (float)0;
                    sqrDistance = a00 + ((float)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (float)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                        t*(a01*s + a11*t + ((float)2)*b1) + c;
                }
            }
            else
            {
                s = (float)0;
                if (tmp1 <= (float)0)
                {
                    t = (float)1;
                    sqrDistance = a11 + ((float)2)*b1 + c;
                }
                else if (b1 >= (float)0)
                {
                    t = (float)0;
                    sqrDistance = c;
                }
                else
                {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        }
        else if (t < (float)0)  // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((float)2)*a01 + a11;
                if (numer >= denom)
                {
                    t = (float)1;
                    s = (float)0;
                    sqrDistance = a11 + ((float)2)*b1 + c;
                }
                else
                {
                    t = numer/denom;
                    s = (float)1 - t;
                    sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                        t*(a01*s + a11*t + ((float)2)*b1) + c;
                }
            }
            else
            {
                t = (float)0;
                if (tmp1 <= (float)0)
                {
                    s = (float)1;
                    sqrDistance = a00 + ((float)2)*b0 + c;
                }
                else if (b0 >= (float)0)
                {
                    s = (float)0;
                    sqrDistance = c;
                }
                else
                {
                    s = -b0/a00;
                    sqrDistance = b0*s + c;
                }
            }
        }
        else  // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= (float)0)
            {
                s = (float)0;
                t = (float)1;
                sqrDistance = a11 + ((float)2)*b1 + c;
            }
            else
            {
                denom = a00 - ((float)2)*a01 + a11;
                if (numer >= denom)
                {
                    s = (float)1;
                    t = (float)0;
                    sqrDistance = a00 + ((float)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (float)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                        t*(a01*s + a11*t + ((float)2)*b1) + c;
                }
            }
        }
    }

    // Account for numerical round-off error.
    if (sqrDistance < 0.0f)
    {
        sqrDistance = 0.0f;
    }

    //mClosestPoint0 = *mPoint;
    //mClosestPoint1 = mTriangle->V[0] + s*edge0 + t*edge1;
    //mTriangleBary[1] = s;
    //mTriangleBary[2] = t;
    //mTriangleBary[0] = (float)1 - s - t;
    return sqrDistance;
}

float Math::DistanceLineTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &l0, const Vec3f &l1, DistanceLineTriangleResult *result)
{
    // Test if line intersects triangle.  If so, the squared distance is zero.
    Vec3f edge0 = t1 - t0;
    Vec3f edge1 = t2 - t0;
    Vec3f normal = Vec3f::Cross(edge0, edge1);
    Vec3f lineDirection = Vec3f::Normalize(l1 - l0);
    float NdD = Vec3f::Dot(normal, lineDirection);
    if (fabs(NdD) > 1e-7f)
    {
        // The line and triangle are not parallel, so the line intersects
        // the plane of the triangle.
        Vec3f diff = l0 - t0;
        Vec3f U, V;
        Vec3f::CompleteOrthonormalBasis(lineDirection, U, V);
        float UdE0 = Vec3f::Dot(U, edge0);
        float UdE1 = Vec3f::Dot(U, edge1);
        float UdDiff = Vec3f::Dot(U, diff);
        float VdE0 = Vec3f::Dot(V, edge0);
        float VdE1 = Vec3f::Dot(V, edge1);
        float VdDiff = Vec3f::Dot(V, diff);
        float invDet = ((float)1)/(UdE0*VdE1 - UdE1*VdE0);

        // Barycentric coordinates for the point of intersection.
        float b1 = (VdE1*UdDiff - UdE1*VdDiff)*invDet;
        float b2 = (UdE0*VdDiff - VdE0*UdDiff)*invDet;
        float b0 = (float)1 - b1 - b2;

        if (b0 >= (float)0 && b1 >= (float)0 && b2 >= (float)0)
        {
            // Line parameter for the point of intersection.
            float DdE0 = Vec3f::Dot(lineDirection, edge0);
            float DdE1 = Vec3f::Dot(lineDirection, edge1);
            float DdDiff = Vec3f::Dot(lineDirection, diff);
            
            if(result != NULL)
            {
                result->lineParameter = b1*DdE0 + b2*DdE1 - DdDiff;
            }

            // Barycentric coordinates for the point of intersection.
            //mTriangleBary[0] = b0;
            //mTriangleBary[1] = b1;
            //mTriangleBary[2] = b2;

            // The intersection point is inside or on the triangle.
            //mClosestPoint0 = mLine->Origin + mLineParameter*mLine->Direction;

            //mClosestPoint1 = t0 + b1*edge0 + b2*edge1;

            return (float)0;
        }
    }

    const Vec3f trianglePoints[3] = {t0, t1, t2};
    // Either (1) the line is not parallel to the triangle and the point of
    // intersection of the line and the plane of the triangle is outside the
    // triangle or (2) the line and triangle are parallel.  Regardless, the
    // closest point on the triangle is on an edge of the triangle.  Compare
    // the line to all three edges of the triangle.
    float sqrDist = numeric_limits<float>::max();
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        DistanceLineSegmentResult lineSegmentResult;
        float sqrDistTmp = DistanceLineSegmentSq(l0, l1, trianglePoints[i0], trianglePoints[i1], &lineSegmentResult);
        if (sqrDistTmp < sqrDist)
        {
            //mClosestPoint0 = queryLS.GetClosestPoint0();
            //mClosestPoint1 = queryLS.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            if(result != NULL)
            {
                result->lineParameter = lineSegmentResult.lineParameter;
            }
            //mLineParameter = queryLS.GetLineParameter();
            //float ratio = queryLS.GetSegmentParameter()/segment.Extent;
            //mTriangleBary[i0] = ((float)0.5)*((float)1 - ratio);
            //mTriangleBary[i1] = (float)1 - mTriangleBary[i0];
            //mTriangleBary[3-i0-i1] = (float)0;
        }
    }

    return sqrDist;
}

float Math::DistanceSegmentTriangleSq(const Vec3f &t0, const Vec3f &t1, const Vec3f &t2, const Vec3f &seg0, const Vec3f &seg1)
{
    float segmentExtent = (seg1 - seg0).Length() * 0.5f;
    Vec3f segmentCenter = (seg0 + seg1) * 0.5f;

    DistanceLineTriangleResult lineTriangleResult;
    float sqrDist = DistanceLineTriangleSq(t0, t1, t2, segmentCenter, seg1, &lineTriangleResult);
    float segmentParameter = lineTriangleResult.lineParameter;

    if (segmentParameter >= -segmentExtent)
    {
        if (segmentParameter <= segmentExtent)
        {
            //mClosestPoint0 = queryLT.GetClosestPoint0();
            //mClosestPoint1 = queryLT.GetClosestPoint1();
            //mTriangleBary[0] = queryLT.GetTriangleBary(0);
            //mTriangleBary[1] = queryLT.GetTriangleBary(1);
            //mTriangleBary[2] = queryLT.GetTriangleBary(2);
        }
        else
        {
            //mClosestPoint0 = mSegment->P1;
            sqrDist = DistancePointTriangleSq(t0, t1, t2, seg1);
            //mClosestPoint1 = queryPT.GetClosestPoint1();
            //mSegmentParameter = mSegment->Extent;
            //mTriangleBary[0] = queryPT.GetTriangleBary(0);
            //mTriangleBary[1] = queryPT.GetTriangleBary(1);
            //mTriangleBary[2] = queryPT.GetTriangleBary(2);
        }
    }
    else
    {
        //mClosestPoint0 = mSegment->P0;
        sqrDist = DistancePointTriangleSq(t0, t1, t2, seg0);
        //mClosestPoint1 = queryPT.GetClosestPoint1();
        //mSegmentParameter = -mSegment->Extent;
        //mTriangleBary[0] = queryPT.GetTriangleBary(0);
        //mTriangleBary[1] = queryPT.GetTriangleBary(1);
        //mTriangleBary[2] = queryPT.GetTriangleBary(2);
    }

    return sqrDist;

}

float Math::DistanceTriangleTriangleSq(const Vec3f &t0v0, const Vec3f &t0v1, const Vec3f &t0v2, const Vec3f &t1v0, const Vec3f &t1v1, const Vec3f &t1v2)
{
    // Compare edges of triangle0 to the interior of triangle1.
    float sqrDist = numeric_limits<float>::max(), sqrDistTmp;

    const Vec3f triangle0Points[3] = {t0v0, t0v1, t0v2};
    const Vec3f triangle1Points[3] = {t1v0, t1v1, t1v2};

    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        sqrDistTmp = DistanceSegmentTriangleSq(triangle1Points[0], triangle1Points[1], triangle1Points[2], triangle0Points[i0], triangle0Points[i1]);
        if (sqrDistTmp < sqrDist)
        {
            //mClosestPoint0 = queryST.GetClosestPoint0();
            //mClosestPoint1 = queryST.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            //ratio = queryST.GetSegmentParameter()/edge.Extent;
            //mTriangleBary0[i0] = ((float)0.5)*((float)1 - ratio);
            //mTriangleBary0[i1] = (float)1 - mTriangleBary0[i0];
            //mTriangleBary0[3-i0-i1] = (float)0;
            //mTriangleBary1[0] = queryST.GetTriangleBary(0);
            //mTriangleBary1[1] = queryST.GetTriangleBary(1);
            //mTriangleBary1[2] = queryST.GetTriangleBary(2);

            if (sqrDist <= 1e-7f)
            {
                return (float)0;
            }
        }
    }

    // Compare edges of triangle1 to the interior of triangle0.
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        sqrDistTmp = DistanceSegmentTriangleSq(triangle0Points[0], triangle0Points[1], triangle0Points[2], triangle1Points[i0], triangle1Points[i1]);
        if (sqrDistTmp < sqrDist)
        {
            //mClosestPoint0 = queryST.GetClosestPoint0();
            //mClosestPoint1 = queryST.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            //ratio = queryST.GetSegmentParameter()/edge.Extent;
            //mTriangleBary1[i0] = ((float)0.5)*((float)1 - ratio);
            //mTriangleBary1[i1] = (float)1 - mTriangleBary1[i0];
            //mTriangleBary1[3-i0-i1] = (float)0;
            //mTriangleBary0[0] = queryST.GetTriangleBary(0);
            //mTriangleBary0[1] = queryST.GetTriangleBary(1);
            //mTriangleBary0[2] = queryST.GetTriangleBary(2);

            if (sqrDist <= 1e-7f)
            {
                return (float)0;
            }
        }
    }

    return sqrDist;

}
