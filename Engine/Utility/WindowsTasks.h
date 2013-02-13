#include <Tlhelp32.h>

namespace WindowsUtility
{
    String DescribeLastWindowsError();
};

class WindowsProcess
{
public:
    WindowsProcess(const PROCESSENTRY32 &entry);

    void Suspend();

    __forceinline const String& name() const
    {
        return _name;
    }
    __forceinline DWORD id() const
    {
        return _id;
    }

private:
    HANDLE _handle;
    String _name;
    DWORD _id;
};

class ProcessList
{
public:
    void FreeMemory();
    void Update();
    void Describe() const;
    
    WindowsProcess* GetProcessByName(const String &name) const;

    __forceinline const Vector<WindowsProcess*>& list() const
    {
        return _list;
    }

private:
    Vector<WindowsProcess*> _list;
};

class WindowsWindow
{
public:
    WindowsWindow(HWND handle);

    void SuspendWindowThread();
    void ResumeWindowThread();

    void MoveMouse(const Vec2i &coords);
    void MoveClick(bool left, bool down);
    void KeyPress(UINT keyCode);

    void Monitor();

    Vec2i dimensions() const
    {
        RECT clientRect;
        GetClientRect(_windowHandle, &clientRect);
        return Vec2i(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
    }
    __forceinline const String& name() const
    {
        return _name;
    }
    __forceinline HWND handle() const
    {
        return _windowHandle;
    }

private:
    Vec2i _mouseCoords;
    HANDLE _threadHandle;
    HWND _windowHandle;
    String _name;
};

class WindowList
{
public:
    void FreeMemory();
    void Update(UINT processIDFilter);
    void Describe() const;

    const Vector<WindowsWindow*>& list() const
    {
        return _list;
    }

    void RecordWindow(HWND window);

private:
    Vector<WindowsWindow*> _list;
    UINT _processIDFilter;
};

class WindowCapture
{
public:
    void Init(HWND window);
    void Capture();

    __forceinline const Bitmap& bmp()
    {
        return _bmp;
    }

private:
    HDC _windowDC;
    HDC _bmpDC;
    HBITMAP _bitmap;
    HWND _handle;
    BITMAPCOREHEADER _header;
    Vector<BYTE> _storage;
    Bitmap _bmp;
};
