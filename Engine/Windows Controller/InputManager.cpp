/*
InputManager.cpp
Written by Matthew Fisher

InputManager stores and controls access to keyboard and mouse data.
*/

InputManager::InputManager()
{
    for(UINT KeyIndex = 0; KeyIndex < MaxKeyCount; KeyIndex++)
    {
        _KeyPressed[KeyIndex] = false;
    }
    for(UINT ButtonIndex = 0; ButtonIndex < MouseButtonCount; ButtonIndex++)
    {
        _Mouse.Buttons[ButtonIndex] = false;
    }

    _Mouse.Pos = Vec2i::Origin;
    _Mouse.Wheel = 0;
}

void InputManager::Frame()
{
    _MouseDiff.Pos = _Mouse.Pos - _PrevMouse.Pos;
    _MouseDiff.Wheel = _Mouse.Wheel - _PrevMouse.Wheel;
    _PrevMouse = _Mouse;
}
