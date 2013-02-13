/*
Bitmap.cpp
Written by Matthew Fisher

A bitmap class (a 2D array of RGBColor's)
Rather self-explanitory, can only save and load a few primitive formats.
See Bitmap.h for a complete description
*/

Bitmap::Bitmap()
{
    _Data = 0;
    _Width = 0;
    _Height = 0;
}

Bitmap::Bitmap(UINT Width, UINT Height)
{
    _Data = 0;
    Allocate(Width, Height);
}

Bitmap::Bitmap(UINT Width, UINT Height, RGBColor clearColor)
{
    _Data = 0;
    Allocate(Width, Height);
    Clear(clearColor);
}

Bitmap::~Bitmap()
{
    FreeMemory();
}

Bitmap::Bitmap(const Bitmap &B)
{
    _Data = NULL;
    Allocate(B._Width,B._Height);
    memcpy(_Data,B._Data,sizeof(RGBColor)*_Width*_Height);
}

Bitmap::Bitmap(Bitmap &&B)
{
    _Width = B._Width;
    _Height = B._Height;
    _Data = B._Data;
    B._Width = 0;
    B._Height = 0;
    B._Data = NULL;
}

Bitmap::Bitmap(const Bitmap &B, UINT x, UINT y, UINT Width, UINT Height)
{
    _Data = NULL;
    Allocate(Width, Height);
    Clear();
    for(UINT CurY = 0; CurY < _Height; CurY++)
    {
        for(UINT CurX = 0; CurX < _Width; CurX++)
        {
            _Data[CurY * _Width + CurX] = B[CurY + y][CurX + x];
        }
    }
}

void Bitmap::operator = (const Bitmap &B)
{
    if(&B == this) return;
    Allocate(B._Width,B._Height);
    memcpy(_Data,B._Data,sizeof(RGBColor)*_Width*_Height);
}

void Bitmap::operator = (Bitmap &&Bmp)
{
    if(this != &Bmp)
    {
        if(_Data != NULL)
        {
            delete[] _Data;
        }
        _Width = Bmp._Width;
        _Height = Bmp._Height;
        _Data = Bmp._Data;
        Bmp._Width = 0;
        Bmp._Height = 0;
        Bmp._Data = NULL;
    }
}

void Bitmap::FreeMemory()
{
    if(_Data)
    {
        delete[] _Data;
    }
    _Data = NULL;
    _Width = 0;
    _Height = 0;
}

void Bitmap::Allocate(UINT Width, UINT Height)
{
    if(_Data == NULL || _Width != Width || _Height != Height)
    {
        FreeMemory();
        _Width = Width;
        _Height = Height;
        _Data = new RGBColor[Width * Height];
    }
}

void Bitmap::Allocate(UINT Width, UINT Height, RGBColor clearColor)
{
    Allocate(Width, Height);
    Clear(clearColor);
}

void Bitmap::SaveBMP(const String &Filename) const
{
    BITMAPFILEHEADER    bmfh;  //stores information about the file format
    BITMAPINFOHEADER    bmih;  //stores information about the bitmap
    FILE                *file; //stores file pointer

    //create bitmap file header
    ((unsigned char *)&bmfh.bfType)[0] = 'B';
    ((unsigned char *)&bmfh.bfType)[1] = 'M';
    bmfh.bfSize = 54 + _Height * _Width * 4;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = 54;

    //create bitmap information header
    bmih.biSize = 40;
    bmih.biWidth = _Width;
    bmih.biHeight = _Height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = 0;
    bmih.biSizeImage = 0;
    bmih.biXPelsPerMeter = 3800;
    bmih.biYPelsPerMeter = 3800;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    //save all header and bitmap information into file
    file = Utility::CheckedFOpen(Filename.CString(), "wb");
    fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, file);
    fwrite(_Data, sizeof(RGBColor), _Width * _Height, file);
    fclose(file);
}

void Bitmap::LoadBMP(const String &Filename)
{
    BITMAPFILEHEADER    bmfh;  //stores information about the file format
    BITMAPINFOHEADER    bmih;  //stores information about the bitmap
    FILE                *file; //stores file pointer

    //open the file and read in the headers
    file = Utility::CheckedFOpen(Filename.CString(), "rb");

    fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&bmih, sizeof(BITMAPINFOHEADER), 1, file);

    _Width = bmih.biWidth;
    _Height = abs(bmih.biHeight);
    if(bmih.biBitCount == 32)
    {
        //Allocate space for the read operation
        Allocate(_Width, _Height);

        //save all header and bitmap information into file
        fread(_Data, sizeof(RGBColor), _Width * _Height, file);
    }
    else if(bmih.biBitCount == 24)
    {
        //Allocate space for the read operation
        Allocate(_Width, _Height);

        UINT Pitch = _Width * 3;
        UINT ExcessPitch = 0;
        while(double(Pitch / 4) != double(Pitch) / 4.0)
        {
            Pitch++;
            ExcessPitch++;
        }

        unsigned char *DataStore = new unsigned char[Pitch*_Height];
        fread(DataStore, 1, Pitch*_Height, file);

        UINT CurDataPos = 0;
        for(UINT i=0;i<_Height;i++)
        {
            for(UINT i2=0;i2<_Width;i2++)
            {
                RGBColor CurColor;
                CurColor.b = DataStore[CurDataPos++];
                CurColor.g = DataStore[CurDataPos++];
                CurColor.r = DataStore[CurDataPos++];
                CurColor.a = 0;
                _Data[i*_Width+i2] = CurColor;
            }
            CurDataPos += ExcessPitch;
        }

        delete[] DataStore;
    }
    else
    {
        SignalError("Invalid bit count");
    }
    fclose(file);
}

