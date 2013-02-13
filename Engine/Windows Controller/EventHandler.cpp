/*
EventHandler.cpp
Origionally from NeHe, http://nehe.gamedev.net/
Modified by Matthew Fisher

Creating a new window involves having a callback function, WndProc, that is called whenever Windows has
an event to send to your window/application.
*/

InputManager *g_WndProcContext = NULL;

LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if(g_WndProcContext == NULL)
    {
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
    switch( msg )
    {
    case WM_SYSCOMMAND:                  // Intercept System Commands
        {
            switch (wParam)              // Check System Calls
            {
                case SC_SCREENSAVE:      // Screensaver Trying To Start?
                case SC_MONITORPOWER:    // Monitor Trying To Enter Powersave?
                return 0;                // Prevent From Happening
            }
            break;                       // Exit
        }

    case WM_CLOSE:
        PostQuitMessage(0);              //Kill the current window
        break;

    case WM_KEYDOWN:
        switch( wParam )                 //key pressed
        {
        case VK_ESCAPE:
            //PostQuitMessage(0);        //if escape pressed, exit
            break;
        default:
            g_WndProcContext->SetKeyState((UINT)wParam, true);
            break;
        }
        break;

    case WM_KEYUP:
        g_WndProcContext->SetKeyState((UINT)wParam, false);
        break;

    case WM_COMMAND:
        if ( 0 == HIWORD(wParam) )
        {
            g_WndProcContext->PushEvent(InputEvent(InputEventMenu, (UINT)wParam));
           //PostQuitMessage(0);
        }
        break;

    case WM_SYSKEYDOWN:
        // Alt key pressed
        if ((TCHAR)wParam == KEY_ENTER)
        {
            g_WndProcContext->SetKeyState(KEY_ALT_ENTER, true);
            return NULL;
        }
      break;
      case WM_SYSKEYUP:
        // Alt key pressed
        if ((TCHAR)wParam == KEY_ENTER)
        {
            g_WndProcContext->SetKeyState(KEY_ALT_ENTER, false);
            return NULL;
        }
        break;

    case WM_LBUTTONDOWN:
        g_WndProcContext->SetMouseState(MouseButtonLeft, true);
        break;

    case WM_LBUTTONUP:
        g_WndProcContext->SetMouseState(MouseButtonLeft, false);
        break;

    case WM_RBUTTONDOWN:
        g_WndProcContext->SetMouseState(MouseButtonRight, true);
        break;

    case WM_RBUTTONUP:
        g_WndProcContext->SetMouseState(MouseButtonRight, false);
        break;

    case WM_MBUTTONDOWN:
        g_WndProcContext->SetMouseState(MouseButtonMiddle, true);
        break;

    case WM_MBUTTONUP:
        g_WndProcContext->SetMouseState(MouseButtonMiddle, false);
        break;

    case WM_MOUSEMOVE:
        {
            POINTS P = MAKEPOINTS(lParam);
            Vec2i NewPos(P.x, P.y);
            g_WndProcContext->UpdateMousePos(NewPos);
        }
        break;

    case WM_MOUSEWHEEL:
        g_WndProcContext->UpdateWheelState(GET_WHEEL_DELTA_WPARAM(wParam));
        break;

    case WM_SIZING:
        {
            /*double Ratio = g_WndProcContext->GetWindowManager().CastWindows().FixedAspectRatio();
            if(Ratio > 0.0)
            {
                RECT ScreenRect;
                GetWindowRect(GetDesktopWindow(), &ScreenRect);

                RECT *Rect = (RECT *)lParam;
                Vec2f BorderDimensions = g_WndProcContext->GetWindowManager().CastWindows().GetBorderDimensions();
                //Console::WriteLine(BorderDimensions.CommaSeperatedString());
                Vec2f ProposedDimensions = Vec2f(float(Rect->right - Rect->left), float(Rect->bottom - Rect->top)) - BorderDimensions;
                
                float NewWidth = float(ProposedDimensions.y / Ratio + BorderDimensions.x);
                float NewHeight = float(ProposedDimensions.x * Ratio + BorderDimensions.y);

                if(NewHeight > ScreenRect.bottom)
                {
                    NewHeight = float(ScreenRect.bottom);
                    NewWidth = float(ProposedDimensions.y / Ratio + BorderDimensions.x);
                    Rect->right = Rect->left + Math::Round(NewWidth);
                }
                
                switch(wParam)
                {
                case WMSZ_BOTTOM:
                    Rect->right = Rect->left + Math::Round(NewWidth);
                    break;
                case WMSZ_LEFT:
                case WMSZ_TOPLEFT:
                case WMSZ_TOPRIGHT:
                    Rect->top = Rect->bottom - Math::Round(NewHeight);
                    break;
                case WMSZ_RIGHT:
                case WMSZ_BOTTOMRIGHT:
                case WMSZ_BOTTOMLEFT:
                    Rect->bottom = Rect->top + Math::Round(NewHeight);
                    break;
                case WMSZ_TOP:
                    Rect->left = Rect->right - Math::Round(NewWidth);
                    break;
                }
                
                return TRUE;
            }*/
        }
        break;
        //Console::WriteLine(String(wParam) + " " + String(lParam)); 
    }

    return DefWindowProc( hWnd, msg, wParam, lParam ); //return handling of command to main
}
