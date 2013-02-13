/*
WinMain.cpp
Written by Matthew Fisher

The WinMain function itself.  Just creates the App class and runs it.  See App.h/App.cpp
*/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpszCmdLine, int nCmdShow)
{
    App *A = new App;                    //create a new App
    A->Init(hInstance, nCmdShow);        //initalize App
    A->MessageLoop(hInstance, nCmdShow); //begin App's "lifetime" (looping for messages.  all frames occur here.)
    A->FreeMemory();                     //kill App
    return 0;                            //exit Main
}