void Bitmap::LoadSDL(const String &Filename)
{
#ifdef USE_SDL
    SDL_Surface *Image = IMG_Load(Filename.CString());
    PersistentAssert(Image != NULL, String("SDL IMG_Load failed on ") + Filename);
    
    //SDL_Surface *FormattedImage = SDL_CreateRGBSurface(SDL_SWSURFACE, Image->w, Image->h, 32, 0, 0, 0, 0);
    SDL_Surface *FormattedImage = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32, 0, 0, 0, 0);

    /*SDL_PixelFormat Format;
    Format.palette = NULL;
    Format.BitsPerPixel = 32;
    Format.BytesPerPixel = 4;
    Format.*/

    Utility::Swap(FormattedImage->format->Rmask, FormattedImage->format->Bmask);
    
    SDL_Surface *ConvertedImage = SDL_ConvertSurface(Image, FormattedImage->format, SDL_SWSURFACE);
    PersistentAssert(ConvertedImage != NULL, String("SDL_ConvertSurface failed on ") + Filename);

    Allocate(ConvertedImage->w, ConvertedImage->h);
    //memcpy(_Data, ConvertedImage->pixels, sizeof(RGBColor) * _Width * _Height);
    for(UINT y = 0; y < _Height; y++)
    {
        memcpy(_Data + _Width * y, ((RGBColor *)ConvertedImage->pixels) + _Width * (_Height - 1 - y), sizeof(RGBColor) * _Width);
    }

    SDL_FreeSurface(ConvertedImage);
    SDL_FreeSurface(FormattedImage);
    SDL_FreeSurface(Image);
#else
    PersistentSignalError("LoadSDL called without USE_SDL");
#endif
}

#ifdef USE_PNG
struct PNGDirectMemoryIORead
{
    const Vector<BYTE> *Buffer;
    UINT Offset;
};

void Bitmap::PNGReadFromBuffer(png_structp png_ptr, png_bytep data, png_size_t length)
{
    PNGDirectMemoryIORead *ReadInfo = (PNGDirectMemoryIORead *)png_get_io_ptr(png_ptr);
    memcpy(data, ReadInfo->Buffer->CArray() + ReadInfo->Offset, length);
    ReadInfo->Offset += (UINT)length;
}

struct PNGDirectMemoryIOWrite
{
    Vector<BYTE> *Buffer;
};

void Bitmap::PNGWriteToBuffer(png_structp png_ptr, png_bytep data, png_size_t length)
{
    PNGDirectMemoryIOWrite *WriteInfo = (PNGDirectMemoryIOWrite *)png_get_io_ptr(png_ptr);
    UINT StartLength = WriteInfo->Buffer->Length();
    WriteInfo->Buffer->ReSize(StartLength + (UINT)length);
    memcpy(WriteInfo->Buffer->CArray() + StartLength, data, length);
}

void Bitmap::PNGFlushBuffer(png_structp png_ptr)
{
    
}
#endif

void Bitmap::LoadPNG(const String &Filename)
{
#ifdef USE_PNG
    FILE *File;
    File = fopen(Filename.CString(), "rb");
    PersistentAssert(File != NULL, String("File open for LoadPNG failed: ") + String(Filename));

    png_structp PngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    PersistentAssert(PngRead != NULL, "png_create_read_struct failed.");

    png_infop PngInfo = png_create_info_struct(PngRead);
    PersistentAssert(PngInfo != NULL, "png_create_info_struct failed.");

    png_init_io(PngRead, File);
    PNGCompleteRead(PngRead, PngInfo, Filename);

    fclose(File);
#else
    SignalError("LoadPNG called without USE_PNG");
#endif
}

#ifdef USE_PNG
void Bitmap::LoadPNGFromMemory(const Vector<BYTE> &Buffer)
{
    png_structp PngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    Assert(PngRead != NULL, "png_create_read_struct failed.");

    png_infop PngInfo = png_create_info_struct(PngRead);
    Assert(PngInfo != NULL, "png_create_info_struct failed.");

    PNGDirectMemoryIORead ReadInfo;
    ReadInfo.Buffer = &Buffer;
    ReadInfo.Offset = 0;

    png_set_read_fn(PngRead, (void *)&ReadInfo, Bitmap::PNGReadFromBuffer);
    PNGCompleteRead(PngRead, PngInfo, "Memory");
}

