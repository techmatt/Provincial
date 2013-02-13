MenuShortcut::MenuShortcut()
{
    _Key = 0;
}

MenuShortcut::MenuShortcut(const String &S)
{
    _Shift = S.StartsWith("Shift+");
    _Ctrl = S.StartsWith("Ctrl+");
    _Alt = S.StartsWith("Alt+");

    Vector<String> Result;
    S.Partition('+', Result);
    String StringKey = Result.Last();
    if(StringKey.Length() == 1 && StringKey[0] >= 'A' && StringKey[0] <= 'Z')
    {
        _Key = KEY_A + UINT(StringKey[0]) - UINT('A');
    }
    else if(StringKey.Length() == 1 && StringKey[0] >= '0' && StringKey[0] <= '9')
    {
        _Key = KEY_0 + UINT(StringKey[0]) - UINT('0');
    }
    else if(StringKey == "Enter")
    {
        _Key = KEY_ENTER;
    }
    else if(StringKey == "End")
    {
        _Key = KEY_END;
    }
    else if(StringKey == "Pause")
    {
        _Key = KEY_PAUSE;
    }
    else if(StringKey == "Left")
    {
        _Key = KEY_LEFT;
    }
    else if(StringKey == "Right")
    {
        _Key = KEY_RIGHT;
    }
    else if(StringKey == "Up")
    {
        _Key = KEY_UP;
    }
    else if(StringKey == "Down")
    {
        _Key = KEY_DOWN;
    }
    else
    {
        SignalError(String("Invalid Menu Shortcut: ") + S);
    }
}

void MenuShortcut::Dispatch(InputManager &I, UINT Id)
{
    if(_Key != 0)
    {
        if( (!_Shift || I.KeyCheck(KEY_SHIFT)) &&
            (!_Ctrl || I.KeyCheck(KEY_CTRL)) )
        {
            if(I.KeyCheckOnce(_Key))
            {
                I.PushEvent(InputEvent(InputEventMenu, Id));
            }
        }
    }
}

MenuEntry::MenuEntry()
{
    _SubMenu = NULL;
}

MenuEntry::MenuEntry(Menu *SubMenu, const String &Name, UINT Id)
{
    _SubMenu = SubMenu;
    _Name = Name;
    _Id = Id;

    Vector<String> Partition;
    Name.Partition('\t', Partition);
    if(Partition.Length() == 2)
    {
        _Shortcut = MenuShortcut(Partition.Last());
    }
}

MenuEntry::~MenuEntry()
{
    FreeMemory();
}

void MenuEntry::FreeMemory()
{
    /*if(_SubMenu)
    {
        _SubMenu->FreeMemory();
        delete _SubMenu;
        _SubMenu = NULL;
    }
    _Name.FreeMemory();*/
}

void MenuEntry::DispatchKeyboardShortcuts(InputManager &I)
{
    if(_SubMenu != NULL)
    {
        _SubMenu->DispatchKeyboardShortcuts(I);
    }
    else
    {
        _Shortcut.Dispatch(I, _Id);
    }
}

Menu::Menu()
{
    _Handle = NULL;
}

Menu::Menu(HMENU Handle)
{
    _Handle = Handle;
}

Menu::~Menu()
{
    FreeMemory();
}

void Menu::FreeMemory()
{
    for(UINT Index = 0; Index < _Entries.Length(); Index++)
    {
        _Entries[Index].FreeMemory();
    }
    _Entries.FreeMemory();
    if(_Handle != NULL)
    {
        DestroyMenu(_Handle);
        _Handle = NULL;
    }
}

Menu& Menu::AddSubMenu(ApplicationWindow &WM, UINT Id, const String &Name)
{
    HMENU NewHandle = CreatePopupMenu();
    Assert(NewHandle != NULL, "CreatePopupMenu failed");

    _Entries.PushEnd(MenuEntry(new Menu(NewHandle), Name, Id));

    BOOL Success = AppendMenu(_Handle, MF_POPUP, UINT_PTR(NewHandle), Name.CString());
    Assert(Success != 0, "AppendMenu failed");

    Success = DrawMenuBar(WM.Handle());
    Assert(Success != 0, "DrawMenuBar failed");

    return _Entries.Last().SubMenu();
}

