/*
MeshBVH.cpp
Written by Matthew Fisher

A 3D bounding volume heiarchy for representing meshes.
*/

//
// This include will be ignored by double-inclusion detection in Main.h --- it is here to appease Intellisense since this is a *.cpp file and not a *.inl file.
//
#include "Main.h"

void MeshIntersectStatistics::Describe() const
{
    Console::WriteLine("tri=" + String(triComparisons) + ", " + String(bboxComparisons));
}
    
MeshBVHNode::MeshBVHNode(MeshBVHTriangleInfo **data, UINT dataLength, MeshBVHNodeConstructorInfo &info, UINT depth)
{
    //
    // Compute bbox info
    //
    Rectangle3f bbox(data[0]->v[0], data[0]->v[0]);
    
    for(UINT dataIndex = 0; dataIndex < dataLength; dataIndex++)
    {
        const MeshBVHTriangleInfo &curData = *data[dataIndex];
        bbox.ExpandBoundingBox(curData.v[0]);
        bbox.ExpandBoundingBox(curData.v[1]);
        bbox.ExpandBoundingBox(curData.v[2]);
    }

    center = bbox.Center();
    variance = bbox.Variance();

    bool split = (dataLength > info.triangleCountCutoff);
    if(split)
    {
        //
        // Compute best axis
        //
        /*float axisScores[3];
        for(UINT axisIndex = 0; axisIndex < 3; axisIndex++)
        {
            for(UINT dataIndex = 0; dataIndex < dataCount; dataIndex++)
            {
                const MeshBVHTriangleInfo &curInfo = *data[dataIndex];
                data[dataIndex]->value = Math::Min(curInfo.v[0][axisIndex], curInfo.v[1][axisIndex], curInfo.v[2][axisIndex]);
            }
            MeshBVHTriangleInfo::Sorter sorter;
            data.Sort(sorter);

            Rectangle3f leftBBox(data.First()->v[0], data.First()->v[0]);
            Rectangle3f rightBBox(data.Last()->v[0], data.Last()->v[0]);

            for(UINT leftIndex = 0; leftIndex < leftCount; leftIndex++)
            {
                for(UINT vertexIndex = 0; vertexIndex < 3; vertexIndex++) leftBBox.ExpandBoundingBox(data[leftIndex]->v[vertexIndex]);
            }
            for(UINT rightIndex = 0; rightIndex < rightCount; rightIndex++)
            {
                for(UINT vertexIndex = 0; vertexIndex < 3; vertexIndex++) rightBBox.ExpandBoundingBox(data[leftCount + rightIndex]->v[vertexIndex]);
            }
            axisScores[axisIndex] = 2.0f - (leftBBox.Volume() + rightBBox.Volume()) / bbox.Volume();
        }
        float bestAxisScore = Math::Max(axisScores[0], axisScores[1], axisScores[2]);
        if(axisScores[0] == bestAxisScore) bestAxis = 0;
        else if(axisScores[1] == bestAxisScore) bestAxis = 1;
        else if(axisScores[2] == bestAxisScore) bestAxis = 2;*/

        const UINT bestAxis = depth % 3;

        const UINT leftCount = dataLength / 2;
        const UINT rightCount = dataLength - leftCount;
    
        for(UINT dataIndex = 0; dataIndex < dataLength; dataIndex++)
        {
            const MeshBVHTriangleInfo &curInfo = *data[dataIndex];
            curInfo.value = Math::Min(curInfo.v[0][bestAxis], curInfo.v[1][bestAxis], curInfo.v[2][bestAxis]);
        }
        MeshBVHTriangleInfo::Sorter sorter;
        std::sort(data, data + dataLength, sorter);

        if(MeshBVHDebugging) Console::WriteLine("Splitting d=" + String(depth) + ", l=" + String(leftCount) + ", r=" + String(rightCount));

        children[0] = new MeshBVHNode(data, leftCount, info, depth + 1);
        children[1] = new MeshBVHNode(data + leftCount, rightCount, info, depth + 1);
    }
    else
    {
        children[0] = NULL;
        children[1] = NULL;
        triIndex = info.storageIndex;
        triCount = dataLength;
        for(UINT dataIndex = 0; dataIndex < dataLength; dataIndex++)
        {
            info.storage[info.storageIndex++] = data[dataIndex];
        }
    }
}