void Bitmap::PNGCompleteRead(png_structp PngRead, png_infop PngInfo, const String &Filename)
{
    png_read_png(PngRead, PngInfo, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
    png_bytepp RowPointers = png_get_rows(PngRead, PngInfo);
    Assert(RowPointers != NULL, "png_get_rows failed.");

    Allocate(PngRead->width, PngRead->height);
    if(PngRead->channels == 3 && PngRead->pixel_depth == 24)
    {
        for(UINT y = 0; y < _Height; y++)
        {
            png_bytep CurRow = RowPointers[_Height - 1 - y];
            for(UINT x = 0; x < _Width; x++)
            {
                (*this)[y][x].r = CurRow[x * 3 + 0];
                (*this)[y][x].g = CurRow[x * 3 + 1];
                (*this)[y][x].b = CurRow[x * 3 + 2];
                (*this)[y][x].a = 0;
            }
        }
    }
    else if(PngRead->channels == 4 && PngRead->pixel_depth == 32)
    {
        for(UINT y = 0; y < _Height; y++)
        {
            png_bytep CurRow = RowPointers[_Height - 1 - y];
            for(UINT x = 0; x < _Width; x++)
            {
                (*this)[y][x].r = CurRow[x * 4 + 0];
                (*this)[y][x].g = CurRow[x * 4 + 1];
                (*this)[y][x].b = CurRow[x * 4 + 2];
                (*this)[y][x].a = CurRow[x * 4 + 3];
            }
        }
    }
    else if(PngRead->channels == 1 && PngRead->pixel_depth == 8)
    {
        for(UINT y = 0; y < _Height; y++)
        {
            png_bytep CurRow = RowPointers[_Height - 1 - y];
            for(UINT x = 0; x < _Width; x++)
            {
                BYTE C = CurRow[x];
                (*this)[y][x] = RGBColor(C, C, C, 0);
            }
        }
    }
    else
    {
        /*PersistentAssert(NULL, String("Unsupported channel # / pixel depth in ") + Filename + String("; ") +
                                      String(PngRead->channels) + String(" channels ") + String(PngRead->pixel_depth) +
                                      String(" bits per pixel"));*/
        Console::WriteLine(String("Unsupported channel # / pixel depth in ") + Filename + String("; ") +
                                      String(PngRead->channels) + String(" channels ") + String(PngRead->pixel_depth) +
                                      String(" bits per pixel"));
        Clear(RGBColor::Magenta);
    }

    png_destroy_read_struct(&PngRead, &PngInfo, NULL);
}
#endif

void Bitmap::SavePNG(const String &Filename) const
{
#ifdef USE_PNG
    BitmapSaveOptions Options;
    SavePNG(Filename, Options);
#else
    SignalError("SavePNG called without USE_PNG");
#endif
}

#ifdef USE_PNG
void Bitmap::SavePNG(const String &Filename, const BitmapSaveOptions &Options) const
{
    PersistentAssert(_Width > 0 && _Height > 0, "Saving empty image");

    FILE *File;
    File = fopen(Filename.CString(), "wb");
    PersistentAssert(File != NULL, String("File open for SavePNG failed: ") + Filename);

    png_structp PngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    PersistentAssert(PngWrite != NULL, "png_create_write_struct failed.");

    png_infop PngInfo = png_create_info_struct(PngWrite);
    PersistentAssert(PngInfo != NULL, "png_create_info_struct failed.");

    png_init_io(PngWrite, File);

    PNGCompleteWrite(PngWrite, PngInfo, Options);

    fclose(File);
}

void Bitmap::SavePNGToMemory(Vector<BYTE> &Buffer) const
{
    BitmapSaveOptions Options;
    SavePNGToMemory(Buffer, Options);
}

Vector<BYTE> Bitmap::SavePNGToMemory() const
{
    Vector<BYTE> buffer;
    BitmapSaveOptions options;
    SavePNGToMemory(buffer, options);
    return buffer;
}

void Bitmap::SavePNGToMemory(Vector<BYTE> &Buffer, const BitmapSaveOptions &Options) const
{
    Buffer.FreeMemory();
    png_structp PngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    Assert(PngWrite != NULL, "png_create_write_struct failed");

    png_infop PngInfo = png_create_info_struct(PngWrite);
    Assert(PngInfo != NULL, "png_create_info_struct failed");

    PNGDirectMemoryIORead WriteInfo;
    WriteInfo.Buffer = &Buffer;

    png_set_write_fn(PngWrite, (void *)&WriteInfo, Bitmap::PNGWriteToBuffer, Bitmap::PNGFlushBuffer);
    PNGCompleteWrite(PngWrite, PngInfo, Options);
}

void Bitmap::PNGCompleteWrite(png_structp PngWrite, png_infop PngInfo, const BitmapSaveOptions &Options) const
{
    UINT LocalWidth = _Width;
    UINT LocalHeight = _Height;
    if(Options.Region.Max != Vec2i::Origin)
    {
        LocalWidth = Options.Region.Dimensions().x;
        LocalHeight = Options.Region.Dimensions().y;
    }

    if(Options.SaveAlpha)
    {
        png_set_IHDR(PngWrite, PngInfo, LocalWidth, LocalHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }
    else
    {
        png_set_IHDR(PngWrite, PngInfo, LocalWidth, LocalHeight, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }
    png_set_PLTE(PngWrite, PngInfo, NULL, 0);
    png_set_gAMA(PngWrite, PngInfo, 0.0);
    //png_set_sRGB_gAMA_and_cHRM(PngWrite, PngInfo, PNG_sRGB_INTENT_PERCEPTUAL);
    
    png_color_8 ColorInfo;

    ColorInfo.alpha = 0;
    UINT BytesPerPixel = 3;
    if(Options.SaveAlpha)
    {
        ColorInfo.alpha = 8;
        BytesPerPixel = 4;
    }
    ColorInfo.red = 8;
    ColorInfo.green = 8;
    ColorInfo.blue = 8;
    ColorInfo.gray = 0;
    png_set_sBIT(PngWrite, PngInfo, &ColorInfo);
    
    
    UINT ScratchSizeNeeded = (sizeof(png_bytep) + sizeof(png_byte) * BytesPerPixel * LocalWidth) * LocalHeight;
    BYTE *ScratchSpace = Options.ScratchSpace;
    BYTE *ScratchSpaceStart = NULL;
    if(Options.ScratchSpaceSize < ScratchSizeNeeded)
    {
        ScratchSpace = new BYTE[ScratchSizeNeeded];
        ScratchSpaceStart = ScratchSpace;
    }

    png_bytep *RowPointers = (png_bytep *)ScratchSpace;
    ScratchSpace += sizeof(png_bytep) * LocalHeight;
    for(UINT y = 0; y < LocalHeight; y++)
    {
        BYTE *DestRowStart = ScratchSpace;
        ScratchSpace += sizeof(png_byte) * BytesPerPixel * LocalWidth;
        RowPointers[LocalHeight - 1 - y] = DestRowStart;
        
        const RGBColor *SrcRowStart = (*this)[y + Options.Region.Min.y];
        UINT DestRowStartOffset = 0;
        
        if(Options.UseBGR)
        {
            if(Options.SaveAlpha)
            {
                for(UINT x = 0; x < LocalWidth; x++)
                {
                    RGBColor Color = SrcRowStart[x + Options.Region.Min.x];
                    DestRowStart[DestRowStartOffset++] = Color.b;
                    DestRowStart[DestRowStartOffset++] = Color.g;
                    DestRowStart[DestRowStartOffset++] = Color.r;
                    DestRowStart[DestRowStartOffset++] = Color.a;
                }
            }
            else
            {
                for(UINT x = 0; x < LocalWidth; x++)
                {
                    RGBColor Color = SrcRowStart[x + Options.Region.Min.x];
                    DestRowStart[DestRowStartOffset++] = Color.b;
                    DestRowStart[DestRowStartOffset++] = Color.g;
                    DestRowStart[DestRowStartOffset++] = Color.r;
                }
            }
        }
        else
        {
            if(Options.SaveAlpha)
            {
                for(UINT x = 0; x < LocalWidth; x++)
                {
                    RGBColor Color = SrcRowStart[x + Options.Region.Min.x];
                    DestRowStart[DestRowStartOffset++] = Color.r;
                    DestRowStart[DestRowStartOffset++] = Color.g;
                    DestRowStart[DestRowStartOffset++] = Color.b;
                    DestRowStart[DestRowStartOffset++] = Color.a;
                }
            }
            else
            {
                for(UINT x = 0; x < LocalWidth; x++)
                {
                    RGBColor Color = SrcRowStart[x + Options.Region.Min.x];
                    DestRowStart[DestRowStartOffset++] = Color.r;
                    DestRowStart[DestRowStartOffset++] = Color.g;
                    DestRowStart[DestRowStartOffset++] = Color.b;
                }
            }
        }
    }

    png_set_rows(PngWrite, PngInfo, RowPointers);
    png_write_png(PngWrite, PngInfo, NULL, NULL);

    png_destroy_write_struct(&PngWrite, &PngInfo);

    if(ScratchSpaceStart != NULL)
    {
        delete[] ScratchSpaceStart;
    }
}
#endif

void Bitmap::SavePPM(const String &Filename) const
{
    //PPM is a very simple ASCII format
    String FilenameCopy = Filename;
    ofstream file(FilenameCopy.CString());
    file << "P3" << endl;
    file << "# PPM saved to " << FilenameCopy.CString() << endl;
    file << _Width << ' ' << _Height << endl;
    file << 255 << endl;    //maximum value of a component

    for(UINT i2=0;i2<_Height;i2++)
    {
        for(UINT i=0;i<_Width;i++)
        {
            file << int((*this)[i2][i].r) << ' ' << int((*this)[i2][i].g) << ' ' << int((*this)[i2][i].b) << endl;
        }
    }
}

void Bitmap::SavePGM(const String &Filename, int Channel) const
{
    FILE *File;
    File = fopen(Filename.CString(), "wb");
    PersistentAssert(File != NULL, "File open for SavePNG failed.");

    fprintf(File, "P5\n%d %d\n255\n", _Width, _Height);

    for (UINT y = 0; y < _Height; y++)
    {
        for (UINT x = 0; x < _Width; x++)
        {
            unsigned char Value;
            RGBColor Color = (*this)[y][x];
            switch(Channel)
            {
            case 0:
                Value = Color.r;
                break;
            case 1:
                Value = Color.g;
                break;
            case 2:
                Value = Color.b;
                break;
            case 3:
                Value = Color.a;
                break;
            case 4:
            default:
                Value = Utility::BoundToByte((int(Color.r) + int(Color.g) + int(Color.b)) / 3);
            }
            fputc(Value, File);
        }
    }
}

void Bitmap::LoadAlphaChannelAsGrayscale()
{
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            RGBColor &C = (*this)[y][x];
            C.r = C.a;
            C.g = C.a;
            C.b = C.a;
        }
    }
}

void Bitmap::FlipBlueAndRed()
{
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            RGBColor &C = (*this)[y][x];
            unsigned char Temp = C.r;
            C.r = C.b;
            C.b = Temp;
        }
    }
}

