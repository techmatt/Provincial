/*
ImageCompressor.cpp
Written by Matthew Fisher
*/

#include "Main.h"

#ifdef USE_FOURIER_TRANSFORM
#include "Engine/Math/FourierTransform.cpp"

//ddct(n, cos(M_PI/n/2), -sin(M_PI/n/2), a);

void iddct8(float c, float s, float *a)
{
    a[0] *= 0.5;
    ddct(8, c, s, a);
    for (int j = 0; j < 8; j++)
    {
        a[j] *= 0.25f;
    }
}

void ImageCompressorJPEG2000::Compress(const Bitmap &bmp, OutputDataStream &stream)
{
    Block b;
    const UINT width = bmp.Width();
    const UINT height = bmp.Height();
    for(UINT x = 0; x < width; x += 8)
    {
        for(UINT y = 0; y < height; y += 8)
        {
            for(UINT xOffset = 0; xOffset < 8; xOffset++)
            {
                for(UINT yOffset = 0; yOffset < 8; yOffset++)
                {
                    b.d[xOffset][yOffset] = bmp[y + yOffset][x + xOffset];
                }
            }
            CompressBlock(b, stream);
        }
    }
}

void ImageCompressorJPEG2000::Decompress(InputDataStream &stream, Bitmap &bmp)
{

}

void DCTBlock(float w[8][8])
{
    static const float c = cosf(Math::PIf/ 8.0f / 2.0f);
    static const float s = -sinf(Math::PIf/ 8.0f / 2.0f);
    for(UINT x = 0; x < 8; x++)
    {
        ddct(8, c, s, w[x]);
    }

    float storage[8];
    for(UINT y = 0; y < 8; y++)
    {
        for(UINT x = 0; x < 8; x++)
        {
            storage[x] = w[x][y];
        }
        ddct(8, c, s, storage);
        for(UINT x = 0; x < 8; x++)
        {
            w[x][y] = storage[x];
        }
    }

    w[0][0] -= 1024.0f;
}

void IDCTBlock(float w[8][8])
{
    static const float c = cosf(Math::PIf/ 8.0f / 2.0f);
    static const float s = sinf(Math::PIf/ 8.0f / 2.0f);

    w[0][0] += 1024.0f;
    
    for(UINT x = 0; x < 8; x++)
    {
        iddct8(c, s, w[x]);
    }

    float storage[8];
    for(UINT y = 0; y < 8; y++)
    {
        for(UINT x = 0; x < 8; x++)
        {
            storage[x] = w[x][y];
        }
        iddct8(c, s, storage);
        for(UINT x = 0; x < 8; x++)
        {
            w[x][y] = storage[x];
        }
    }
}

// use same quantization matrix, just scale it by a constant

void ImageCompressorJPEG2000::CompressBlock(Block &b, OutputDataStream &stream)
{
    float w[8][8];
    for(UINT channel = 0; channel < 3; channel++)
    {
        if(channel == 0)      for(UINT xOffset = 0; xOffset < 8; xOffset++) for(UINT yOffset = 0; yOffset < 8; yOffset++) w[xOffset][yOffset] = b.d[xOffset][yOffset].r;
        else if(channel == 1) for(UINT xOffset = 0; xOffset < 8; xOffset++) for(UINT yOffset = 0; yOffset < 8; yOffset++) w[xOffset][yOffset] = b.d[xOffset][yOffset].g;
        else if(channel == 2) for(UINT xOffset = 0; xOffset < 8; xOffset++) for(UINT yOffset = 0; yOffset < 8; yOffset++) w[xOffset][yOffset] = b.d[xOffset][yOffset].b;
        
        DCTBlock(w);
        IDCTBlock(w);
    }
}

void ImageCompressorJPEG2000::DecompressBlock(InputDataStream &stream, Block &b)
{

}
#endif