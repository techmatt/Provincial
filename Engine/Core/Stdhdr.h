/*
Stdhdr.h
Written by Matthew Fisher

Collection of useful functions/macros/constants.
*/

#pragma once

//
// Math namespace contains useful math-related constants and functions
//
namespace Math
{
    const double PI = 3.1415926535897932384626433832795028842;
    const float PIf = 3.14159265358979323846f;

    //
    // rnd() returns a number between 0.0 and 1.0
    //
    #define rnd()            (((FLOAT)rand() ) / RAND_MAX)

    //
    // pmrnd() returns a number between -1.0 and 1.0
    //
    #define pmrnd()            ((rnd() - 0.5f) * 2.0f)

    __forceinline float DegreesToRadians(float x)
    {
        return x * PIf / 180.0f;
    }

    __forceinline float RadiansToDegrees(float x)
    {
        return x * 180.0f / PIf;
    }

	template <class T>
	__forceinline T StandardizeAngle(T rads)
	{
		T TWOPI = (T)(2.0*Math::PI);
		while (rads < 0.0f) rads += TWOPI;
		while (rads > TWOPI) rads -= TWOPI;
		return rads;
	}

    __forceinline float Sign(float x)
    {
        if(x < 0.0f)
        {
            return -1.0f;
        }
        else
        {
            return 1.0f;
        }
    }

    __forceinline double Sigmoid(double X)
    {
        return 1.0 / (1.0 + exp(-X));
    }
    
    //Math::LinearMap is a very useful function.  Given a source interval (s1, e1),
    //a target interval (s2, e2), and a source value start, returns the mapping
    //of the source value's position on the source interval to the same relative
    //position in the target interval.  For example, Math::LinearMap(-1, 1, 0, ScreenWidth, X)
    //would map from perspective space to screen space; in other words
    //LinearMap(-1, 1, 0, ScreenWidth, -1)        = 0
    //LinearMap(-1, 1, 0, ScreenWidth, -0.5)    = ScreenWidth*0.25
    //LinearMap(-1, 1, 0, ScreenWidth, 0)        = ScreenWidth*0.5
    //LinearMap(-1, 1, 0, ScreenWidth, 0.5)        = ScreenWidth*0.75
    //LinearMap(-1, 1, 0, ScreenWidth, 1)        = ScreenWidth
    __forceinline float LinearMap(float s1, float e1, float s2, float e2, float start)
    {
        return ((start-s1)*(e2-s2)/(e1-s1)+s2);
    }

    __forceinline double LinearMap(double s1, double e1, double s2, double e2, double start)
    {
        return ((start-s1)*(e2-s2)/(e1-s1)+s2);
    }

    __forceinline long double LinearMap(long double s1, long double e1, long double s2, long double e2, long double start)
    {
        return ((start-s1)*(e2-s2)/(e1-s1)+s2);
    }

    __forceinline float Lerp(float Left, float Right, float s)
    {
        return (Left + s * (Right - Left));
    }

    __forceinline double Lerp(double Left, double Right, double s)
    {
        return (Left + s * (Right - Left));
    }

    __forceinline long double Lerp(long double Left, long double Right, long double s)
    {
        return (Left + s * (Right - Left));
    }

    //
    // This is approximately 2x slower than fabsf
    //
    //template <class type> __forceinline type Abs(type x)
    //{
    //    if(x < 0)
    //    {
    //        return -x;
    //    }
    //    else
    //    {
    //        return x;
    //    }
    //}
    __forceinline int AbsInt(int x)
    {
        if(x < 0)
        {
            return -x;
        }
        else
        {
            return x;
        }
    }

    __forceinline int Mod(int x, UINT M)
    {
        if(x >= 0)
        {
            return (x % M);
        }
        else
        {
            return ((x + (x / int(M) + 2) * int(M)) % M);
        }
    }

    __forceinline int Floor(float x)
    {
        if(x >= 0.0f)
        {
            return int(x);
        }
        else
        {
            return int(x) - 1;
        }
    }