void Bitmap::DownsampleQuarter()
{
    Bitmap copy = *this;
    Allocate(_Width / 2, _Height / 2);
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            Vec3f sum = Vec3f(copy[y * 2 + 0][x * 2 + 0]);
            sum += Vec3f(copy[y * 2 + 0][x * 2 + 1]);
            sum += Vec3f(copy[y * 2 + 1][x * 2 + 0]);
            sum += Vec3f(copy[y * 2 + 1][x * 2 + 1]);
            sum *= 0.25f;
            (*this)[y][x] = RGBColor(sum);
        }
    }
}

void Bitmap::Clear()
{
    memset(_Data, 0, sizeof(RGBColor) * _Width * _Height);
}

void Bitmap::Clear(const RGBColor &Color)
{
    for(UINT i=0;i<_Width;i++)
    {
        _Data[i] = Color;
    }

    for(UINT i2=1;i2<_Height;i2++)
    {
        memcpy(&_Data[i2*_Width], &_Data[0], sizeof(RGBColor) * _Width);
    }
}

void Bitmap::BltTo(Bitmap &B, int TargetX, int TargetY) const
{
    BltTo(B, TargetX, TargetY, 0, 0, _Width, _Height);
}

void Bitmap::BltToNoClipMemcpy(Bitmap &B, UINT TargetX, UINT TargetY) const
{
    const UINT Height = _Height;
    const UINT Width = _Width;
    for(UINT y = 0; y < Height; y++)
    {
        const UINT CurTargetY = y + TargetY;
        memcpy(B._Data + (CurTargetY * B._Width + TargetX), _Data + y * _Width, sizeof(RGBColor) * Width);
    }
}

