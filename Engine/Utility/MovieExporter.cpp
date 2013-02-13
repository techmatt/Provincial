/*
MovieExporter.cpp
Written by Matthew Fisher

MovieExporter manages screen capture and exports as a sequence of images for compilation into a movie.
*/

void MovieExporter::Init(const String &BaseFilename, const String &ImageListFilename, const String &MaskFilename)
{
    if(MaskFilename == NULL)
    {
        _Mask.FreeMemory();
    }
    else
    {
        _Mask.LoadPNG(MaskFilename);
    }
    _ImageList.open(ImageListFilename.CString());
    _BaseFilename = BaseFilename;
    _CurImageIndex = 0;
}

void MovieExporter::ExportScreen(GraphicsDevice &GD, ApplicationWindow &WM)
{
    if(_CurImageIndex == 0)
    {
        _CurImageIndex = 1;
        return;
    }

    String CurFilename = _BaseFilename + String(_CurImageIndex) + String(".png");

    Bitmap Bmp;
    GD.CaptureScreen(WM, Bmp);

    Assert(_Mask.Width() == 0 || (Bmp.Width() == _Mask.Width() && Bmp.Height() == _Mask.Height()), "Invalid Mask Dimensions");

    for(UINT y = 0; y < _Mask.Height(); y++)
    {
        for(UINT x = 0; x < _Mask.Width(); x++)
        {
            if(_Mask[y][x] != RGBColor::Magenta)
            {
                Bmp[y][x] = _Mask[y][x];
            }
        }
    }

    Bmp.SavePNG(CurFilename);

    _ImageList << CurFilename << endl;

    _CurImageIndex++;
}