/*
VideoCompressor.h
Written by Matthew Fisher

VideoCompressor takes a sequence of images and compressed them into a video file.
*/

#ifdef USE_WMF
#include <Mfidl.h>
#include <Mfreadwrite.h>

enum VideoCompressorResult
{
    VideoCompressorResultSuccess = 0,
    VideoCompressorResultFailure = 1,
    VideoCompressorResultAudioFailed = 2,
};

class VideoCompressor
{
public:
    VideoCompressor();
    ~VideoCompressor();
    void FreeMemory();

    VideoCompressorResult OpenFile(const String &Filename, UINT Width, UINT Height, UINT BitRate, UINT FrameRate, UINT AudioDeviceIndex, Clock *Timer);
    void AddFrame(const Bitmap &Bmp, double TimeInSeconds);

#ifdef D3D9_IN_NAMESPACE
    void AddFrame(D3D9Base::LPDIRECT3DSURFACE9 Surface, const Vec2i &Start, double TimeInSeconds);
#else
    void AddFrame(LPDIRECT3DSURFACE9 Surface, const Vec2i &Start, double TimeInSeconds);
#endif
    void AudioSample32Bit2Channel(float *Samples, UINT FrameCount, UINT64 CaptureStartTime);

    __forceinline bool Capturing() const
    {
        return (_Writer != NULL);
    }

private:
    static void InitMediaType(IMFMediaType *M, const GUID &Format, UINT BitRate, UINT Width, UINT Height, UINT FrameRate);

    Clock*          _Clock;
    bool            _CapturingAudio;
    AudioCapture    _AudioCapture;
    IMFSinkWriter*  _Writer;
    IMFSample*      _Sample;
    IMFMediaBuffer* _Buffer;
    UINT            _Width;
    UINT            _Height;
};
#endif