    __forceinline int Floor(double x)
    {
        if(x >= 0.0)
        {
            return int(x);
        }
        else
        {
            return int(x) - 1;
        }
    }

    __forceinline int Ceiling(float x)
    {
        int FloorX = Floor(x);
        if(x == float(FloorX))
        {
            return FloorX;
        }
        else
        {
            return FloorX + 1;
        }
    }

    __forceinline int Ceiling(double x)
    {
        int FloorX = Floor(x);
        if(x == double(FloorX))
        {
            return FloorX;
        }
        else
        {
            return FloorX + 1;
        }
    }

    __forceinline int Round(float x)
    {
        return int(x + 0.5f);
    }

    __forceinline int Round(double x)
    {
        return int(x + 0.5);
    }

    template <class type> __forceinline type Square(type T)
    {
        return T * T;
    }

    template <class type> __forceinline type Min(type A, type B)
    {
        if(A < B)
        {
            return A;
        }
        else
        {
            return B;
        }
    }

    template <class type> __forceinline type Min(type A, type B, type C)
    {
        if(A < B && A < C)
        {
            return A;
        }
        else if(B < C)
        {
            return B;
        }
        else
        {
            return C;
        }
    }

    template <class type> __forceinline type Max(type A, type B)
    {
        if(A > B)
        {
            return A;
        }
        else
        {
            return B;
        }
    }

    template <class type> __forceinline type Max(type A, type B, type C)
    {
        if(A > B && A > C)
        {
            return A;
        }
        else if(B > C)
        {
            return B;
        }
        else
        {
            return C;
        }
    }

} // end Math namespace

//
// Utility namespace contains useful, generic functions
//
namespace Utility
{
    struct StringComparison
    {
        bool operator()(const char *L, const char *R) const
        {
            return strcmp(L, R) < 0;
        }
    };

    //
    // between returns true if a is between b and c, inclusive
    //
    #define between(a, b, c)    ( (a >= b) && (a <= c) )

    //
    // D3DValidate is used to assert that a DirectX call succeeded
    //
    #define D3DValidate(x, Function) { HRESULT hr = (x); if(FAILED(hr) && hr != 0x8876086c) SignalError(String("The function ") + Function + String(" has unexpectedly failed; the program will now abort."));}
    #define D3DAlwaysValidate(x, Function) { HRESULT hr = (x); if(FAILED(hr)) PersistentSignalError(String("The function ") + Function + String(" has unexpectedly failed; the program will now abort.")); }
    #define D3DValidateRelease(x) { ULONG References = x->Release(); PersistentAssert(References == 0, String("Release reference count: ") + String(References)); }

    UINT32 Hash32(const BYTE *Start, UINT Length);
    UINT64 Hash64(const BYTE *Start, UINT Length);

    template <class type> __forceinline UINT32 Hash32(const type &Obj)
    {
        return Hash32((const BYTE *)&Obj, sizeof(type));
    }
    template <class type> __forceinline UINT64 Hash64(const type &Obj)
    {
        return Hash64((const BYTE *)&Obj, sizeof(type));
    }

