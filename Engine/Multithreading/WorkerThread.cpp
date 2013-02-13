#include "Main.h"

//unsigned int WINAPI WorkerThreadEntry( LPVOID context )
void __cdecl WorkerThreadEntry( LPVOID context )
{
    WorkerThread *curWorkerThread = (WorkerThread *)context;
    curWorkerThread->Entry();
    //return 0;
}

void WorkerThread::Init(ThreadLocalStorage *storage)
{
    _storage = storage;
}

WorkerThread::~WorkerThread()
{
    
}

void WorkerThread::Start(TaskList<WorkerThreadTask*> &tasks)
{
    _done = false;
    _tasks = &tasks;
    _thread.CloseThread();
    _thread.BeginCdecl(WorkerThreadEntry, this);
}

void WorkerThread::Entry()
{
    WorkerThreadTask* curTask;
    while(_tasks->GetNextTask(curTask))
    {
        curTask->Run(_storage);
        delete curTask;
    }
    _done = true;
}