MeshBVH::MeshBVH()
{
    _root = NULL;
}

MeshBVH::~MeshBVH()
{
    FreeMemory();
}

void MeshBVH::FreeMemory()
{
    if(_root) delete _root;
    _root = NULL;
    _triInfo.FreeMemory();
    _storage.FreeMemory();
}

void MeshBVH::InitSingleMesh(const BaseMesh &M, UINT triangleCountCutoff)
{
	FreeMemory();

    const MeshVertex *vertices = M.Vertices();
    const UINT vertexCount = M.VertexCount();

    const DWORD *indices = M.Indices();
    const UINT triCount = M.FaceCount();

    if(MeshBVHDebugging) Console::WriteLine("Making BVH, triCount=" + String(triCount));

    _triInfo.Allocate(triCount);
    Vector<MeshBVHTriangleInfo*> dataPointers(triCount);
    for(UINT triIndex = 0; triIndex < triCount; triIndex++)
    {
        MeshBVHTriangleInfo &curTriInfo = _triInfo[triIndex];
        dataPointers[triIndex] = &curTriInfo;

        curTriInfo.v[0] = vertices[ indices[ triIndex * 3 + 0 ] ].Pos;
        curTriInfo.v[1] = vertices[ indices[ triIndex * 3 + 1 ] ].Pos;
        curTriInfo.v[2] = vertices[ indices[ triIndex * 3 + 2 ] ].Pos;
    }

    _storage.Allocate(triCount);

    MeshBVHNodeConstructorInfo info;
    info.storage = _storage.CArray();
    info.storageIndex = 0;
    info.triangleCountCutoff = triangleCountCutoff;

    _root = new MeshBVHNode(dataPointers.CArray(), dataPointers.Length(), info, 0);
}

void MeshBVH::InitMeshList(const Vector< pair<const BaseMesh*, Matrix4> > &meshes, UINT triangleCountCutoff)
{
	FreeMemory();

    UINT triCount = 0;
    const UINT meshCount = meshes.Length();
    for(UINT meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        triCount += meshes[meshIndex].first->FaceCount();
    }

    if(MeshBVHDebugging) Console::WriteLine("Making BVH, triCount=" + String(triCount));
    
    _triInfo.Allocate(triCount);
    Vector<MeshBVHTriangleInfo*> dataPointers(triCount);

    UINT totalTriangleIndex = 0;

	for(UINT meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        const BaseMesh &curMesh = *meshes[meshIndex].first;
        const Matrix4 &curTransform = meshes[meshIndex].second;
        const UINT triangleCount = curMesh.FaceCount();
        const MeshVertex *vertices = curMesh.Vertices();
        const DWORD *indices = curMesh.Indices();
		for(UINT localTriangleIndex = 0; localTriangleIndex < triangleCount; localTriangleIndex++)
		{
            MeshBVHTriangleInfo &curTriInfo = _triInfo[totalTriangleIndex];
            dataPointers[totalTriangleIndex] = &curTriInfo;
            totalTriangleIndex++;

            curTriInfo.v[0] = curTransform.TransformPointNoProjection(vertices[ indices[ localTriangleIndex * 3 + 0 ] ].Pos);
            curTriInfo.v[1] = curTransform.TransformPointNoProjection(vertices[ indices[ localTriangleIndex * 3 + 1 ] ].Pos);
            curTriInfo.v[2] = curTransform.TransformPointNoProjection(vertices[ indices[ localTriangleIndex * 3 + 2 ] ].Pos);
		}
    }

    _storage.Allocate(triCount);
    
    MeshBVHNodeConstructorInfo info;
    info.storage = _storage.CArray();
    info.storageIndex = 0;
    info.triangleCountCutoff = triangleCountCutoff;

    _root = new MeshBVHNode(dataPointers.CArray(), dataPointers.Length(), info, 0);
}

