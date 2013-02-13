#include "Main.h"

ThreadPool::~ThreadPool()
{
    FreeMemory();
}

void ThreadPool::FreeMemory()
{
    _threads.DeleteMemory();
}

void ThreadPool::Init(UINT threadCount)
{
    _threads.Allocate(threadCount);
    for(UINT threadIndex = 0; threadIndex < threadCount; threadIndex++)
    {
        _threads[threadIndex] = new WorkerThread;
        _threads[threadIndex]->Init(NULL);
    }
}

void ThreadPool::Init(UINT threadCount, Vector<ThreadLocalStorage*> &theadLocalStorage)
{
    _threads.Allocate(threadCount);
    for(UINT threadIndex = 0; threadIndex < threadCount; threadIndex++)
    {
        _threads[threadIndex] = new WorkerThread;
        _threads[threadIndex]->Init(theadLocalStorage[threadIndex]);
    }
}

void ThreadPool::Go(TaskList<WorkerThreadTask*> &tasks, bool useConsole)
{
    for(UINT threadIndex = 0; threadIndex < _threads.Length(); threadIndex++)
    {
        _threads[threadIndex]->Start(tasks);
    }
    if(useConsole) Console::AdvanceLine();

    UINT consoleDelay = 0;

    bool allThreadsCompleted = false;
    while(!allThreadsCompleted)
    {
        UINT activeThreadCount = 0;
        for(UINT threadIndex = 0; threadIndex < _threads.Length(); threadIndex++)
        {
            if(!_threads[threadIndex]->Done())
            {
                activeThreadCount++;
            }
        }
        if(activeThreadCount == 0)
        {
            allThreadsCompleted = true;
        }

        if(consoleDelay == 0)
        {
            if(useConsole) Console::OverwriteLine(String("Tasks left: ") + String(tasks.TasksLeft() + activeThreadCount));
            consoleDelay = 40;
        }
        else
        {
            consoleDelay--;
        }
        Sleep(25);
    }
    if(useConsole) Console::OverwriteLine("All tasks completed.");
}