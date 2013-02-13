class ThreadPool
{
public:
    ~ThreadPool();
    void FreeMemory();

    void Init(UINT threadCount);
    void Init(UINT threadCount, Vector<ThreadLocalStorage*> &theadLocalStorage);
    void Go(TaskList<WorkerThreadTask*> &tasks, bool useConsole = true);

private:
    Vector<WorkerThread*> _threads;
};