bool MeshBVH::Intersect(const MeshBVH &left, const MeshBVH &right, MeshIntersectStatistics &stats)
{
    if(left._root == NULL || right._root == NULL) return false;
    Vector< pair<const MeshBVHNode*, const MeshBVHNode*> > conflicts(1);
    conflicts[0] = make_pair(left._root, right._root);
    while(conflicts.Length() > 0)
    {
        const auto curConflict = conflicts.Last();
        conflicts.PopEnd();
        
        stats.bboxComparisons++;

        const Vec3f centerDiff = curConflict.first->center - curConflict.second->center;
        const Vec3f varianceSum = curConflict.first->variance + curConflict.second->variance;
        if(fabsf(centerDiff.x) <= varianceSum.x &&
           fabsf(centerDiff.y) <= varianceSum.y &&
           fabsf(centerDiff.z) <= varianceSum.z)
        {
            const bool leftIsLeaf = curConflict.first->isLeafNode();
            const bool rightIsLeaf = curConflict.second->isLeafNode();
            if(leftIsLeaf && rightIsLeaf)
            {
                if(MeshBVHDebugging) Console::WriteLine("l=" + String(curConflict.first->triCount) + " r=" + String(curConflict.second->triCount));
                const UINT baseTri0Index = curConflict.first->triIndex;
                const UINT baseTri1Index = curConflict.second->triIndex;
                auto triStorage0 = left._storage.CArray();
                auto triStorage1 = right._storage.CArray();
                for(UINT tri0Index = 0; tri0Index < curConflict.first->triCount; tri0Index++)
                {
                    const MeshBVHTriangleInfo &tri0 = *triStorage0[baseTri0Index + tri0Index];
                    for(UINT tri1Index = 0; tri1Index < curConflict.second->triCount; tri1Index++)
                    {
                        stats.triComparisons++;
                        const MeshBVHTriangleInfo &tri1 = *triStorage1[baseTri1Index + tri1Index];
                        //if(Math::DistanceTriangleTriangleSq(tri0.v[0], tri0.v[1], tri0.v[2], tri1.v[0], tri1.v[1], tri1.v[2]) == 0.0f)
                        if(Math::TriangleIntersectTriangle(tri0.v[0], tri0.v[1], tri0.v[2], tri1.v[0], tri1.v[1], tri1.v[2]))
                        {
                            return true;
                        }
                    }
                }
            }
            else
            {
                if(leftIsLeaf)
                {
                    conflicts.PushEnd(make_pair(curConflict.first, curConflict.second->children[0]));
                    conflicts.PushEnd(make_pair(curConflict.first, curConflict.second->children[1]));
                }
                else if(rightIsLeaf)
                {
                    conflicts.PushEnd(make_pair(curConflict.first->children[0], curConflict.second));
                    conflicts.PushEnd(make_pair(curConflict.first->children[1], curConflict.second));
                }
                else
                {
                    conflicts.PushEnd(make_pair(curConflict.first->children[0], curConflict.second->children[0]));
                    conflicts.PushEnd(make_pair(curConflict.first->children[0], curConflict.second->children[1]));
                    conflicts.PushEnd(make_pair(curConflict.first->children[1], curConflict.second->children[0]));
                    conflicts.PushEnd(make_pair(curConflict.first->children[1], curConflict.second->children[1]));
                }

                /*bool splitLeft = rightIsLeaf;
                if(!leftIsLeaf && !rightIsLeaf)
                {
                    splitLeft = (curConflict.first->size() > curConflict.second->size());
                }
                if(splitLeft)
                {
                    conflicts.PushEnd(make_pair(curConflict.first->children[0], curConflict.second));
                    conflicts.PushEnd(make_pair(curConflict.first->children[1], curConflict.second));
                }
                else
                {
                    conflicts.PushEnd(make_pair(curConflict.first, curConflict.second->children[0]));
                    conflicts.PushEnd(make_pair(curConflict.first, curConflict.second->children[1]));
                }*/
            }
        }
    }
    return false;
}