void Bitmap::BltTo(Bitmap &B, int TargetX, int TargetY, int SourceX, int SourceY, int Width, int Height) const
{
    const int BHeight = B._Height;
    const int BWidth = B._Width;
    for(int y = 0; y < Height; y++)
    {
        const int CurTargetY = y + TargetY;
        const int CurSourceY = y + SourceY;
        if(CurTargetY >= 0 && CurTargetY < BHeight &&
           CurSourceY >= 0 && CurSourceY < int(_Height))
        {
            for(int x = 0; x < Width; x++)
            {
                const int CurTargetX = x + TargetX;
                const int CurSourceX = x + SourceX;
                if(CurTargetX >= 0 && CurTargetX < BWidth &&
                   CurSourceX >= 0 && CurSourceX < int(_Width))
                {
                    B[CurTargetY][CurTargetX] = (*this)[CurSourceY][CurSourceX];
                }
            }
        }
    }
}

void Bitmap::StretchBltTo(Bitmap &B, const Rectangle2i &TargetRect, const Rectangle2i &SourceRect, SamplingType Sampling) const
{
    StretchBltTo(B,
        TargetRect.Min.x, TargetRect.Min.y, TargetRect.Width(), TargetRect.Height(), 
        SourceRect.Min.x, SourceRect.Min.y, SourceRect.Width(), SourceRect.Height(), Sampling);
}

void Bitmap::StretchBltTo(Bitmap &B, int TargetX, int TargetY, int TargetWidth, int TargetHeight, int SourceX, int SourceY, int SourceWidth, int SourceHeight, SamplingType Sampling) const
{
    if(Sampling == SamplingPoint)
    {
        const float RatioX = float(SourceWidth) / float(TargetWidth);
        const float RatioY = float(SourceHeight) / float(TargetHeight);
        const float SourceXOffset = float(SourceX + 0.5f * RatioX) + 0.5f;
        const float SourceYOffset = float(SourceY + 0.5f * RatioY) + 0.5f;
        const int Width = _Width;
        const int Height = _Height;
        for(int y = 0; y < TargetHeight; y++)
        {
            for(int x = 0; x < TargetWidth; x++)
            {
                int XMiddle = int(x * RatioX + SourceXOffset);
                int YMiddle = int(y * RatioY + SourceYOffset);
                if(XMiddle >= 0 && XMiddle < Width && YMiddle >= 0 && YMiddle < Height)
                {
                    B._Data[(y + TargetY) * B._Width + x + TargetX] = _Data[YMiddle * _Width + XMiddle];
                }
            }
        }
    }
    else if(Sampling == SamplingLinear)
    {
        for(int y = 0; y < TargetHeight; y++)
        {
            for(int x = 0; x < TargetWidth; x++)
            {
                float XStart = Math::LinearMap(0.0f, float(TargetWidth),  float(SourceX), float(SourceX + SourceWidth ), float(x + 0));
                float XEnd =   Math::LinearMap(0.0f, float(TargetWidth),  float(SourceX), float(SourceX + SourceWidth ), float(x + 1));
                float YStart = Math::LinearMap(0.0f, float(TargetHeight), float(SourceY), float(SourceY + SourceHeight), float(y + 0));
                float YEnd =   Math::LinearMap(0.0f, float(TargetHeight), float(SourceY), float(SourceY + SourceHeight), float(y + 1));

                B[y + TargetY][x + TargetX] = AverageColorOverRegion(Rectangle2f(Vec2f(XStart, YStart), Vec2f(XEnd, YEnd)));
            }
        }
    }
}

