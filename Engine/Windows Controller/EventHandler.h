/*
EventHandler.h
Written by Matthew Fisher

Creating a new window involves having a callback function, WndProc, that is called whenever Windows has
an event to send to your window/application.
*/

extern InputManager *g_WndProcContext;
LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );    //the windows callback function