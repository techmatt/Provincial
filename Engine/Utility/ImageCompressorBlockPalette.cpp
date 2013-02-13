/*
ImageCompressor.cpp
Written by Matthew Fisher
*/

#include "Main.h"

#ifdef USE_FOURIER_TRANSFORM
#include "Engine/Math/FourierTransform.cpp"
#endif

UINT ImageCompressorBlockPalette::RoundDown(UINT dimension)
{
    while(dimension % blockDimension != 0) dimension--;
    return dimension;
}

void ImageCompressorBlockPalette::Compress(const Bitmap &bmp, OutputDataStream &stream)
{
    TrainPalette(bmp);
    SavePalette(stream);
    QuantizeToPalette(bmp, stream);
}

void ImageCompressorBlockPalette::Decompress(InputDataStream &stream, Bitmap &bmp)
{
    LoadPalette(stream);
    LoadFromPalette(stream, bmp);
}

void ImageCompressorBlockPalette::SavePalette(OutputDataStream &stream)
{
    Palette16 p16;
    for(UINT paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        Block16 &curBlock16 = p16.colors[paletteIndex];
        const Block32 &curBlock32 = _palette.colors[paletteIndex];
        for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
        {
            for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
            {
                curBlock16.c[yOffset][xOffset] = RGBColor16FromRGBColor32(curBlock32.c[yOffset][xOffset]);
            }
        }
    }
    stream.WriteData(p16);
}

void ImageCompressorBlockPalette::LoadPalette(InputDataStream &stream)
{
    Palette16 p16;
    stream.ReadData(p16);
    for(UINT paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        const Block16 &curBlock16 = p16.colors[paletteIndex];
        Block32 &curBlock32 = _palette.colors[paletteIndex];
        for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
        {
            for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
            {
                curBlock32.c[yOffset][xOffset] = RGBColor32FromRGBColor16(curBlock16.c[yOffset][xOffset]);
            }
        }
    }
}

void ImageCompressorBlockPalette::QuantizeToPalette(const Bitmap &bmp, OutputDataStream &stream)
{
    const UINT width = RoundDown(bmp.Width());
    const UINT height = RoundDown(bmp.Height());

    stream << width << height;
    
    for(UINT x = 0; x < width; x += blockDimension)
    {
        for(UINT y = 0; y < height; y += blockDimension)
        {
            Block32 curBlock;
            for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
            {
                for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
                {
                    curBlock.c[yOffset][xOffset] = bmp[y + yOffset][x + xOffset];
                }
            }
            UINT bestPaletteError = 0xFFFFFFFF;
            UINT bestPaletteIndex = 0;
            for(UINT paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
            {
                UINT curPaletteError = CompareBlocks(curBlock, _palette.colors[paletteIndex]);
                if(curPaletteError < bestPaletteError)
                {
                    bestPaletteIndex = paletteIndex;
                    bestPaletteError = curPaletteError;
                }
            }
            stream << BYTE(bestPaletteIndex);
            //stream << UINT16(bestPaletteIndex);
        }
    }
}

void ImageCompressorBlockPalette::LoadFromPalette(InputDataStream &stream, Bitmap &bmp)
{
    UINT width, height;
    stream >> width >> height;
    bmp.Allocate(width, height);
    for(UINT x = 0; x < width; x += blockDimension)
    {
        for(UINT y = 0; y < height; y += blockDimension)
        {
            BYTE paletteIndex;
            //UINT16 paletteIndex;
            stream >> paletteIndex;
            const Block32 &curBlock = _palette.colors[paletteIndex];
            for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
            {
                for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
                {
                    bmp[y + yOffset][x + xOffset] = curBlock.c[yOffset][xOffset];
                }
            }
        }
    }
}

void ImageCompressorBlockPalette::TrainPalette(const Bitmap &bmp)
{
    Vector<const float*> allBlocks;
    
    const UINT width = bmp.Width();
    const UINT height = bmp.Height();
    for(UINT x = 0; x < width; x += blockDimension)
    {
        for(UINT y = 0; y < height; y += blockDimension)
        {
            float *curBlock = new float[blockSize * 3];
            UINT curDimensionIndex = 0;
            for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
            {
                for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
                {
                    RGBColor curColor = bmp[y + yOffset][x + xOffset];
                    curBlock[curDimensionIndex++] = curColor.r;
                    curBlock[curDimensionIndex++] = curColor.g;
                    curBlock[curDimensionIndex++] = curColor.b;
                }
            }
            allBlocks.PushEnd(curBlock);
        }
    }

    KMeansClusteringFloat<blockSize * 3> clustering;
    
    Vector<KMeansClusteringTierEntry> tierInfo(4);
    tierInfo[0] = KMeansClusteringTierEntry(4, 10);
    tierInfo[1] = KMeansClusteringTierEntry(4, 10);
    tierInfo[2] = KMeansClusteringTierEntry(4, 10);
    tierInfo[3] = KMeansClusteringTierEntry(4, 10);

    clustering.Cluster(allBlocks, tierInfo, true);
    //clustering.Cluster(allBlocks, paletteSize);
    //clustering.Cluster(allBlocks, paletteSize, 100);

    for(UINT paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
    {
        Block32 &curEntry = _palette.colors[paletteIndex];
        const float *curCluster = clustering.ClusterCentroid(paletteIndex);
        UINT curDimensionIndex = 0;
        for(UINT yOffset = 0; yOffset < blockDimension; yOffset++)
        {
            for(UINT xOffset = 0; xOffset < blockDimension; xOffset++)
            {
                RGBColor &curColor = curEntry.c[yOffset][xOffset];
                curColor.r = BYTE(curCluster[curDimensionIndex++]);
                curColor.g = BYTE(curCluster[curDimensionIndex++]);
                curColor.b = BYTE(curCluster[curDimensionIndex++]);
            }
        }
    }
}

