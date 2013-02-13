/*
UVATlas.cpp
Written by Matthew Fisher

UVAtlas is used to break a mesh up into regions topologically equivalent to a disk, and pack these regions
into a planar texture.
*/

#ifdef USE_D3D9

UVAtlas::UVAtlas()
{
    _GutterHelper = NULL;
}

UVAtlas::~UVAtlas()
{
    FreeMemory();
}

void UVAtlas::FreeMemory()
{
    _VertexRemapArray.FreeMemory();
    _Cells.FreeMemory();
    if(_GutterHelper)
    {
        _GutterHelper->Release();
        _GutterHelper = NULL;
    }
}

void UVAtlas::MakeDescriptiveBitmap(Bitmap &Bmp)
{
    Bmp.Allocate(_Cells.Cols(), _Cells.Rows());
    for(UINT y = 0; y < Bmp.Height(); y++)
    {
        for(UINT x = 0; x < Bmp.Width(); x++)
        {
            const UVAtlasCell &curCell = _Cells(y, x);
            RGBColor &C = Bmp[y][x];
            switch(curCell.TextelClass)
            {
            case 0:
                C = RGBColor::Black;
                break;
            case 1:
                C = RGBColor(Utility::Hash32(curCell.FaceIndex) % 256, 
                             Utility::Hash32(curCell.FaceIndex * 765 + 424) % 256, 
                             Utility::Hash32(curCell.FaceIndex * 431 + 861) % 256);

                break;
            case 2:
            case 4:
                C = RGBColor::Magenta;
                break;
            }
        }
    }
}

void UVAtlas::ApplyToTexture(LPDIRECT3DTEXTURE9 pTexture)
{
    Assert(_GutterHelper != NULL, "Gutter helper not allocated");
    _GutterHelper->ApplyGuttersTex(pTexture);
}

struct UVAtlasCallbackContext
{
    UINT LastValue;
};

HRESULT __stdcall UVAtlasCallback(FLOAT fPercentDone, LPVOID lpUserContext)
{
    UVAtlasCallbackContext *Context = (UVAtlasCallbackContext *)lpUserContext;
    UINT CurValue = UINT(fPercentDone * 100.0f);
    if(CurValue > Context->LastValue)
    {
        if(Context->LastValue == 0)
        {
            Console::WriteLine(String("Percent done: ") + String(fPercentDone * 100.0f) + String("%"));
        }
        else
        {
            Console::OverwriteLine(String("Percent done: ") + String(fPercentDone * 100.0f) + String("%"));
        }
        Context->LastValue = CurValue;
    }
    return S_OK;
}

