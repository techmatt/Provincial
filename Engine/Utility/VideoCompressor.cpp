/*
VideoCompressor.h
Written by Matthew Fisher

VideoCompressor takes a sequence of images and compressed them into a video file.
*/

#ifdef USE_WMF
#include <Mfapi.h>

VideoCompressor::VideoCompressor()
{
    _Writer = NULL;
    _Sample = NULL;
    _Buffer = NULL;
}

VideoCompressor::~VideoCompressor()
{
    FreeMemory();
}

void VideoCompressor::FreeMemory()
{
    if(_Sample)
    {
        _Sample->Release();
        _Sample = NULL;
    }
    if(_Buffer)
    {
        _Buffer->Release();
        _Buffer = NULL;
    }
    if(_AudioCapture.Capturing())
    {
        _AudioCapture.StopCapture();
    }
    if(_Writer)
    {
        HRESULT hr = _Writer->Finalize();
        PersistentAssert(SUCCEEDED(hr), "Finalize failed");
        _Writer->Release();
        _Writer = NULL;
    }
}

enum eAVEncH264VProfile {
  eAVEncH264VProfile_unknown    = 0,
  eAVEncH264VProfile_Simple     = 66,
  eAVEncH264VProfile_Base       = 66,
  eAVEncH264VProfile_Main       = 77,
  eAVEncH264VProfile_High       = 100,
  eAVEncH264VProfile_422        = 122,
  eAVEncH264VProfile_High10     = 110,
  eAVEncH264VProfile_444        = 144,
  eAVEncH264VProfile_Extended   = 88 
};

void VideoCompressor::InitMediaType(IMFMediaType *M, const GUID &Format, UINT BitRate, UINT Width, UINT Height, UINT FrameRate)
{
    M->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    M->SetGUID(MF_MT_SUBTYPE, Format);
    M->SetUINT32(MF_MT_AVG_BITRATE, BitRate);
    MFSetAttributeSize(M, MF_MT_FRAME_SIZE, Width, Height);
    MFSetAttributeRatio(M, MF_MT_FRAME_RATE, FrameRate, 1);
    MFSetAttributeRatio(M, MF_MT_FRAME_RATE_RANGE_MAX, FrameRate, 1);
    MFSetAttributeRatio(M, MF_MT_FRAME_RATE_RANGE_MIN, FrameRate / 2, 1);
    MFSetAttributeRatio(M, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    M->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    M->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, 1);
    M->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, 1);
    M->SetUINT32(MF_MT_SAMPLE_SIZE, Width * Height * 4);
    M->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_Main); //eAVEncH264VProfile_Base

    //M->SetUINT32(MF_MT_DEFAULT_STRIDE, -960);
    //M->SetGUID(MF_MT_AM_FORMAT_TYPE, Webcam);
}

//
// Audio information
// http://msdn.microsoft.com/en-us/library/ff819476%28VS.85%29.aspx
// http://msdn.microsoft.com/en-us/library/dd742785%28v=VS.85%29.aspx
//