    __forceinline UINT CastBoolToUINT(bool b)
    {
        if(b)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    template <class type> __forceinline type& Choose(type& Choice0, type& Choice1, bool ChooseChoice0)
    {
        if(ChooseChoice0)
        {
            return Choice0;
        }
        else
        {
            return Choice1;
        }
    }

    template <class type> __forceinline const type& Choose(const type& Choice0, const type& Choice1, bool ChooseChoice0)
    {
        if(ChooseChoice0)
        {
            return Choice0;
        }
        else
        {
            return Choice1;
        }
    }

    template <class type> __forceinline type& Choose(type& Choice0, type& Choice1, UINT Index)
    {
        if(Index == 0)
        {
            return Choice0;
        }
        else if(Index == 1)
        {
            return Choice1;
        }
        return Choice0;
    }

    template <class type> __forceinline type& Choose(type& Choice0, type& Choice1, type& Choice2, UINT Index)
    {
        if(Index == 0)
        {
            return Choice0;
        }
        else if(Index == 1)
        {
            return Choice1;
        }
        else if(Index == 2)
        {
            return Choice2;
        }
        return Choice0;
    }

    template <class type> __forceinline const type& Choose(const type& Choice0, const type& Choice1, UINT Index)
    {
        if(Index == 0)
        {
            return Choice0;
        }
        else if(Index == 1)
        {
            return Choice1;
        }
        return Choice0;
    }

    template <class type> __forceinline type Bound(type Value, type Low, type High)
    {
        if(Value < Low)
        {
            Value = Low;
        }
        if(Value > High)
        {
            Value = High;
        }
        return Value;
    }

    template <class type> __forceinline BYTE BoundToByte(type Value)
    {
        if(Value < 0)
        {
            Value = 0;
        }
        if(Value > 255)
        {
            Value = 255;
        }
        return BYTE(Value);
    }

    //
    // swaps an arbitrary type of data
    //
    template <class type> __forceinline void Swap(type &t1, type &t2)
    {
        type Temp = std::move(t1);
        t1 = std::move(t2);
        t2 = std::move(Temp);
    }

    template <class type> __forceinline void ZeroMem(type &t)
    {
        memset(&t, 0, sizeof(type));
        /*BYTE *B = &t;
        for(UINT ByteIndex = 0; ByteIndex < sizeof(type); ByteIndex++)
        {
            B[ByteIndex] = 0;
        }*/
    }

    //
    // Displays a Win32 message box with the data string.
    //
    void MessageBox(const char *String);
    void MessageBox(const String &S);
    void CopyStringToClipboard(const String &S);
    String LoadStringFromClipboard();
    void GetClipboardLines(Vector<String> &Output);

    bool FileExists(const String &Filename);

    //
    // Returns the next line in the given file
    //
    String GetNextLine(ifstream &File);
    void GetFileData(const String &Filename, Vector<BYTE> &Output);

    //
    // Returns the set of all lines in the given file
    //
    void GetFileLines(ifstream &File, Vector<String> &Output, UINT minLineLength = 0);
    void GetFileLines(const String &Filename, Vector<String> &Output, UINT minLineLength = 0);
    __forceinline Vector<String> GetFileLines(ifstream &file, UINT minLineLength = 0)
    {
        Vector<String> output;
        GetFileLines(file, output, minLineLength);
        return output;
    }
    __forceinline Vector<String> GetFileLines(const String &filename, UINT minLineLength = 0)
    {
        Vector<String> output;
        GetFileLines(filename, output, minLineLength);
        return output;
    }

    void GetUnicodeFileLines(ifstream &File, Vector<UnicodeString> &Output);
    void GetUnicodeFileLines(const String &Filename, Vector<UnicodeString> &Output, UINT LineLimit = 0);

    UINT GetFileSize(const String &Filename);
    void CopyFile(const String &SourceFile, const String &DestFile);

    FILE* CheckedFOpen(const char *Filename, const char *Mode);

    __forceinline void CheckedFRead(void *Dest, UINT ElementSize, UINT ElementCount, FILE *File)
    {
        size_t ElementsRead = fread(Dest, ElementSize, ElementCount, File);
        PersistentAssert(!ferror(File) && ElementsRead == ElementCount, "fread failed");
    }

    __forceinline void CheckedFWrite(const void *Src, UINT ElementSize, UINT ElementCount, FILE *File)
    {
        size_t ElementsWritten = fwrite(Src, ElementSize, ElementCount, File);
        PersistentAssert(!ferror(File) && ElementsWritten == ElementCount, "fwrite failed");
    }

    __forceinline void CheckedFSeek(UINT Offset, FILE *File)
    {
        int Result = fseek(File, Offset, SEEK_SET);
        PersistentAssert(!ferror(File) && Result == 0, "fseek failed");
    }

    //
    // Create a process with the given command line
    //
    int RunCommand(const String &ExecutablePath, const String &CommandLine, bool Blocking);

    void MakeDirectory(const String &directory);

} // end Utility namespace