/*
MeshDistance.cpp
Written by Matthew Fisher

Mesh-mesh distance implementation.
*/

void MeshDistanceBruteForce::InitMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes)
{
    UINT triangleSum = 0;
    const UINT meshCount = meshes.Length();
    for(UINT meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        triangleSum += meshes[meshIndex].first->FaceCount();
    }

    _trianglesA.Allocate(triangleSum);
    _trianglesB.Allocate(triangleSum);

	UINT triangleListIndex = 0;

	for(UINT meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        const BaseMesh &curMesh = *meshes[meshIndex].first;
        const Matrix4 &curTransform = meshes[meshIndex].second;
        const UINT triangleCount = curMesh.FaceCount();
        const MeshVertex *vertices = curMesh.Vertices();
        const DWORD *indices = curMesh.Indices();
		for(UINT triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
		{
            TriangleEntryA &curEntryA = _trianglesA[triangleListIndex];
            TriangleEntryB &curEntryB = _trianglesB[triangleListIndex];
            curEntryB.V[0] = curTransform.TransformPointNoProjection(vertices[ indices[ triangleIndex * 3 + 0 ] ].Pos);
            curEntryB.V[1] = curTransform.TransformPointNoProjection(vertices[ indices[ triangleIndex * 3 + 1 ] ].Pos);
            curEntryB.V[2] = curTransform.TransformPointNoProjection(vertices[ indices[ triangleIndex * 3 + 2 ] ].Pos);
            Rectangle3f curBBox = Rectangle3f::ConstructFromTwoPoints(curEntryB.V[0], curEntryB.V[1]);
            curBBox.ExpandBoundingBox(curEntryB.V[2]);
            curEntryA.center = curBBox.Center();
            curEntryA.variance = curBBox.Variance();
			triangleListIndex++;
		}
    }
}

bool MeshDistanceBruteForce::IntersectMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes) const
{
    const UINT meshCount = meshes.Length();
    const UINT mTriangleCount = _trianglesA.Length();

    const TriangleEntryA *triangleAList = _trianglesA.CArray();
    const TriangleEntryB *triangleBList = _trianglesB.CArray();
    
    for(UINT meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        const BaseMesh &curMesh = *meshes[meshIndex].first;
        const Matrix4 &curTransform = meshes[meshIndex].second;
        const UINT triangleCount = curMesh.FaceCount();
        const MeshVertex *vertices = curMesh.Vertices();
        const DWORD *indices = curMesh.Indices();
		for(UINT outerTriangleIndex = 0; outerTriangleIndex < triangleCount; outerTriangleIndex++)
		{
			const Vec3f t0 = curTransform.TransformPointNoProjection(vertices[ indices[ outerTriangleIndex * 3 + 0 ] ].Pos);
			const Vec3f t1 = curTransform.TransformPointNoProjection(vertices[ indices[ outerTriangleIndex * 3 + 1 ] ].Pos);
			const Vec3f t2 = curTransform.TransformPointNoProjection(vertices[ indices[ outerTriangleIndex * 3 + 2 ] ].Pos);

            Rectangle3f curBBox = Rectangle3f::ConstructFromTwoPoints(t0, t1);
            curBBox.ExpandBoundingBox(t2);
            const Vec3f center = curBBox.Center();
            const Vec3f variance = curBBox.Variance();
			
            for(UINT innerTriangleIndex = 0; innerTriangleIndex < mTriangleCount; innerTriangleIndex++)
            {
                const TriangleEntryA &curEntryA = triangleAList[innerTriangleIndex];
                if(fabsf(center.x - curEntryA.center.x) <= variance.x + curEntryA.variance.x &&
                   fabsf(center.y - curEntryA.center.y) <= variance.y + curEntryA.variance.y &&
                   fabsf(center.z - curEntryA.center.z) <= variance.z + curEntryA.variance.z)
                {
                    const TriangleEntryB &curEntryB = triangleBList[innerTriangleIndex];
                    //if(Math::DistanceTriangleTriangleSq(t0, t1, t2, curEntryB.V[0], curEntryB.V[1], curEntryB.V[2]) == 0.0f)
                    if(Math::TriangleIntersectTriangle(t0, t1, t2, curEntryB.V[0], curEntryB.V[1], curEntryB.V[2]))
                    {
                        return true;
                    }
                }
            }
		}
    }
    return false;
}