RGBColor Bitmap::AverageColorOverRegion(const Rectangle2f &Region) const
{
    Vec3f Result = Vec3f::Origin;
    int XStart = Math::Max(Math::Floor(Region.Min.x), 0);
    int YStart = Math::Max(Math::Floor(Region.Min.y), 0);
    int XEnd = Math::Min(Math::Ceiling(Region.Max.x), int(_Width) - 1);
    int YEnd = Math::Min(Math::Ceiling(Region.Max.y), int(_Height) - 1);

    const RGBColor *Data = _Data;
    const UINT Width = _Width;
    UINT Count = 0;
    for(int y = YStart; y <= YEnd; y++)
    {
        for(int x = XStart; x <= XEnd; x++)
        {
            Count++;
            Result += Vec3f(Data[y * Width + x]);
        }
    }
    return RGBColor(Result * (1.0f / float(Count)));
}

void Bitmap::LoadGrid(const Grid<float> &Values)
{
    float Min = Values(0, 0);
    float Max = Values(0, 0);
    for(UINT y = 0; y < Values.Rows(); y++)
    {
        for(UINT x = 0; x < Values.Cols(); x++)
        {
            if(Values(y, x) != 0.0f)
            {
                if(Min == 0.0f)
                {
                    Min = Values(y, x);
                }
                Min = Math::Min(Min, Values(y, x));
            }
            Max = Math::Max(Max, Values(y, x));
        }
    }
    LoadGrid(Values, Min, Max);
}

void Bitmap::LoadGrid(const Grid<float> &Values, float Min, float Max)
{
    Allocate(Values.Cols(), Values.Rows());
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            BYTE Intensity = 0;
            if(Max != Min)
            {
                Intensity = Utility::BoundToByte(Math::LinearMap(Min, Max, 0.0f, 255.0f, Values(y, x)));
            }
            if(Values(y, x) == 0.0f)
            {
                (*this)[y][x] = RGBColor::Magenta;
            }
            else
            {
                (*this)[y][x] = RGBColor(Intensity, Intensity, Intensity);
            }
        }
    }
}

/*BITMAP_HASH Bitmap::UnorientedHash() const
{
    BITMAP_HASH Result = _Width + _Height + _Width * _Height + _Width * _Width * _Height;
    for(UINT y=0;y<_Height;y++)
    {
        for(UINT x=0;x<_Width;x++)
        {
            RGBColor C = (*this)[y][x];
            Result += int(C.r) * 541 +
                      int(C.g) * 978 +
                      int(C.b) * 1024 +
                      int(C.r) * int(C.g);
        }
    }
    return Result;
}*/

void Bitmap::ReplaceColor(RGBColor CurrentColor, RGBColor NewColor)
{
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            if((*this)[y][x] == CurrentColor)
            {
                (*this)[y][x] = NewColor;
            }
        }
    }
}

UINT32 Bitmap::Hash32() const
{
    return Utility::Hash32((BYTE *)_Data, sizeof(RGBColor) * _Width * _Height) + (_Width * 785 + _Height * 97);
}

UINT64 Bitmap::Hash64() const
{
    return Utility::Hash64((BYTE *)_Data, sizeof(RGBColor) * _Width * _Height) + (_Width * 785 + _Height * 97);
}

UINT Bitmap::CountPixelsWithColor(RGBColor Color) const
{
    UINT Result = 0;
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            if(_Data[y * _Width + x] == Color)
            {
                Result++;
            }
        }
    }
    return Result;
}

bool Bitmap::Monochrome(RGBColor Color) const
{
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            if(_Data[y * _Width + x] != Color)
            {
                return false;
            }
        }
    }
    return true;
}

bool Bitmap::MonochromeIncludingAlpha(RGBColor Color) const
{
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            const RGBColor C = _Data[y * _Width + x];
            if(C.r != Color.r || C.g != Color.g || C.b != Color.b || C.a != Color.a)
            {
                return false;
            }
        }
    }
    return true;
}

bool Bitmap::PerfectMatch(const Bitmap &Bmp) const
{
    if(Bmp.Dimensions() != Dimensions())
    {
        return false;
    }
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            if ( (*this)[y][x].r != Bmp[y][x].r ||
                 (*this)[y][x].g != Bmp[y][x].g ||
                 (*this)[y][x].b != Bmp[y][x].b)
            {
                return false;
            }
        }
    }
    return true;
}

