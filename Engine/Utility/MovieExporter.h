/*
MovieExporter.h
Written by Matthew Fisher

MovieExporter manages screen capture and exports as a sequence of images for compilation into a movie.
*/

class MovieExporter
{
public:
    void Init(const String &BaseFilename, const String &ImageListFilename, const String &MaskFilename);
    void ExportScreen(GraphicsDevice &GD, ApplicationWindow &Window);

private:
    Bitmap _Mask;
    UINT _CurImageIndex;
    String _BaseFilename;
    ofstream _ImageList;
};