void UVAtlas::Create(const BaseMesh &MeshIn, BaseMesh &MeshOut, UINT TextureWidth, UINT TextureHeight, bool UseProvidedTexCoords, const String &FastLoadDirectory)
{
    Console::WriteLine(String("Generating atlas for ") + FastLoadDirectory + String(", ") + String(MeshIn.VertexCount()) + String(" vertices"));
    Assert(MeshIn.VertexCount() > 0 && MeshIn.IndexCount() > 0, "UVAtlas called on empty mesh");
    FreeMemory();

    const UINT MaxCharts = 0;
    const float AllowedStretch = 0.25f;
    const float GutterSize = 3.0f;
    const float Tolerance = 0.0f;
    float MaxStretch;
    UINT NumCharts;

    D3D9Mesh MeshInCopy(MeshIn);
    LPD3DXMESH MeshOutCopy = NULL;
    bool FreeMeshOutCopy = false;

    D3D9Mesh PartitionedMesh(MeshIn.GetGD());

    if(UseProvidedTexCoords)
    {
        MeshOutCopy = MeshInCopy.GetMesh();
        MeshOut = MeshInCopy;
    }
    else
    {
        DWORD Hash = Utility::Hash32((BYTE *)MeshInCopy.Indices(), sizeof(DWORD) * MeshInCopy.IndexCount()) +
                     Utility::Hash32((BYTE *)MeshInCopy.Vertices(), sizeof(MeshVertex) * MeshInCopy.VertexCount()) +
                     Utility::Hash32(TextureWidth) + Utility::Hash32(TextureHeight);
        
        String DataFilename = FastLoadDirectory + String(Hash) + String(".dat");
        String MeshFilename = FastLoadDirectory + String(Hash) + String(".x");
        bool LoadFromFile = Utility::FileExists(DataFilename);
        if(LoadFromFile)
        {
            PartitionedMesh.LoadFromXFile(MeshFilename);
            MeshOutCopy = PartitionedMesh.GetMesh();
            InputDataStream Stream;
            Stream.LoadFromFile(DataFilename);
            UINT Length;
            Stream >> Length;
            _VertexRemapArray.Allocate(Length);
            Stream.ReadData((BYTE *)_VertexRemapArray.CArray(), _VertexRemapArray.Length() * sizeof(UINT));
        }
        else
        {    
            FreeMeshOutCopy = true;
            DWORD *pAdjacency = new DWORD[MeshIn.IndexCount()];
            //FLOAT *pfIMTArray;
            LPD3DXBUFFER FacePartitioningBuffer = NULL, VertexRemapArrayBuffer = NULL;
            
            D3DValidate(MeshInCopy.GetMesh()->GenerateAdjacency(Tolerance, pAdjacency), "GenerateAdjacency");
            /*pFalseEdges = new DWORD[MeshIn.IndexCount()];
            for(UINT i = 0; i < MeshIn.IndexCount(); i++)
            {
                pFalseEdges[i] = -1;
            }*/
            
            UVAtlasCallbackContext Context;
            Context.LastValue = 0;
            D3DAlwaysValidate(D3DXUVAtlasCreate(
                    MeshInCopy.GetMesh(),
                    MaxCharts, 
                    AllowedStretch,
                    TextureWidth,
                    TextureHeight,
                    GutterSize,
                    0,
                    pAdjacency,
                    NULL,
                    NULL,
                    UVAtlasCallback,
                    0.0001f,
                    &Context,
                    D3DXUVATLAS_DEFAULT,
                    &MeshOutCopy,
                    &FacePartitioningBuffer,
                    &VertexRemapArrayBuffer,
                    &MaxStretch,
                    &NumCharts
                    ), "D3DXUVAtlasCreate");

            delete[] pAdjacency;
            //delete[] pFalseEdges;
            
            _VertexRemapArray.Allocate(MeshOutCopy->GetNumVertices());
            memcpy(_VertexRemapArray.CArray(), VertexRemapArrayBuffer->GetBufferPointer(), VertexRemapArrayBuffer->GetBufferSize());

            FacePartitioningBuffer->Release();
            VertexRemapArrayBuffer->Release();

            PartitionedMesh.LoadMesh(MeshOutCopy);

			if(FastLoadDirectory.Length() > 0)
			{
				PartitionedMesh.SaveToXFile(MeshFilename);
				OutputDataStream Stream;
				Stream << _VertexRemapArray.Length();
				Stream.WriteData(_VertexRemapArray);
				Stream.SaveToFile(DataFilename);
			}
        }

        MeshOut = PartitionedMesh;
    }

    D3DAlwaysValidate(D3DXCreateTextureGutterHelper(TextureWidth, TextureHeight, MeshOutCopy, GutterSize / 2.0f, &_GutterHelper), "D3DXCreateTextureGutterHelper");
    if(FreeMeshOutCopy)
    {
        MeshOutCopy->Release();
    }

    Grid<Vec2f> BarycentricCoordGrid(TextureHeight, TextureWidth);
    Grid<BYTE> TextelClassGrid(TextureHeight, TextureWidth);
    Grid<UINT> FaceIndexGrid(TextureHeight, TextureWidth);
    D3DAlwaysValidate(_GutterHelper->GetBaryMap((D3DXVECTOR2 *)(BarycentricCoordGrid.CArray())), "GetBaryMap");
    D3DAlwaysValidate(_GutterHelper->GetGutterMap(TextelClassGrid.CArray()), "GetGutterMap");
    D3DAlwaysValidate(_GutterHelper->GetFaceMap(FaceIndexGrid.CArray()), "GetFaceMap");

    _Cells.Allocate(TextureHeight, TextureWidth);
    for(UINT y = 0; y < TextureHeight; y++)
    {
        for(UINT x = 0; x < TextureWidth; x++)
        {
            UVAtlasCell &CurCell = _Cells(y, x);
            CurCell.BarycentricCoord = BarycentricCoordGrid(y, x);
            CurCell.TextelClass = TextelClassGrid(y, x);
            CurCell.FaceIndex = FaceIndexGrid(y, x);
        }
    }
}

#endif