VideoCompressorResult VideoCompressor::OpenFile(const String &Filename, UINT Width, UINT Height, UINT BitRate, UINT FrameRate, UINT AudioDeviceIndex, Clock *Timer)
{
    VideoCompressorResult Result = VideoCompressorResultSuccess;
    _Width = Width;
    _Height = Height;
    _CapturingAudio = (AudioDeviceIndex != 0xFFFFFFFF);
    _Clock = Timer;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    //PersistentAssert(SUCCEEDED(hr), "CoInitializeEx failed");

    hr = MFStartup(MF_VERSION);
    PersistentAssert(SUCCEEDED(hr), "MFStartup failed");
    
    hr = MFCreateSinkWriterFromURL(
            UnicodeString(Filename).CString(),
            NULL,
            NULL,
            &_Writer
            );
    PersistentAssert(SUCCEEDED(hr), "MFCreateSinkWriterFromURL failed");

    const UINT RawBufferSize = Width * Height * 4;
    
    IMFMediaType *OutputMediaType;
    MFCreateMediaType(&OutputMediaType);
    InitMediaType(OutputMediaType, MFVideoFormat_H264, BitRate, Width, Height, FrameRate);

    IMFMediaType *InputMediaType;
    MFCreateMediaType(&InputMediaType);
    InitMediaType(InputMediaType, MFVideoFormat_RGB32, RawBufferSize, Width, Height, FrameRate);

    DWORD VideoStreamIndex;
    hr = _Writer->AddStream(OutputMediaType, &VideoStreamIndex);
    PersistentAssert(SUCCEEDED(hr), "AddStream failed");
    OutputMediaType->Release();
    
    /*hr = MFTRegisterLocalByCLSID(
            __uuidof(CColorConvertDMO),
            MFT_CATEGORY_VIDEO_PROCESSOR,
            L"",
            MFT_ENUM_FLAG_SYNCMFT,
            0,
            NULL,
            0,
            NULL
            );
    PersistentAssert(SUCCEEDED(hr), "MFTRegisterLocalByCLSID failed");*/

    hr = _Writer->SetInputMediaType(VideoStreamIndex, InputMediaType, NULL);
    InputMediaType->Release();
    if(FAILED(hr))
    {
        if(Width > 1920 || Height > 1080)
        {
            MessageBox(NULL, "The maximum resolution for H.264 video is 1920x1080.", "Invalid Window Dimensions", MB_OK | MB_ICONERROR);
        }
        else
        {
            MessageBox(NULL, "There was an error when attempting to initialize video capture.  The maximum resolution for H.264 video is 1920x1080.", "Invalid Window Dimensions", MB_OK | MB_ICONERROR);
        }
        _Writer->Release();
        _Writer = NULL;
        _Clock = NULL;
        return VideoCompressorResultFailure;
    }
    
    if(_CapturingAudio)
    {
        //
        // Setup the output media type
        //
        IMFMediaType *OutputAudioType;
        hr = MFCreateMediaType( &OutputAudioType );
        PersistentAssert(SUCCEEDED(hr), "MFCreateMediaType failed");

        const UINT SamplesPerSecond = 44100;
        const UINT AverageBytesPerSecond = 24000;
        const UINT ChannelCount = 2;
        const UINT BitsPerSample = 16;
        
        OutputAudioType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio ) ;  
        OutputAudioType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_AAC ) ;
        OutputAudioType->SetUINT32( MF_MT_AUDIO_SAMPLES_PER_SECOND, SamplesPerSecond ) ;
        OutputAudioType->SetUINT32( MF_MT_AUDIO_BITS_PER_SAMPLE, BitsPerSample ) ;
        OutputAudioType->SetUINT32( MF_MT_AUDIO_NUM_CHANNELS, ChannelCount ) ;
        OutputAudioType->SetUINT32( MF_MT_AUDIO_AVG_BYTES_PER_SECOND, AverageBytesPerSecond ) ;
        OutputAudioType->SetUINT32( MF_MT_AUDIO_BLOCK_ALIGNMENT, 1 ) ;
        //OutputAudioType->SetUINT32( MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29 ) ;

        DWORD AudioStreamIndex;
        hr = _Writer->AddStream( OutputAudioType, &AudioStreamIndex );
        PersistentAssert(SUCCEEDED(hr), "AddStream failed");

        //
        // Setup the input media type
        //

        IMFMediaType *InputAudioType;
        MFCreateMediaType( &InputAudioType );
        InputAudioType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
        InputAudioType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM );
        InputAudioType->SetUINT32( MF_MT_AUDIO_BITS_PER_SAMPLE, BitsPerSample );
        InputAudioType->SetUINT32( MF_MT_AUDIO_SAMPLES_PER_SECOND, SamplesPerSecond );
        InputAudioType->SetUINT32( MF_MT_AUDIO_NUM_CHANNELS, ChannelCount );

        hr = _Writer->SetInputMediaType( AudioStreamIndex, InputAudioType, NULL );
        PersistentAssert(SUCCEEDED(hr), "SetInputMediaType failed");

        _AudioCapture.StartCapture(this, AudioDeviceIndex);
    }

    hr = _Writer->BeginWriting();
    PersistentAssert(SUCCEEDED(hr), "BeginWriting failed");

    
    hr = MFCreateSample(&_Sample);
    PersistentAssert(SUCCEEDED(hr), "MFCreateSample failed");

    hr = MFCreateMemoryBuffer(RawBufferSize, &_Buffer);
    _Buffer->SetCurrentLength(RawBufferSize);
    _Sample->AddBuffer(_Buffer);

    return Result;
}

