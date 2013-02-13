/*
Compression.cpp
Written by Matthew Fisher
*/

#ifdef USE_ZLIB

void Compression::CompressStreamToFile(const Vector<BYTE> &stream, const String &filename)
{
    CompressStreamToFile(stream.CArray(), stream.Length(), filename);
}

void Compression::CompressStreamToFile(const BYTE *stream, UINT byteCount, const String &filename)
{
    BYTE *compressedStream = new BYTE[byteCount + 64];

    z_stream zstream;

    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
	
	zstream.avail_in = byteCount;
	zstream.next_in = const_cast<BYTE*>(stream);

	zstream.data_type = Z_BINARY;

	zstream.avail_out = byteCount + 64;
    zstream.next_out = compressedStream;

	const int Level = 6;
    //Result = deflateInit(&Stream, Level);
	int result = deflateInit2(&zstream, Level, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
	PersistentAssert(result == Z_OK, "deflateInit failed");

    result = deflate(&zstream, Z_FINISH);
	PersistentAssert(result == Z_STREAM_END, "deflate failed");

    deflateEnd(&zstream);

    FILE *file = Utility::CheckedFOpen(filename.CString(), "wb");
    Utility::CheckedFWrite(&byteCount, sizeof(UINT32), 1, file);
    Utility::CheckedFWrite(compressedStream, sizeof(BYTE), zstream.total_out, file);
    fclose(file);

    delete[] compressedStream;
}

void Compression::DecompressStreamFromFile(const String &filename, Vector<BYTE> &stream)
{
    Vector<BYTE> input;
    Utility::GetFileData(filename, input);
    UINT32 decompressedByteCount = ((UINT32*)input.CArray())[0];
    stream.Allocate(decompressedByteCount);

    uLongf finalByteCount = decompressedByteCount;
    int result = uncompress(stream.CArray(), &finalByteCount, input.CArray() + sizeof(UINT32), input.Length() - sizeof(UINT32));
    PersistentAssert(result == Z_OK, "Decompression failed");
    PersistentAssert(finalByteCount == decompressedByteCount, "Decompression returned invalid length");
}

void Compression::DecompressStreamFromMemory(const Vector<BYTE> &compressedStream, Vector<BYTE> &decompressedStream)
{
    DecompressStreamFromMemory(compressedStream.CArray(), compressedStream.Length(), decompressedStream.CArray(), decompressedStream.Length());
}

void Compression::DecompressStreamFromMemory(const BYTE *compressedStream, UINT compressedStreamLength, BYTE *decompressedStream, UINT decompressedStreamLength)
{
    PersistentAssert(decompressedStreamLength > 0, "Caller must provide the length of the decompressed stream");

    uLongf finalByteCount = decompressedStreamLength;
    int result = uncompress(decompressedStream, &finalByteCount, compressedStream, compressedStreamLength);
    PersistentAssert(result == Z_OK, "Decompression failed");
    PersistentAssert(finalByteCount == decompressedStreamLength, "Decompression returned invalid length");
}

#else

void Compression::CompressStreamToFile(const Vector<BYTE> &stream, const String &filename)
{
    SignalError("Must define USE_ZLIB to use compression functions");
}

void Compression::CompressStreamToFile(const BYTE *stream, UINT byteCount, const String &filename)
{
    SignalError("Must define USE_ZLIB to use compression functions");
}

void Compression::DecompressStreamFromFile(const String &filename, Vector<BYTE> &stream)
{
    SignalError("Must define USE_ZLIB to use compression functions");
}

#endif