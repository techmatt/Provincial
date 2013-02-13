
//
// Abstract base class for thread local storage
//
class ThreadLocalStorage
{
public:

};

//
// Abstract base class for thread tasks
//
class WorkerThreadTask
{
public:
    virtual void Run(ThreadLocalStorage *threadLocalStorage) = 0;
};

class WorkerThread
{
public:
    ~WorkerThread();
    void Init(ThreadLocalStorage *storage);
    void Start(TaskList<WorkerThreadTask*> &tasks);
    void Entry();

    __forceinline bool Done()
    {
        return _done;
    }

private:
    bool _done;
    Thread _thread;
    ThreadLocalStorage *_storage;
    TaskList<WorkerThreadTask*> *_tasks;
};
