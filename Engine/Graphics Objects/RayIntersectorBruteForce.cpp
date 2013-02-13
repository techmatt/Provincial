/*
RayIntersectorBruteForce.cpp
Written by Matthew Fisher

Brute force implementation of RayIntersector.
*/

void RayIntersectorBruteForce::InitMeshList(const Vector<const BaseMesh*> &Meshes)
{
    UINT FaceCountSum = 0;
    for(UINT MeshIndex = 0; MeshIndex < Meshes.Length(); MeshIndex++)
    {
        FaceCountSum += Meshes[MeshIndex]->FaceCount();
    }

    /*_Vertices.Allocate(VertexCountSum);
    _Indices.Allocate(FaceCountSum * 3);
    UINT CurVertexIndex = 0, CurIndexIndex = 0;

    for(UINT MeshIndex = 0; MeshIndex < Meshes.Length(); MeshIndex++)
    {
        const BaseMesh &CurMesh = *Meshes[MeshIndex];
        UINT BaseVertexIndex = CurVertexIndex;
        for(UINT VertexIndex = 0; VertexIndex < CurMesh.VertexCount(); VertexIndex++)
        {
            _Vertices[CurVertexIndex++] = CurMesh.Vertices()[VertexIndex].Pos;
        }
        for(UINT IndexIndex = 0; IndexIndex < CurMesh.IndexCount(); IndexIndex++)
        {
            _Indices[CurIndexIndex++] = CurMesh.Indices()[IndexIndex] + BaseVertexIndex;
        }
    }*/

	_Triangles.Allocate(FaceCountSum);
	UINT TriangleListIndex = 0;

	for(UINT MeshIndex = 0; MeshIndex < Meshes.Length(); MeshIndex++)
    {
        const BaseMesh &CurMesh = *Meshes[MeshIndex];
		for(UINT FaceIndex = 0; FaceIndex < CurMesh.FaceCount(); FaceIndex++)
		{
			const Vec3f &T0 = CurMesh.Vertices()[ CurMesh.Indices()[ FaceIndex * 3 + 0] ].Pos;
			const Vec3f &T1 = CurMesh.Vertices()[ CurMesh.Indices()[ FaceIndex * 3 + 1] ].Pos;
			const Vec3f &T2 = CurMesh.Vertices()[ CurMesh.Indices()[ FaceIndex * 3 + 2] ].Pos;
			_Triangles[TriangleListIndex].Init(T0, T1, T2);
			TriangleListIndex++;
		}
    }
}

void RayIntersectorBruteForce::FindIntersections(const Ray3D &R, Vector<Intersection> &IntersectPts) const
{
    /*const UINT TriangleCount = _Indices.Length() / 3;
    const UINT *Indices = _Indices.CArray();
    const Vec3f *Vertices = _Vertices.CArray();
    
    for(UINT TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        const Vec3f &V0 = Vertices[Indices[TriangleIndex * 3 + 0]];
        const Vec3f &V1 = Vertices[Indices[TriangleIndex * 3 + 1]];
        const Vec3f &V2 = Vertices[Indices[TriangleIndex * 3 + 2]];

        Intersection CurIntersection;
        if(Math::RayIntersectTriangle(R.P0, R.D, V0, V1, V2, CurIntersection.Pt))
        {
            CurIntersection.FaceIndex = TriangleIndex;
            IntersectPts.PushEnd(CurIntersection);
        }
    }

    RayIntersectionSorter Sorter(R.P0);
    IntersectPts.Sort(Sorter);*/
}

bool RayIntersectorBruteForce::FindFirstIntersection(const Ray3D &R, Intersection &Result, float &DistSq) const
{
	/*const UINT TriangleCount = _Indices.Length() / 3;
    const UINT *Indices = _Indices.CArray();
    const Vec3f *Vertices = _Vertices.CArray();
    
	bool IntersectionFound = false;
    for(UINT TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        const Vec3f &V0 = Vertices[Indices[TriangleIndex * 3 + 0]];
        const Vec3f &V1 = Vertices[Indices[TriangleIndex * 3 + 1]];
        const Vec3f &V2 = Vertices[Indices[TriangleIndex * 3 + 2]];

        Vec3f IntersectionPt;
        if(Math::RayIntersectTriangle(R.P0, R.D, V0, V1, V2, IntersectionPt))
        {
			float CurDistSq = Vec3f::DistSq(IntersectionPt, R.P0);
			if(CurDistSq < DistSq)
			{
				IntersectionFound = true;
				Result.Pt = IntersectionPt;
				Result.FaceIndex = TriangleIndex;
				DistSq = CurDistSq;
			}
        }
    }
	return IntersectionFound;*/

	const UINT TriangleCount = _Triangles.Length();
    bool IntersectionFound = false;
    
	for(UINT TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
    {
        float HitDistance = 1000000.0f;
        if(_Triangles[TriangleIndex].Intersect(R.P0, R.D, HitDistance))
        {
			Vec3f IntersectionPt = R.P0 + R.D * HitDistance;
			float CurDistSq = Vec3f::DistSq(IntersectionPt, R.P0);
			if(CurDistSq < DistSq)
			{
				IntersectionFound = true;
				Result.Pt = IntersectionPt;
				Result.FaceIndex = TriangleIndex;
				DistSq = CurDistSq;
			}
        }
    }
	return IntersectionFound;
}