void Bitmap::FlipHorizontal()
{
    Bitmap BTemp = *this;
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            (*this)[y][x] = BTemp[y][_Width - 1 - x];
        }
    }
}

void Bitmap::FlipVertical()
{
    Bitmap BTemp = *this;
    for(UINT y = 0; y < _Height; y++)
    {
        for(UINT x = 0; x < _Width; x++)
        {
            (*this)[y][x] = BTemp[_Height - 1 - y][x];
        }
    }
}

#if 0
//
// The following is taken from
// http://www.suchit-tiwari.org/antialias.html
//
void Bitmap::DrawLine(int X0, int Y0, int X1, int Y1)
{
    const UINT NumLevels = 256;
    const UINT IntensityBits = 8;
    const UINT BaseColor = 0;

    unsigned short IntensityShift, ErrorAdj, ErrorAcc;
    unsigned short ErrorAccTemp, Weighting, WeightingComplementMask;
    short DeltaX, DeltaY, Temp, XDir;

    X0 = Utility::Bound(X0, 0, int(_Width) - 1);
    X1 = Utility::Bound(X1, 0, int(_Width) - 1);

    Y0 = Utility::Bound(Y0, 0, int(_Height) - 1);
    Y1 = Utility::Bound(Y1, 0, int(_Height) - 1);

    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        Temp = Y0; Y0 = Y1; Y1 = Temp;
        Temp = X0; X0 = X1; X1 = Temp;
    }

    /* Draw the initial pixel, which is always exactly intersected by
      the line and so needs no weighting */
    (*this)[Y0][X0] = RGBColor::Black;

    if ((DeltaX = X1 - X0) >= 0)
    {
      XDir = 1;
    }
    else
    {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
    }
    /* Special-case horizontal, vertical, and diagonal lines, which
      require no weighting because they go right through the center of
      every pixel */
    if ((DeltaY = Y1 - Y0) == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
            (*this)[Y0][X0] = RGBColor::Black;
        }
        return;
    }
    if (DeltaX == 0) {
        /* Vertical line */
        do
        {
            Y0++;
            (*this)[Y0][X0] = RGBColor::Black;
        } while (--DeltaY != 0);
        return;
    }
    if (DeltaX == DeltaY) {
      /* Diagonal line */
      do
      {
         X0 += XDir;
         Y0++;
         (*this)[Y0][X0] = RGBColor::Black;
      } while (--DeltaY != 0);
      return;
    }
    /* Line is not horizontal, diagonal, or vertical */
    ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
    /* # of bits by which to shift ErrorAcc to get intensity level */
    IntensityShift = 16 - IntensityBits;
    /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
    WeightingComplementMask = NumLevels - 1;
    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
      ErrorAdj = unsigned short(((unsigned long) DeltaX << 16) / (unsigned long) DeltaY);
      /* Draw all pixels other than the first and last */
      while (--DeltaY) {
         ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
         ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
         if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the X coord */
            X0 += XDir;
         }
         Y0++; /* Y-major, so always advance Y */
         /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         
         unsigned char LVal = (BaseColor + Weighting);
         (*this)[Y0][X0] = RGBColor(LVal, LVal, LVal);

         LVal = BaseColor + (Weighting ^ WeightingComplementMask);
         (*this)[Y0][X0 + XDir] = RGBColor(LVal, LVal, LVal);
      }
      /* Draw the final pixel, which is always exactly intersected by the line
         and so needs no weighting */
      (*this)[Y1][X1] = RGBColor::Black;
      return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
      pixel that Y advances each time X advances 1 pixel, truncating the
      result to avoid overrunning the endpoint along the X axis */
    ErrorAdj = unsigned short(((unsigned long) DeltaY << 16) / (unsigned long) DeltaX);
    /* Draw all pixels other than the first and last */
    while (--DeltaX) {
      ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
      ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
      if (ErrorAcc <= ErrorAccTemp) {
         /* The error accumulator turned over, so advance the Y coord */
         Y0++;
      }
      X0 += XDir; /* X-major, so always advance X */
      /* The IntensityBits most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      
      unsigned char LVal = (BaseColor + Weighting);
      (*this)[Y0][X0] = RGBColor(LVal, LVal, LVal);

      LVal = BaseColor + (Weighting ^ WeightingComplementMask);
      (*this)[Y0 + 1][X0] = RGBColor(LVal, LVal, LVal);
    }
    /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
    (*this)[Y1][X1] = RGBColor::Black;
}
#endif

#ifdef USE_D3D9
void Bitmap::LoadFromSurface(LPDIRECT3DSURFACE9 Surface)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT LockedRect;
    D3DAlwaysValidate(Surface->GetDesc(&Desc), "GetDesc");
    PersistentAssert(Desc.Format == D3DFMT_A8R8G8B8 ||
                     Desc.Format == D3DFMT_X8R8G8B8, "Invalid surface format");
    Allocate(Desc.Width, Desc.Height);
    D3DAlwaysValidate(Surface->LockRect(&LockedRect, NULL, 0), "LockRect");
    BYTE *_Data = (BYTE *)LockedRect.pBits;
    if(Desc.Format == D3DFMT_A8R8G8B8 || Desc.Format == D3DFMT_X8R8G8B8)
    {
        for(UINT y = 0; y < _Height; y++)
        {
            RGBColor *RowStart = (RGBColor *)(_Data + y * LockedRect.Pitch);
            memcpy((*this)[_Height - 1 - y], RowStart, sizeof(RGBColor) * _Width);
            //for(UINT x = 0; x < _Width; x++)
            //{
            //    (*this)[y][x] = RowStart[x];
            //}
        }
    }
    D3DAlwaysValidate(Surface->UnlockRect(), "UnlockRect");
}

#ifdef USE_PNG
void Bitmap::SaveSurfaceToPNG(LPDIRECT3DSURFACE9 Surface, const String &Filename, const BitmapSaveOptions &Options)
{
    D3DSURFACE_DESC Desc;
    D3DLOCKED_RECT LockedRect;
    D3DAlwaysValidate(Surface->GetDesc(&Desc), "GetDesc");
    PersistentAssert(Desc.Format == D3DFMT_A8R8G8B8 ||
                     Desc.Format == D3DFMT_X8R8G8B8, "Invalid surface format");
    D3DAlwaysValidate(Surface->LockRect(&LockedRect, NULL, 0), "LockRect");
	const UINT Width = Desc.Width;
	const UINT Height = Desc.Height;
    BYTE *_Data = (BYTE *)LockedRect.pBits;
    /*for(UINT y = 0; y < Height; y++)
    {
        RGBColor *RowStart = (RGBColor *)(_Data + y * LockedRect.Pitch);
        memcpy((*this)[Height - 1 - y], RowStart, sizeof(RGBColor) * Width);
        //for(UINT x = 0; x < _Width; x++)
        //{
        //    (*this)[y][x] = RowStart[x];
        //}
    }*/

    FILE *File;
    File = fopen(Filename.CString(), "wb");
    PersistentAssert(File != NULL, "File open for SavePNG failed");

    png_structp PngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    PersistentAssert(PngWrite != NULL, "png_create_write_struct failed");

    png_infop PngInfo = png_create_info_struct(PngWrite);
    PersistentAssert(PngInfo != NULL, "png_create_info_struct failed");

    png_init_io(PngWrite, File);

    png_set_IHDR(PngWrite, PngInfo, Width, Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
	png_set_PLTE(PngWrite, PngInfo, NULL, 0);
    png_set_gAMA(PngWrite, PngInfo, 0.0);
    
    png_color_8 ColorInfo;

    ColorInfo.alpha = 0;
    UINT BytesPerPixel = 3;
    if(Options.SaveAlpha)
    {
        ColorInfo.alpha = 8;
        BytesPerPixel = 4;
    }
    ColorInfo.red = 8;
    ColorInfo.green = 8;
    ColorInfo.blue = 8;
    ColorInfo.gray = 0;
    png_set_sBIT(PngWrite, PngInfo, &ColorInfo);
    
    
    UINT ScratchSizeNeeded = sizeof(png_bytep) * Height;
    BYTE *ScratchSpace = Options.ScratchSpace;
    BYTE *ScratchSpaceStart = NULL;
    if(Options.ScratchSpaceSize < ScratchSizeNeeded)
    {
        ScratchSpace = new BYTE[ScratchSizeNeeded];
        ScratchSpaceStart = ScratchSpace;
    }

    png_bytep *RowPointers = (png_bytep *)ScratchSpace;
    for(UINT y = 0; y < Height; y++)
    {
        RowPointers[y] = (_Data + y * LockedRect.Pitch);
    }

    png_set_rows(PngWrite, PngInfo, RowPointers);
    png_write_png(PngWrite, PngInfo, NULL, NULL);

	png_destroy_write_struct(&PngWrite, &PngInfo);

    if(ScratchSpaceStart != NULL)
    {
        delete[] ScratchSpaceStart;
    }

	D3DAlwaysValidate(Surface->UnlockRect(), "UnlockRect");
    fclose(File);
}
#endif

#endif

Bitmap operator + (const Bitmap &L, const Bitmap &R)
{
    Assert(L.Width() == R.Width() && L.Height() == R.Height(), "Invalid dimensions in Bitmap operator +");
    Bitmap Result = L;
    for(UINT y = 0; y < Result.Height(); y++)
    {
        for(UINT x = 0; x < Result.Width(); x++)
        {
            Result[y][x] += R[y][x];
        }
    }
    return Result;
}

OutputDataStream& operator << (OutputDataStream &stream, const Bitmap &bmp)
{
    stream << bmp.Width() << bmp.Height();
    stream.WriteData((BYTE *)bmp.Pixels(), bmp.PixelCount() * sizeof(RGBColor));
    return stream;
}

InputDataStream& operator >> (InputDataStream &stream, Bitmap &bmp)
{
    UINT width, height;
    stream >> width >> height;
    bmp.Allocate(width, height);
    stream.ReadData((BYTE *)bmp.Pixels(), bmp.PixelCount() * sizeof(RGBColor));
    return stream;
}