void Menu::AddSeperator(ApplicationWindow &WM, UINT Id)
{
    BOOL Success = AppendMenu(_Handle, MF_SEPARATOR, Id, NULL);
    Assert(Success != 0, "SetMenu failed");

    Success = DrawMenuBar(WM.Handle());
    Assert(Success != 0, "DrawMenuBar failed");
}

void Menu::AddString(ApplicationWindow &WM, UINT Id, const String &Name)
{
    BOOL Success = AppendMenu(_Handle, MF_STRING, Id, Name.CString());
    Assert(Success != 0, "SetMenu failed");

    Success = DrawMenuBar(WM.Handle());
    Assert(Success != 0, "DrawMenuBar failed");

    _Entries.PushEnd(MenuEntry(NULL, Name, Id));
}

void Menu::SetItemName(ApplicationWindow &WM, UINT Id, const String &Name)
{
    String NameCopy = Name;
    MENUITEMINFO Info;
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STRING;
    Info.dwTypeData = NameCopy.CString();

    BOOL Success = SetMenuItemInfo(_Handle, Id, FALSE, &Info);
    Assert(Success != 0, "SetMenuItemInfo failed");
}

void Menu::SetItemState(ApplicationWindow &WM, UINT Id, bool Enabled, bool Checked)
{
    MENUITEMINFO Info;
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STATE;

    Info.fState = 0;
    
    if(Enabled)
    {
        Info.fState |= MFS_ENABLED;
    }
    else
    {
        Info.fState |= MFS_DISABLED;
    }

    if(Checked)
    {
        Info.fState |= MFS_CHECKED;
    }
    else
    {
        
    }

    BOOL Success = SetMenuItemInfo(_Handle, Id, FALSE, &Info);
    Assert(Success != 0, "SetMenuItemInfo failed");
    
    //Success = DrawMenuBar(WM.GetHWND());
    //Assert(Success != 0, "DrawMenuBar failed");
}

bool Menu::HasSubMenu(const String &Name)
{
    for(UINT Index = 0; Index < _Entries.Length(); Index++)
    {
        MenuEntry &CurEntry = _Entries[Index];
        if(CurEntry.Name() == Name)
        {
            return true;
        }
    }
    return false;
}

Menu& Menu::GetSubMenu(const String &Name)
{
    for(UINT Index = 0; Index < _Entries.Length(); Index++)
    {
        MenuEntry &CurEntry = _Entries[Index];
        if(CurEntry.Name() == Name)
        {
            return CurEntry.SubMenu();
        }
    }
    SignalError("Sub menu not found");
    return _Entries[0].SubMenu();
}

void Menu::DispatchKeyboardShortcuts(InputManager &I)
{
    for(UINT Index = 0; Index < _Entries.Length(); Index++)
    {
        MenuEntry &CurEntry = _Entries[Index];
        CurEntry.DispatchKeyboardShortcuts(I);
    }
}

void MenuInterface::Init(ApplicationWindow &WM)
{
    HMENU MenuBarHandle = CreateMenu();
    Assert(MenuBarHandle != NULL, "CreateMenu failed");

    BOOL Success = SetMenu(WM.Handle(), MenuBarHandle);
    Assert(Success != 0, "SetMenu failed");

    Success = DrawMenuBar(WM.Handle());
    Assert(Success != 0, "DrawMenuBar failed");

    _MenuBar = new Menu(MenuBarHandle);
}

void MenuInterface::DispatchKeyboardShortcuts(InputManager &I)
{
    if(_MenuBar != NULL)
    {
        _MenuBar->DispatchKeyboardShortcuts(I);
    }
}
