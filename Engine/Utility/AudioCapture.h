/*
AudioCapture.h
Written by Matthew Fisher

VideoCompressor takes a sequence of images and compressed them into a video file.
*/

#ifdef USE_WMF
struct IMMDeviceCollection;
struct IMMDevice;
class VideoCompressor;
class CWASAPICapture;

class AudioCapture
{
public:
    AudioCapture()
    {
        _Compressor = NULL;
        _CaptureDevice = NULL;
        _Capturer = NULL;
    }
    bool Capturing()
    {
        return (_Capturer != NULL);
    }
    bool StartCapture(VideoCompressor *Compressor, UINT AudioDeviceIndex);
    bool StartCapture(const String &Filename, UINT AudioDeviceIndex);
    void StopCapture();

private:
    bool StartCaptureInternal(UINT AudioDeviceIndex);

    CWASAPICapture*  _Capturer;
    Vector<BYTE>     _CaptureBuffer;
    String           _Filename;
    VideoCompressor* _Compressor;
    IMMDevice*       _CaptureDevice;
};
#endif