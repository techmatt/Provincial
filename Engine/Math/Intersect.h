/*
Intersect.h
Written by Matthew Fisher

Code for computing geometric object intersections.
http://www.geometrictools.com/LibMathematics/Distance/Distance.html
http://www.realtimerendering.com/intersections.html
*/

namespace Math
{
    bool RayIntersectRectangle(const Ray3D &Ray, const Rectangle3f &Rect);
    bool RayIntersectTriangle(const Vec3f &RayPt, const Vec3f &RayDir, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt);
    bool LineIntersectTriangle(const Vec3f &LinePt, const Vec3f &LineDir, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt);
    bool SegmentIntersectTriangle(const Vec3f &P0, const Vec3f &P1, const Vec3f &T0, const Vec3f &T1, const Vec3f &T2, Vec3f &IntersectPt);
    void ReorientBasis(Vec3f &V1, const Vec3f &V2, const Vec3f &V3);
    bool TriangleIntersectTriangle(const Vec3f &t0v0, const Vec3f &t0v1, const Vec3f &t0v2, const Vec3f &t1v0, const Vec3f &t1v1, const Vec3f &t1v2);
}

struct TriangleRayIntersection
{
	void Init(const Vec3f &T0, const Vec3f &T1, const Vec3f &T2)
	{
		Vec3f Normal = Vec3f::Normalize(Vec3f::Cross(T1 - T0, T2 - T0));
		
		unsigned int n, u, v;

		n = 0;
		if ( fabsf(Normal[1]) > fabsf(Normal[n])) n = 1;
		if ( fabsf(Normal[2]) > fabsf(Normal[n])) n = 2;

		switch ( n )
		{
			case 0: u = 1; v = 2; break;
			case 1: u = 2; v = 0; break;
			default: u = 0; v = 1; break;
		};

		k = n;
		float inv_normalk = 1 / Normal[k];
		n_u = Normal[u] * inv_normalk;
		n_v = Normal[v] * inv_normalk;
		n_d = T0[u] * n_u + T0[v] * n_v + T0[k];

		float s;

		c_nu = + T1[v] - T0[v];
		c_nv = - T1[u] + T0[u];
		c_d  = - (T0[u] * c_nu + T0[v] * c_nv);

		s = 1.f / ( T2[u] * c_nu + T2[v] * c_nv + c_d );

		c_nu *= s;
		c_nv *= s;
		c_d  *= s;

		b_nu = + T2[v] - T0[v];
		b_nv = - T2[u] + T0[u];
		b_d  = - (T0[u] * b_nu + T0[v] * b_nv);

		s = 1.f / ( T1[u] * b_nu + T1[v] * b_nv + b_d );

		b_nu *= s;
		b_nv *= s;
		b_d  *= s;

		n_k = Normal[k];
	}

	//static const unsigned int modulo[] = {0, 1, 2, 0, 1};
	//#define ku modulo[k+1]
	//#define kv modulo[k+2]


	bool Intersect(const Vec3f &RayPt, const Vec3f &RayDir, float &HitDistance) const
	{
		//const int axis = k;
		const int ku = (k == 2) ? 0 : k + 1;
		const int kv = (k == 0) ? 2 : k - 1;

		
		const float nd = 1.0f/(RayDir[k] + n_u * RayDir[ku] + n_v * RayDir[kv]);
		const float f = (n_d - RayPt[k] - n_u * RayPt[ku] - n_v * RayPt[kv]) * nd;

		if (!(HitDistance > f && f > 1e-5f ))
		{
				return false;
		}
		
		// compute hitpoint positions on uv plane
		const float hu = (RayPt[ku] + f * RayDir[ku]);
		const float hv = (RayPt[kv] + f * RayDir[kv]);
		
		// check first barycentric coordinate
		const float lambda = (hu * b_nu + hv * b_nv + b_d);
		if (lambda < 0.0f)
		{
				return false;
		}
		
		// check second barycentric coordinate
		const float mue = (hu * c_nu + hv * c_nv + c_d);
		if (mue < 0.0f)
		{
			return false;
		}

		// check third barycentric coordinate
		if (lambda + mue > 1.0f)
		{
			return false;
		}

		// have a valid hitpoint here. store it.
		HitDistance = f;
		return true;
	}
	float n_u;
    float n_v;
    float n_d;
    unsigned int k;

    float b_nu;
    float b_nv;
    float b_d;
    float n_k;

    float c_nu;
    float c_nv;
    float c_d;
	unsigned int Pad;
};
