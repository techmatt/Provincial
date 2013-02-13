/*
WindowManager.cpp
Written by Matthew Fisher

WindowManager handles creating and updating the application's main window.
*/

ApplicationWindow::ApplicationWindow()
{
    _handle = NULL;
    _fullScreen = false;
}

void ApplicationWindow::FreeMemory()
{
    FreeWindow();
    UnregisterClass( _className.CString(), _class.hInstance );
}

void ApplicationWindow::FreeWindow()
{
    DestroyWindow(_handle);
}

void ApplicationWindow::Init(HWND handle)
{
    _handle = handle;
}

void ApplicationWindow::InitAll(HINSTANCE hInstance, int nCmdShow, bool _fullScreen, int _ScreenWidth, int _ScreenHeight, const String &WindowName)
{
    _fixedAspectRatio = 0.0;

    _className = WindowName;
    //_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    _class.style = CS_HREDRAW | CS_VREDRAW;
    _class.lpfnWndProc = (WNDPROC) WndProc; 
    _class.cbClsExtra = 0; 
    _class.cbWndExtra = 0; 
    _class.hInstance = hInstance; 
    _class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    _class.hCursor = LoadCursor(NULL, IDC_ARROW);
    _class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
    _class.lpszMenuName =  NULL;
    _class.lpszClassName = _className.CString();           //window class paramaters
    RegisterClass(&_class);                                //register the class with windows

    InitWindow(hInstance, nCmdShow, _fullScreen, _ScreenWidth, _ScreenHeight, WindowName);    //initalize the window
}

void ApplicationWindow::InitWindow(HINSTANCE hInstance, int nCmdShow, bool FullScreen, int ScreenWidth, int ScreenHeight, const String &WindowName)
{
    _fullScreen = FullScreen;

    DWORD dwStyle;
    if(FullScreen)
    {
        dwStyle = WS_POPUP;
    }
    else
    {
        //dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
        dwStyle = WS_OVERLAPPEDWINDOW;

        //dwStyle = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
        //dwStyle = WS_DLGFRAME | WS_VISIBLE;
    }

    _handle = CreateWindow(
        WindowName.CString(), 
        WindowName.CString(), 
        dwStyle, 
        0, //CW_USEDEFAULT
        0, //CW_USEDEFAULT
        ScreenWidth, 
        ScreenHeight, 
        (HWND) NULL, 
        (HMENU) NULL, 
        hInstance,
        (LPVOID) NULL); //create the window with the appropriate paramaters
                        //see Win32 documentation for a description of the CreateWindow function
    PersistentAssert(_handle != NULL, "CreateWindow failed");

    ShowWindow(_handle, nCmdShow);
    UpdateWindow(_handle);            //show and update the window
}

void ApplicationWindow::RenameWindow(const String &NewName)
{
    SetWindowText(_handle, NewName.CString());
}

Vec2f ApplicationWindow::GetBorderDimensions()
{
    RECT ClientRect, ScreenRect;
    GetClientRect(_handle, &ClientRect);
    GetWindowRect(_handle, &ScreenRect);
    int BorderWidth = ScreenRect.right - ScreenRect.left - ClientRect.right;
    int BorderHeight = ScreenRect.bottom - ScreenRect.top - ClientRect.bottom;
    return Vec2f(float(BorderWidth), float(BorderHeight));
}

void ApplicationWindow::ReSize(int NewWindowWidth, int NewWindowHeight)
{
    RECT ClientRect, ScreenRect;
    GetClientRect(_handle, &ClientRect);
    GetWindowRect(_handle, &ScreenRect);
    int BorderWidth = ScreenRect.right - ScreenRect.left - ClientRect.right;
    int BorderHeight = ScreenRect.bottom - ScreenRect.top - ClientRect.bottom;
    MoveWindow(_handle, ScreenRect.left, ScreenRect.top, NewWindowWidth + BorderWidth, NewWindowHeight + BorderHeight, TRUE);
}

void ApplicationWindow::ResetWindow(bool FullScreen, int nCmdShow, int Width, int Height)
{
    _fullScreen = FullScreen;

    if( FullScreen )
    {
        SetWindowLong( _handle, GWL_STYLE, WS_POPUP );
    }
    else
    {
        Width--;
        Height--;
        SetWindowLong( _handle, GWL_STYLE, WS_OVERLAPPEDWINDOW );    //handle the difference b/w Full screen and not full screen mode
    }

    //SetWindowPos resets the window
    SetWindowPos(_handle, HWND_TOP, 0, 0, Width, Height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_SHOWWINDOW);

    
    //ShowWindow(_handle, nCmdShow);
    //UpdateWindow(_handle);            //show and update the window    
}

UINT ApplicationWindow::Width()
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    return Rect.right - Rect.left;
}

UINT ApplicationWindow::Height()
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    return Rect.bottom - Rect.top;
}

Vec2i ApplicationWindow::Dimensions()
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    return Vec2i(Rect.right - Rect.left, Rect.bottom - Rect.top);
}

float ApplicationWindow::AspectRatio()
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    return float(Rect.right - Rect.left) / float(Rect.bottom - Rect.top);
}

Vec2f ApplicationWindow::MapAbsoluteWindowCoordToRelative(const Vec2f &Absolute)
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    UINT Width = Rect.right - Rect.left;
    UINT Height = Rect.bottom - Rect.top;
    Vec2f Result(Math::LinearMap(0.0f, float(Width), 0.0f, 1.0f, Absolute.x),
                 Math::LinearMap(0.0f, float(Height), 0.0f, 1.0f, Absolute.y));
    return Result;
}

Vec2f ApplicationWindow::MapRelativeWindowCoordToAbsolute(const Vec2f &Relative)
{
    RECT Rect;
    GetClientRect(_handle, &Rect);
    UINT Width = Rect.right - Rect.left;
    UINT Height = Rect.bottom - Rect.top;
    Vec2f Result(Math::LinearMap(0.0f, 1.0f, 0.0f, float(Width), Relative.x),
                Math::LinearMap(0.0f, 1.0f, 0.0f, float(Height), Relative.y));
    return Result;
}
