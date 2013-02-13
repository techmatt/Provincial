/*
Compression.h
Written by Matthew Fisher
*/

class Compression
{
public:
    static void CompressStreamToFile(const Vector<BYTE> &stream, const String &filename);
    static void CompressStreamToFile(const BYTE *stream, UINT byteCount, const String &filename);
    static void DecompressStreamFromFile(const String &filename, Vector<BYTE> &stream);
    static void DecompressStreamFromMemory(const Vector<BYTE> &compressedStream, Vector<BYTE> &decompressedStream);
    static void DecompressStreamFromMemory(const BYTE *compressedStream, UINT compressedStreamLength, BYTE *decompressedStream, UINT decompressedStreamLength);
};
