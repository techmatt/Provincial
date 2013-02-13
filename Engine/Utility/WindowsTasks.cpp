String WindowsUtility::DescribeLastWindowsError()
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, eNum,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					sysMsg, 256, NULL );

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while( ( *p > 31 ) || ( *p == 9 ) )
	++p;
	do { *p-- = 0; } while( ( p >= sysMsg ) &&
	    ( ( *p == '.' ) || ( *p < 33 ) ) );

	return String(sysMsg);
}

WindowsProcess::WindowsProcess(const PROCESSENTRY32 &entry)
{
    _handle = NULL;
    _id = entry.th32ProcessID;
    _name = entry.szExeFile;
}

void WindowsProcess::Suspend()
{
    SignalError("Not implemented");
    if(_handle == NULL)
    {
        //_handle = OpenThread(THREAD_ALL_ACCESS, FALSE, 
    }
}

void ProcessList::FreeMemory()
{
    _list.DeleteMemory();
}

WindowsProcess* ProcessList::GetProcessByName(const String &name) const
{
    for(UINT processIndex = 0; processIndex < _list.Length(); processIndex++)
    {
        WindowsProcess &curProcess = *_list[processIndex];
        if(curProcess.name() == name)
        {
            return &curProcess;
        }
    }
    return NULL;
}

void ProcessList::Update()
{
    FreeMemory();
    
    HANDLE processSnapshot;
    PROCESSENTRY32 processEntry;

    // Take a snapshot of all processes in the system.
    processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(processSnapshot == INVALID_HANDLE_VALUE)
    {
        SignalError("CreateToolhelp32Snapshot");
    }

    // Set the size of the structure before using it.
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(processSnapshot, &processEntry))
    {
        SignalError("Process32First");
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
        WindowsProcess *curProcess = new WindowsProcess(processEntry);
        _list.PushEnd(curProcess);
    } while(Process32Next(processSnapshot, &processEntry));

    CloseHandle(processSnapshot);
}

void ProcessList::Describe() const
{
    Console::WriteLine("Process List");
    for(UINT processIndex = 0; processIndex < _list.Length(); processIndex++)
    {
        const WindowsProcess &curProcess = *_list[processIndex];
        Console::WriteLine(String(curProcess.id()) + String(": ") + curProcess.name());
    }
}

bool IsShellWindow(HWND window)
{
    if(!IsWindow(window) || !IsWindowVisible(window))
    {
        return false;
    }
    if(GetAncestor(window, GA_PARENT) != GetDesktopWindow())
    {
        return false;
    }

    RECT clientRect;
    GetClientRect(window, &clientRect);
    if(clientRect.right - clientRect.left <= 1 || clientRect.bottom - clientRect.top <= 1)
    {
        return false;
    }
    
    char name[256] = {'\0'};
    GetWindowText(window, name, 256);
    String sName = name;
    if(sName.Length() == 0 || sName == "Start")
    {
        return false;
    }

    return true;
}

BOOL WINAPI EnumWindowProc(HWND window, LPARAM context)
{
    ((WindowList *)context)->RecordWindow(window);
    return TRUE;
}

LRESULT CALLBACK WindowsWindowMonitor(int nCode, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT *data = (CWPSTRUCT *)lParam;
    Console::WriteLine("Message: " + String(data->message));
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void WindowsWindow::Monitor()
{
    HHOOK hookHandle = SetWindowsHookEx(WH_CALLWNDPROC, WindowsWindowMonitor, NULL, GetCurrentThreadId());
}

WindowsWindow::WindowsWindow(HWND handle)
{
    _threadHandle = NULL;
    _windowHandle = handle;

    char name[256] = {'\0'};
    GetWindowText(handle, name, 255);
    _name = name;
}

void WindowsWindow::SuspendWindowThread()
{
    if(_threadHandle == NULL)
    {
        UINT threadID = GetWindowThreadProcessId(_windowHandle, NULL);
        _threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadID);
    }
    SuspendThread(_threadHandle);
}

