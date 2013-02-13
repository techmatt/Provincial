/*
Plane.cpp
Written by Matthew Fisher

A standard 3D plane (space plane.)  Essentially just the surface defined by a*x + b*y + c*z + d = 0
See Plane.h for a description of these functions.
*/

Plane::Plane()
{

}

Plane::Plane(const Plane &P)
{
    a = P.a;
    b = P.b;
    c = P.c;
    d = P.d;
}

Plane::Plane(float _a, float _b, float _c, float _d)
{
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

Plane::Plane(const Vec3f &NormalizedNormal, float _d)
{
    a = NormalizedNormal.x;
    b = NormalizedNormal.y;
    c = NormalizedNormal.z;
    d = _d;
}

Plane Plane::ConstructFromPointNormal(const Vec3f &Pt, const Vec3f &Normal)
{
    Plane Result;
    Vec3f NormalizedNormal = Vec3f::Normalize(Normal);
    Result.a = NormalizedNormal.x;
    Result.b = NormalizedNormal.y;
    Result.c = NormalizedNormal.z;
    Result.d = -Vec3f::Dot(Pt, NormalizedNormal);
    return Result;
}

Plane Plane::ConstructFromPointVectors(const Vec3f &Pt, const Vec3f &V1, const Vec3f &V2)
{
    Vec3f Normal = Vec3f::Cross(V1, V2);
    return ConstructFromPointNormal(Pt, Normal);
}

Plane Plane::Normalize()
{
    Plane Result;
    float Distance = sqrtf(a * a + b * b + c * c);
    Result.a = a / Distance;
    Result.b = b / Distance;
    Result.c = c / Distance;
    Result.d = d / Distance;
    return Result;
}

Plane Plane::ConstructFromPoints(const Vec3f &V0, const Vec3f &V1, const Vec3f &V2)
{
    Vec3f Normal = Vec3f::Normalize(Vec3f::Cross(V1 - V0, V2 - V0));
    return ConstructFromPointNormal(V0, Normal);
}

Vec3f Plane::IntersectLine(const Line3D &Line) const
{
    return IntersectLine(Line.P0, Line.P0 + Line.D);
}

Vec3f Plane::IntersectLine(const Vec3f &V1, const Vec3f &V2) const
{
    Vec3f Diff = V1 - V2;
    float Denominator = a * Diff.x + b * Diff.y + c * Diff.z;
    if(Denominator == 0.0f)
    {
        return (V1 + V2) * 0.5f;
    }
    float u = (a * V1.x + b * V1.y + c * V1.z + d) / Denominator;

    return (V1 + u * (V2 - V1));
}

Vec3f Plane::IntersectLine(const Vec3f &V1, const Vec3f &V2, bool &Hit) const
{
    Hit = true;
    Vec3f Diff = V2 - V1;
    float denominator = a * Diff.x + b * Diff.y + c * Diff.z;
    if(denominator == 0) {Hit = false; return V1;}
    float u = (a * V1.x + b * V1.y + c * V1.z + d) / denominator;

    return (V1 + u * (V2 - V1));
}

float Plane::IntersectLineRatio(const Vec3f &V1, const Vec3f &V2)
{
    Vec3f Diff = V2 - V1;
    float Denominator = a * Diff.x + b * Diff.y + c * Diff.z;
    if(Denominator == 0.0f)
    {
        return 0.0f;
    }
    return (a * V1.x + b * V1.y + c * V1.z + d) / -Denominator;
}

float Plane::SignedDistance(const Vec3f &Pt) const
{
    return (a * Pt.x + b * Pt.y + c * Pt.z + d);
}

float Plane::UnsignedDistance(const Vec3f &Pt) const
{
    return fabsf(a * Pt.x + b * Pt.y + c * Pt.z + d);
}

#ifdef USE_D3D
Plane::operator D3DXPLANE()
{
    D3DXPLANE P(a, b, c, d);
    return P;
}
#endif

Vec3f Plane::ClosestPoint(const Vec3f &Point)
{
    return (Point - Normal() * SignedDistance(Point));
}

bool Plane::PlanePlaneIntersection(const Plane &P1, const Plane &P2, Line3D &L)
{
    float Denominator = P1.a * P2.b - P1.b * P2.a;
    if(Denominator == 0.0f)
    {
        // this case should be handled by switching axes...
        return false;
    }
    L.P0 = Vec3f((P2.d * P1.b - P1.d * P2.b) / Denominator, (P1.d * P2.a - P2.d * P1.a) / Denominator, 0.0f);

    L.D = Vec3f::Cross(P1.Normal(), P2.Normal());
    if(L.D.Length() == 0.0f)
    {
        return false;
    }
    L.D = Vec3f::Normalize(L.D);

    return true;
}


float Plane::Dot(const Plane &P, const Vec4f &V)
{
    return P.a * V.x + P.b * V.y + P.c * V.z + P.d * V.w;
}

float Plane::DotCoord(const Plane &P, const Vec3f &V)
{
    return P.a * V.x + P.b * V.y + P.c * V.z + P.d;
}

float Plane::DotNormal(const Plane &P, const Vec3f &V)
{
    return P.a * V.x + P.b * V.y + P.c * V.z;
}

/*
The remainder of the code in this file implements the function Plane::FitToPoints.
Part of this is taken from:
http://www.google.com/codesearch?hl=en&q=+best+fit+plane+show:HZpynAfUBdA:AwtEUprnjf8:-KEKP5FuK5I&sa=N&cd=1&ct=rc&cs_p=ftp://figment.csee.usf.edu/pub/segmentation-comparison/USF-segger-code.tar.Z&cs_f=calcplane.c#first
*/

/*    This routine finds the scatter matrix of a number of points equal
**    to "Points_Total".  The x,y and z coordinates of the points are
**    stored in the "X_Coord", "Y_Coord" and "Z_Coord" arrays.  "Centroid"
**    is a 3-element array containing the center of gravity of the set
**    of points.  The scatter matrix will be returned in the 3x3 array
**    called "ScatterMatrix".  The xyz placement in the Scatter Matrix
**    will be returned by the 3 element array "Order", where the index
**    of Order indicates the column (and row) in "ScatterMatrix", and
**    a value of 0 means x, 1 means y, and 2 means z.
*/

void Find_ScatterMatrix(
    const Vector<Vec4f> &Points,
    const Vec3f &Centroid,
    float ScatterMatrix[3][3],
    int Order[3]
){
    int    i,TempI;
    float    TempD;

    /*    To compute the correct scatter matrix, the centroid must be
    **    subtracted from all points.  If the plane is to be forced to pass
    **    through the origin (0,0,0), then the Centroid was earlier set
    **    equal to (0,0,0).  This, of course, is NOT the true Centroid of
    **    the set of points!  Since the matrix is symmetrical about its
    **    diagonal, one-third of it is redundant and is simply found at
    **    the end.
    */
    for (i=0; i<3; i++){
        ScatterMatrix[i][0]=ScatterMatrix[i][1]=ScatterMatrix[i][2]=0;
    }

    for(UINT i = 0; i < Points.Length(); i++)
    {
        const Vec4f &P = Points[i];
        Vec3f d = Vec3f(P.x, P.y, P.z) - Centroid;
        float Weight = P.w;
        ScatterMatrix[0][0] += d.x*d.x*Weight;
        ScatterMatrix[0][1] += d.x*d.y*Weight;
        ScatterMatrix[0][2] += d.x*d.z*Weight;
        ScatterMatrix[1][1] += d.y*d.y*Weight;
        ScatterMatrix[1][2] += d.y*d.z*Weight;
        ScatterMatrix[2][2] += d.z*d.z*Weight;
    }
    ScatterMatrix[1][0]=ScatterMatrix[0][1];
    ScatterMatrix[2][0]=ScatterMatrix[0][2];
    ScatterMatrix[2][1]=ScatterMatrix[1][2];

    /*    Now, perform a sort of "Matrix-sort", whereby all the larger elements
    **    in the matrix are relocated towards the lower-right portion of the
    **    matrix.  This is done as a requisite of the tred2 and tqli algorithms,
    **    for which the scatter matrix is being computed as an input.
    **    "Order" is a 3 element array that will keep track of the xyz order
    **    in the ScatterMatrix.
    */
    Order[0]=0;        /* Beginning order is x-y-z, as found above */
    Order[1]=1;
    Order[2]=2;
    if (ScatterMatrix[0][0] > ScatterMatrix[1][1]){
        TempD=ScatterMatrix[0][0];
        ScatterMatrix[0][0]=ScatterMatrix[1][1];
        ScatterMatrix[1][1]=TempD;
        TempD=ScatterMatrix[0][2];
        ScatterMatrix[0][2]=ScatterMatrix[2][0]=ScatterMatrix[1][2];
        ScatterMatrix[1][2]=ScatterMatrix[2][1]=TempD;
        TempI=Order[0];
        Order[0]=Order[1];
        Order[1]=TempI;
    }
    if (ScatterMatrix[1][1] > ScatterMatrix[2][2]){
        TempD=ScatterMatrix[1][1];
        ScatterMatrix[1][1]=ScatterMatrix[2][2];
        ScatterMatrix[2][2]=TempD;
        TempD=ScatterMatrix[0][1];
        ScatterMatrix[0][1]=ScatterMatrix[1][0]=ScatterMatrix[0][2];
        ScatterMatrix[0][2]=ScatterMatrix[2][0]=TempD;
        TempI=Order[1];
        Order[1]=Order[2];
        Order[2]=TempI;
    }
    if (ScatterMatrix[0][0] > ScatterMatrix[1][1]){
        TempD=ScatterMatrix[0][0];
        ScatterMatrix[0][0]=ScatterMatrix[1][1];
        ScatterMatrix[1][1]=TempD;
        TempD=ScatterMatrix[0][2];
        ScatterMatrix[0][2]=ScatterMatrix[2][0]=ScatterMatrix[1][2];
        ScatterMatrix[1][2]=ScatterMatrix[2][1]=TempD;
        TempI=Order[0];
        Order[0]=Order[1];
        Order[1]=TempI;
    }
}
     


/*
**    This code is taken from ``Numerical Recipes in C'', 2nd
**    and 3rd editions, by Press, Teukolsky, Vetterling and
**    Flannery, Cambridge University Press, 1992, 1994.
**
*/

/*
**    tred2 Householder reduction of a float, symmetric matrix a[1..n][1..n].
**    On output, a is replaced by the orthogonal matrix q effecting the
**    transformation. d[1..n] returns the diagonal elements of the
**    tridiagonal matrix, and e[1..n] the off-diagonal elements, with
**    e[1]=0.
**
**    For my problem, I only need to handle a 3x3 symmetric matrix,
**    so it can be simplified.
**    Therefore n=3.
**
**    Attention: in the book, the index for array starts from 1,
**    but in C, index should start from zero. so I need to modify it.
**    I think it is very simple to modify, just substract 1 from all the
**    index.
*/

#define    SIGN(a,b)    ((b)<0? -fabs(a):fabs(a))

void tred2(float a[3][3], float d[3], float e[3]){
  int        l,k,i,j;
  float    scale,hh,h,g,f;

    for(i=3;i>=2;i--)
    {
    l=i-1;
    h=scale=0.0;
    if(l>1)
        {
        for(k=1;k<=l;k++)
            scale+=fabs(a[i-1][k-1]);
        if(scale==0.0)        /* skip transformation */
            e[i-1]=a[i-1][l-1];
        else
            {
            for(k=1;k<=l;k++)
                {
                a[i-1][k-1]/=scale;    /* use scaled a's for transformation. */
                h+=a[i-1][k-1]*a[i-1][k-1];    /* form sigma in h. */
                }
            f=a[i-1][l-1];
            g=f>0? -sqrt(h):sqrt(h);
            e[i-1]=scale*g;
            h-=f*g;    /* now h is equation (11.2.4) */
            a[i-1][l-1]=f-g;    /* store u in the ith row of a. */
            f=0.0;
            for(j=1;j<=l;j++)
                {
                a[j-1][i-1]=a[i-1][j-1]/h; /* store u/H in ith column of a. */
                g=0.0;    /* form an element of A.u in g */
                for(k=1;k<=j;k++)
                    g+=a[j-1][k-1]*a[i-1][k-1];
                for(k=j+1;k<=l;k++)
                    g+=a[k-1][j-1]*a[i-1][k-1];
                e[j-1]=g/h; /* form element of p in temorarliy unused element of e. */
                f+=e[j-1]*a[i-1][j-1];
                }
            hh=f/(h+h);    /* form K, equation (11.2.11) */
            for(j=1;j<=l;j++) /* form q and store in e overwriting p. */
                {
                f=a[i-1][j-1]; /* Note that e[l]=e[i-1] survives */
                e[j-1]=g=e[j-1]-hh*f;
                for(k=1;k<=j;k++) /* reduce a, equation (11.2.13) */
                    a[j-1][k-1]-=(f*e[k-1]+g*a[i-1][k-1]);
                }
            }
        }
    else
        e[i-1]=a[i-1][l-1];
    d[i-1]=h;
    }


  /*
  **    For computing eigenvector.
  */
  d[0]=0.0;
  e[0]=0.0;

  for(i=1;i<=3;i++)/* begin accumualting of transfomation matrices */
    {
    l=i-1;
    if(d[i-1]) /* this block skipped when i=1 */
        {
        for(j=1;j<=l;j++)
            {
            g=0.0;
            for(k=1;k<=l;k++) /* use u and u/H stored in a to form P.Q */
                g+=a[i-1][k-1]*a[k-1][j-1];
            for(k=1;k<=l;k++)
                a[k-1][j-1]-=g*a[k-1][i-1];
            }
        }
    d[i-1]=a[i-1][i-1];
    a[i-1][i-1]=1.0; /* reset row and column of a to identity matrix for next iteration */
    for(j=1;j<=l;j++)
        a[j-1][i-1]=a[i-1][j-1]=0.0;
    }
}



/*
**    QL algo with implicit shift, to determine the eigenvalues and
**    eigenvectors of a float,symmetric  tridiagonal matrix, or of a float,
**    symmetric matrix previously reduced by algo tred2.
**    On input , d[1..n] contains the diagonal elements of the tridiagonal
**    matrix. On output, it returns the eigenvalues. The vector e[1..n]
**    inputs the subdiagonal elements of the tridiagonal matrix, with e[1]
**    arbitrary. On output e is destroyed. If the eigenvectors of a
**    tridiagonal matrix are desired, the matrix z[1..n][1..n] is input
**    as the identity matrix. If the eigenvectors of a matrix that has
**    been reduced by tred2 are required, then z is input as the matrix
**    output by tred2. In either case, the kth column of z returns the
**    normalized eigenvector corresponding to d[k].
**
*/
void tqli(float d[3], float e[3], float z[3][3]){
  int        m,l,iter,i,k;
  float    s,r,p,g,f,dd,c,b;

  for(i=2;i<=3;i++)
    e[i-2]=e[i-1];    /* convenient to renumber the elements of e */
  e[2]=0.0;
  for(l=1;l<=3;l++)
    {
    iter=0;
    do
        {
        for(m=l;m<=2;m++)
            {
            /*
            **    Look for a single small subdiagonal element
            **    to split the matrix.
            */
            dd=fabs(d[m-1])+fabs(d[m]);
            if(fabs(e[m-1])+dd == dd)
                break;
            }
        if(m!=l)
            {
            if(iter++ == 30)
                {
                printf("\nToo many iterations in TQLI");
                }
            g=(d[l]-d[l-1])/(2.0f*e[l-1]); /* form shift */
            r=sqrt((g*g)+1.0f);
            g=d[m-1]-d[l-1]+e[l-1]/(g+SIGN(r,g)); /* this is dm-ks */
            s=c=1.0;
            p=0.0;
            for(i=m-1;i>=l;i--)
                {
                /*
                **    A plane rotation as in the original
                **    QL, followed by Givens rotations to
                **    restore tridiagonal form.
                */
                f=s*e[i-1];
                b=c*e[i-1];
                if(fabs(f) >= fabs(g))
                    {
                    c=g/f;
                    r=sqrt((c*c)+1.0f);
                    e[i]=f*r;
                    c*=(s=1.0f/r);
                    }
                else
                    {
                    s=f/g;
                    r=sqrt((s*s)+1.0f);
                    e[i]=g*r;
                    s*=(c=1.0f/r);
                    }
                g=d[i]-p;
                r=(d[i-1]-g)*s+2.0f*c*b;
                p=s*r;
                d[i]=g+p;
                g=c*r-b;
                for(k=1;k<=3;k++)
                    {
                    /*
                    **    Form eigenvectors
                    */
                    f=z[k-1][i];
                    z[k-1][i]=s*z[k-1][i-1]+c*f;
                    z[k-1][i-1]=c*z[k-1][i-1]-s*f;
                    }
                }
            d[l-1]=d[l-1]-p;
            e[l-1]=g;
            e[m-1]=0.0f;
            }
        }while(m != l);
    }
}

bool Plane::FitToPoints(const Vector<Vec3f> &Points, float &ResidualError)
{
    Vec3f Basis1, Basis2;
    Vector<Vec4f> WeightedPoints(Points.Length());
    for(UINT i = 0; i < Points.Length(); i++)
    {
        WeightedPoints[i] = Vec4f(Points[i], 1.0f);
    }
    float NormalEigenvalue;
    return FitToPoints(WeightedPoints, Basis1, Basis2, NormalEigenvalue, ResidualError);
}

bool Plane::FitToPoints(const Vector<Vec4f> &Points, Vec3f &Basis1, Vec3f &Basis2, float &NormalEigenvalue, float &ResidualError)
{
    Vec3f Centroid, Normal;

    float ScatterMatrix[3][3];
    int  Order[3];
    float DiagonalMatrix[3];
    float OffDiagonalMatrix[3];

    // Find centroid
    Centroid = Vec3f::Origin;
    float TotalWeight = 0.0f;
    for(UINT i = 0; i < Points.Length(); i++)
    {
        TotalWeight += Points[i].w;
        Centroid += Vec3f(Points[i].x, Points[i].y, Points[i].z) * Points[i].w;
    }
    Centroid /= TotalWeight;

    // Compute scatter matrix
    Find_ScatterMatrix(Points, Centroid, ScatterMatrix, Order);

    tred2(ScatterMatrix,DiagonalMatrix,OffDiagonalMatrix);
    tqli(DiagonalMatrix,OffDiagonalMatrix,ScatterMatrix);

    /*
    **    Find the smallest eigenvalue first.
    */
    float Min = DiagonalMatrix[0];
    float Max = DiagonalMatrix[0];
    UINT MinIndex = 0;
    UINT MiddleIndex = 0;
    UINT MaxIndex = 0;
    for(UINT i = 1; i < 3; i++)
    {
        if(DiagonalMatrix[i] < Min)
        {
            Min = DiagonalMatrix[i];
            MinIndex = i;
        }
        if(DiagonalMatrix[i] > Max)
        {
            Max = DiagonalMatrix[i];
            MaxIndex = i;
        }
    }
    for(UINT i = 0; i < 3; i++)
    {
        if(MinIndex != i && MaxIndex != i)
        {
            MiddleIndex = i;
        }
    }
    /*
    **    The normal of the plane is the smallest eigenvector.
    */
    for(UINT i = 0; i < 3; i++)
    {
        Normal[Order[i]] = ScatterMatrix[i][MinIndex];
        Basis1[Order[i]] = ScatterMatrix[i][MiddleIndex];
        Basis2[Order[i]] = ScatterMatrix[i][MaxIndex];
    }
    NormalEigenvalue = fabsf(DiagonalMatrix[MinIndex]);
    Basis1.SetLength(DiagonalMatrix[MiddleIndex]);
    Basis2.SetLength(DiagonalMatrix[MaxIndex]);

    if(!Basis1.Valid() || !Basis2.Valid() || !Normal.Valid())
    {
        *this = ConstructFromPointNormal(Centroid, Vec3f::eX);
        Basis1 = Vec3f::eY;
        Basis2 = Vec3f::eZ;
    }
    else
    {
        *this = ConstructFromPointNormal(Centroid, Normal);
    }

    ResidualError = 0.0f;
    for(UINT i = 0; i < Points.Length(); i++)
    {
        ResidualError += UnsignedDistance(Vec3f(Points[i].x, Points[i].y, Points[i].z));
    }
    ResidualError /= Points.Length();

    return true;
}