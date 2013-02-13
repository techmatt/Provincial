/*
ApplicationWindow.h
Written by Matthew Fisher

WindowManager handles creating and updating the application's main window.
*/

class ApplicationWindow
{
public:
    ApplicationWindow();

    void FreeMemory();
    void FreeWindow();
    
    void Init(HWND handle);
    void InitAll(HINSTANCE hInstance, int nCmdShow, bool FullScreen, int ScreenWidth, int ScreenHeight, const String &WindowName);
    void InitWindow(HINSTANCE hInstance, int nCmdShow, bool FullScreen, int ScreenWidth, int ScreenHeight, const String &WindowName);
    void ReSize(int NewClientAreaWidth, int NewClientAreaHeight);
    Vec2f GetBorderDimensions();

    void ResetWindow(bool FullScreen, int nCmdShow, int Width, int Height);

    void RenameWindow(const String &NewName);

    Vec2f MapAbsoluteWindowCoordToRelative(const Vec2f &Absolute);
    Vec2f MapRelativeWindowCoordToAbsolute(const Vec2f &Relative);

    //
    // Query
    //
    UINT Width();
    UINT Height();
    Vec2i Dimensions();
    float AspectRatio();
    
    //
    // Accessors
    //
    __forceinline HWND Handle()
    {
        return _handle;
    }
    bool FullScreen()
    {
        return _fullScreen;
    }
    __forceinline double& FixedAspectRatio()
    {
        return _fixedAspectRatio;
    }

private:
    String   _className;
    WNDCLASS _class;
    HWND     _handle;
    bool     _fullScreen;
    double   _fixedAspectRatio;
};