void WindowsWindow::ResumeWindowThread()
{
    ResumeThread(_threadHandle);
}

void WindowsWindow::MoveMouse(const Vec2i &coords)
{
    SignalError("Not implemented");
    Console::WriteLine("Moving mouse to " + coords.CommaSeparatedString());
    
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);

    POINT screenPoint;
    screenPoint.x = coords.x;
    screenPoint.y = coords.y;
    ClientToScreen(_windowHandle, &screenPoint);

    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, screenPoint.x, screenPoint.y, 0, NULL);
}

void WindowsWindow::MoveClick(bool left, bool down)
{
    SignalError("Not implemented");
}

void WindowsWindow::KeyPress(UINT keyCode)
{
    SendMessage(_windowHandle, WM_KEYDOWN, keyCode, 0);
    SendMessage(_windowHandle, WM_KEYUP, keyCode, 0);
}

void WindowList::FreeMemory()
{
    _list.DeleteMemory();
}

void WindowList::Update(UINT processIDFilter)
{
    _processIDFilter = processIDFilter;
    FreeMemory();
    EnumChildWindows(GetDesktopWindow(), EnumWindowProc, (LPARAM)this);
}

void WindowList::Describe() const
{
    Console::WriteLine("Window List");
    for(UINT processIndex = 0; processIndex < _list.Length(); processIndex++)
    {
        const WindowsWindow &curWindow = *_list[processIndex];
        Console::WriteLine(curWindow.dimensions().CommaSeparatedString() + " " + curWindow.name());
    }
}

void WindowList::RecordWindow(HWND window)
{
    if(IsShellWindow(window))
    {
        if(_processIDFilter != 0)
        {
            DWORD windowsProcessID;
            GetWindowThreadProcessId(window, &windowsProcessID);
            if(windowsProcessID != _processIDFilter)
            {
                return;
            }
        }
        _list.PushEnd(new WindowsWindow(window));
    }
}

//http://www.flounder.com/screencapture.htm

void WindowCapture::Init(HWND window)
{
    _windowDC = GetDC(window);
    _bmpDC = CreateCompatibleDC(_windowDC);

    RECT rect;
    GetClientRect(window, &rect);

    const UINT width = rect.right - rect.left;
    const UINT height = rect.bottom - rect.top;

    _bmp.Allocate(width, height);

    _bitmap = CreateCompatibleBitmap(_windowDC, width, height);

    _header.bcSize = sizeof(BITMAPCOREHEADER);
    _header.bcBitCount = 0;
    GetDIBits(_bmpDC, _bitmap, 0, 0, NULL, (BITMAPINFO*)&_header, DIB_RGB_COLORS);
    Assert(width == _header.bcWidth && height == _header.bcHeight && _header.bcBitCount == 24, "Invalid bitmap parameters");
}

void WindowCapture::Capture()
{
    const UINT width = _bmp.Width();
    const UINT height = _bmp.Height();

    HGDIOBJ startBitmap = SelectObject(_bmpDC, _bitmap);
    BitBlt(_bmpDC, 0, 0, width, height, _windowDC, 0, 0, SRCCOPY);
    SelectObject(_bmpDC, startBitmap);

    UINT pitch = width * 3;
    while((pitch & 3) != 0)
    {
        pitch++;
    }

    const UINT imageSize = height * pitch * 3;
    if(_storage.Length() < imageSize)
    {
        _storage.Allocate(imageSize);
    }
    GetDIBits(_bmpDC, _bitmap, 0, height, (LPVOID)_storage.CArray(), (BITMAPINFO*)&_header, DIB_RGB_COLORS);

    for(UINT y = 0; y < height; y++)
    {
        const BYTE *storageStart = _storage.CArray() + pitch * y;
        RGBColor* bmpStart = _bmp[y];
        for(UINT x = 0; x < width; x++)
        {
            bmpStart[x] = RGBColor(storageStart[x * 3 + 0], storageStart[x * 3 + 1], storageStart[x * 3 + 2], 0);
        }
    }
}
