/*
InputManager.h
Written by Matthew Fisher

InputManager stores and controls access to keyboard and mouse data.
*/

const UINT MaxKeyCount = 256;

enum MouseButtonType
{
    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonMiddle,
    MouseButtonCount
};

enum InputEventType
{
    InputEventMenu,
    InputEventToolbar0,
    InputEventCount
};

struct InputEvent
{
    InputEvent() {}
    InputEvent(InputEventType _Type, UINT _Id)
    {
        Type = _Type;
        Id = _Id;
    }
    InputEventType Type;
    UINT Id;
};

struct MouseState
{
    Vec2i Pos;
    int Wheel;
    bool Buttons[MouseButtonCount];
};

class InputManager
{
public:
    InputManager();
    void Frame();

    //
    // Keyboard Modify
    //
    __forceinline bool* KeyArray()
    {
        return _KeyPressed;
    }
    __forceinline void SetKeyState(UINT Key, bool State)
    {
        Assert(Key < MaxKeyCount, "Invalid key");
        _KeyPressed[Key] = State;
    }

    //
    // Keyboard Query
    //
    __forceinline bool KeyCheck(UINT Key)
    {
        Assert(Key < MaxKeyCount, "Invalid key");
        if(GetAsyncKeyState(Key) == 0)
        {
            _KeyPressed[Key] = false;
        }
        return _KeyPressed[Key];
    }
    __forceinline bool KeyCheckOnce(UINT Key)
    {
        Assert(Key < MaxKeyCount, "Invalid key");
        bool Ret = _KeyPressed[Key];
        _KeyPressed[Key] = false;
        return Ret;
    }

    //
    // Mouse Modify
    //
    __forceinline void SetMouseState(MouseButtonType Button, bool State)
    {
        Assert(Button < MouseButtonCount, "Invalid mouse button");
        _Mouse.Buttons[Button] = State;
    }
    __forceinline void UpdateMousePos(const Vec2i &NewPos)
    {
        _Mouse.Pos = NewPos;
    }
    __forceinline void UpdateWheelState(int WheelDelta)
    {
        _Mouse.Wheel += WheelDelta;
    }

    //
    // Mouse Query
    //
    __forceinline const MouseState& Mouse() const
    {
        return _Mouse;
    }
    __forceinline const MouseState& MouseDiff() const
    {
        return _MouseDiff;
    }

    //
    // Events
    //
    __forceinline bool EventPresent() const
    {
        return (_Events.Length() > 0);
    }
    __forceinline InputEvent PopEvent()
    {
        InputEvent Result = _Events.First();
        _Events.PopFront();
        return Result;
    }
    __forceinline void PushEvent(const InputEvent &Event)
    {
        _Events.PushEnd(Event);
    }

private:
    Vector<InputEvent> _Events;
    
    bool    _KeyPressed[MaxKeyCount];

    MouseState _Mouse;
    MouseState _PrevMouse;
    MouseState _MouseDiff;
};
