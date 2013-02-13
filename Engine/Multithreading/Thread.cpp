/*
Thread.cpp
Written by Matthew Fisher

A thread is a single unit of code execution
*/
#include <process.h>

Thread::Thread()
{
    _Handle = NULL;
}

Thread::~Thread()
{
    //CloseThread();
}

void Thread::CloseThread()
{
    _Handle = NULL;
    //SignalError("Not implemented");
    /*if(_Handle)
    {
        CloseHandle(_Handle);
        bool threadDone = false;
        while(!threadDone)
        {
            DWORD ExitCode = 0;
            BOOL Success = GetExitCodeThread(_Handle, &ExitCode);
            if(!Success || ExitCode != STILL_ACTIVE)
            {
                threadDone = true;
            }
            else
            {
                Sleep(1);
            }
        }
        _Handle = NULL;
    }*/
}

//void Thread::Begin(LPTHREAD_START_ROUTINE StartFunction, void *Context)
void Thread::BeginStdCall(unsigned int (__stdcall *StartFunction)(void *), void *Context)
{
    PersistentAssert(_Handle == NULL, "Begin called on active thread");
    //
    // without the CREATE_SUSPENDED flag, the thread will start immediately
    //
    //_Handle = CreateThread(NULL, 0, StartFunction, Context, 0, NULL);
    _Handle = (HANDLE)_beginthreadex(NULL, 0, StartFunction, Context, 0, NULL);
    PersistentAssert(_Handle != NULL, "CreateThread failed");
}

void Thread::BeginCdecl(void (__cdecl *StartFunction)(void *), void *Context)
{
    PersistentAssert(_Handle == NULL, "Begin called on active thread");
    //
    // without the CREATE_SUSPENDED flag, the thread will start immediately
    //
    //_Handle = CreateThread(NULL, 0, StartFunction, Context, 0, NULL);
    _Handle = (HANDLE)_beginthread(StartFunction, 0, Context);
    //PersistentAssert(_Handle != NULL, "CreateThread failed");
}
