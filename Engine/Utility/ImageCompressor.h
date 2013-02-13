/*
ImageCompressor.h
Written by Matthew Fisher

An ImageCompressor takes an image and compresses it to a byte stream.
*/

#include "Main.h"

class ImageCompressorJPEG
{
public:
    static void Compress(const Bitmap &bmp, OutputDataStream &stream);
    static void Decompress(InputDataStream &stream, Bitmap &bmp);

private:
    struct Block
    {
        RGBColor d[8][8];
    };

    static void CompressBlock(Block &b, OutputDataStream &stream);
    static void DecompressBlock(InputDataStream &stream, Block &b);
};

class ImageCompressorJPEG2000
{
public:
    void Compress(const Bitmap &bmp, OutputDataStream &stream);
    void Decompress(InputDataStream &stream, Bitmap &bmp);

private:
    static const UINT blockDimension = 64;
    static const UINT codingBlockDimension = 8;
    struct Block
    {
        Vec3f c[blockDimension][blockDimension];
    };

    UINT RoundDown(UINT dimension);

    void CompressBlock(Block &b, OutputDataStream &stream);
    void DecompressBlock(InputDataStream &stream, Block &b);
};

class ImageCompressorBlockPalette
{
public:
    void Compress(const Bitmap &bmp, OutputDataStream &stream);
    void Decompress(InputDataStream &stream, Bitmap &bmp);

private:
    static const UINT blockDimension = 2;
    static const UINT blockSize = blockDimension * blockDimension;
    static const UINT paletteSize = 256;

    struct BlockFloat
    {
        Vec3f d[blockSize];
    };
    struct Block32
    {
        RGBColor c[blockDimension][blockDimension];
    };
    struct Block16
    {
        RGBColor16 c[blockDimension][blockDimension];
    };
    struct Palette32
    {
        Block32 colors[paletteSize];
    };
    struct Palette16
    {
        Block16 colors[paletteSize];
    };

    __forceinline UINT CompareBlocks(const Block32 &a, const Block32 &b)
    {
        UINT sum = 0;
        for(UINT x = 0; x < blockDimension; x++)
        {
            for(UINT y = 0; y < blockDimension; y++)
            {
                sum += RGBColor::DistL2(a.c[x][y], b.c[x][y]);
            }
        }
        return sum;
    }

    UINT RoundDown(UINT dimension);
    
    void SavePalette(OutputDataStream &stream);
    void LoadPalette(InputDataStream &stream);
    
    void TrainPalette(const Bitmap &bmp);
    void QuantizeToPalette(const Bitmap &bmp, OutputDataStream &stream);
    void LoadFromPalette(InputDataStream &stream, Bitmap &bmp);

    Palette32 _palette;
};