#ifdef D3D9_IN_NAMESPACE
void VideoCompressor::AddFrame(D3D9Base::LPDIRECT3DSURFACE9 Surface, const Vec2i &Start, double TimeInSeconds)
#else
void VideoCompressor::AddFrame(LPDIRECT3DSURFACE9 Surface, const Vec2i &Start, double TimeInSeconds)
#endif
{
    if(_Clock != NULL)
    {
        TimeInSeconds = _Clock->Elapsed();
    }
    _Sample->SetSampleTime( LONGLONG( TimeInSeconds * 10000000.0 ) );
        
    BYTE *BufferData;
    HRESULT hr = _Buffer->Lock(&BufferData, NULL, NULL);
    PersistentAssert(SUCCEEDED(hr), "_Buffer->Lock failed");

    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT LockedRect;
    D3DAlwaysValidate(Surface->GetDesc(&Desc), "GetDesc");
    PersistentAssert(Desc.Format == D3DFMT_A8R8G8B8 ||
                     Desc.Format == D3DFMT_X8R8G8B8, "Invalid surface format");
    D3DAlwaysValidate(Surface->LockRect(&LockedRect, NULL, 0), "LockRect");
    
    
    const UINT VideoWidth = _Width;
    const UINT VideoHeight = _Height;
    const UINT BufferPitch = sizeof(RGBColor) * VideoWidth;

    Vec2i CorrectedStart = Start;
    if(CorrectedStart.x < 0)
    {
        CorrectedStart.x = 0;
    }
    if(CorrectedStart.x + VideoWidth > Desc.Width)
    {
        CorrectedStart.x = Desc.Width - VideoWidth;
    }
    if(CorrectedStart.y < 0)
    {
        CorrectedStart.y = 0;
    }
    if(CorrectedStart.y + VideoHeight > Desc.Height)
    {
        CorrectedStart.y = Desc.Height - VideoHeight;
    }
    bool SizingCorrect = (CorrectedStart.x >= 0 && CorrectedStart.x + VideoWidth  <= Desc.Width ) &&
                         (CorrectedStart.y >= 0 && CorrectedStart.y + VideoHeight <= Desc.Height);
    if(!SizingCorrect)
    {
        for(UINT y = 0; y < VideoHeight; y++)
        {
            memset(BufferData + y * BufferPitch, 0, BufferPitch);
        }
    }
    else if(Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8)
    {
        for(UINT y = 0; y < VideoHeight; y++)
        {
            RGBColor *RowStart = (RGBColor *)((BYTE*)LockedRect.pBits +  (y + CorrectedStart.y) * LockedRect.Pitch);
            memcpy(BufferData + (VideoHeight - 1 - y) * BufferPitch, RowStart + CorrectedStart.x, BufferPitch);
            //for(UINT x = 0; x < _Width; x++)
            //{
            //    (*this)[y][x] = RowStart[x];
            //}
        }
    }
    D3DAlwaysValidate(Surface->UnlockRect(), "UnlockRect");

    _Buffer->Unlock();

    hr = _Writer->WriteSample(0, _Sample);
    PersistentAssert(SUCCEEDED(hr), "WriteSample failed");
}

void VideoCompressor::AddFrame(const Bitmap &Bmp, double TimeInSeconds)
{
    if(_Clock != NULL)
    {
        TimeInSeconds = _Clock->Elapsed();
    }
    _Sample->SetSampleTime( LONGLONG( TimeInSeconds * 10000000.0 ) );
        
    BYTE *BufferData;
    HRESULT hr = _Buffer->Lock(&BufferData, NULL, NULL);
    PersistentAssert(SUCCEEDED(hr), "_Buffer->Lock failed");

    memcpy( BufferData, &(Bmp[0][0]), Bmp.Width() * Bmp.Height() * sizeof(RGBColor) );
    _Buffer->Unlock();

    hr = _Writer->WriteSample(0, _Sample);
    PersistentAssert(SUCCEEDED(hr), "WriteSample failed");
}

void VideoCompressor::AudioSample32Bit2Channel(float *Samples, UINT FrameCount, UINT64 CaptureStartTime)
{
    //double TimeInSeconds = _Clock->Elapsed();

    const UINT SamplesPerSecond = 44100;
    const UINT ChannelCount = 2;
    const UINT SampleCount = FrameCount * ChannelCount;
    const UINT BitsPerSample = 16;
    const UINT BufferLength = BitsPerSample / 8 * ChannelCount * FrameCount;
    const LONGLONG SampleDuration = LONGLONG(FrameCount) * LONGLONG(10000000) / SamplesPerSecond; // in hns
    
    //
    // Write some data
    //
    IMFSample *spSample;
    IMFMediaBuffer *spBuffer;
    BYTE *pbBuffer = NULL;

    //
    // Create a media sample
    //

    HRESULT hr = MFCreateSample( &spSample );
    hr = spSample->SetSampleDuration( SampleDuration );
    
    //hr = spSample->SetSampleTime( LONGLONG( TimeInSeconds * 10000000.0 ) );
    
    //CaptureStartTime = 10,000,000 * t / f;
    //t = CaptureStartTime * f / 10,000,000
    LONGLONG FileStartCounter = _Clock->StartTime();
    LONGLONG CaptureStartCounter = CaptureStartTime * _Clock->TicksPerSecond() / LONGLONG(10000000);
    hr = spSample->SetSampleTime( ( CaptureStartCounter - FileStartCounter ) * LONGLONG(10000000) / _Clock->TicksPerSecond() );

    //
    // Add a media buffer filled with random data
    //

    hr = MFCreateMemoryBuffer( BufferLength, &spBuffer );
    hr = spBuffer->SetCurrentLength( BufferLength );
    hr = spSample->AddBuffer( spBuffer );

    hr = spBuffer->Lock( &pbBuffer, NULL, NULL );
    __int16 *OutputAudioBuffer = (__int16 *)pbBuffer;
    for(UINT SampleIndex = 0; SampleIndex < SampleCount; SampleIndex++)
    {
        //
        // Floats are in the range -1 to 1
        //
        OutputAudioBuffer[SampleIndex] = int(Samples[SampleIndex] * 32768.0f);
    }
    hr = spBuffer->Unlock();

    //
    // Write the media sample
    //
    hr = _Writer->WriteSample( 1, spSample );
    PersistentAssert(SUCCEEDED(hr), "WriteSample failed");

    spSample->Release();
    spBuffer->Release();
}
#endif