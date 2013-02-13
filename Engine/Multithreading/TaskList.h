template <class T> class TaskList
{
public:
    void InsertTask(T task)
    {
        _lock.Acquire();
        _tasks.PushEnd(task);
        _lock.Release();
    }

    bool Done()
    {
        _lock.Acquire();
        bool result = (_tasks.Length() == 0);
        _lock.Release();
        return result;
    }

    UINT TasksLeft()
    {
        _lock.Acquire();
        UINT result = _tasks.Length();
        _lock.Release();
        return result;
    }

    bool GetNextTask(T &nextTask)
    {
        _lock.Acquire();
        if(_tasks.Length() == 0)
        {
            _lock.Release();
            return false;
        }
        else
        {
            nextTask = _tasks.Last();
            _tasks.PopEnd();
            _lock.Release();
            return true;
        }
    }

private:
    Mutex _lock;
    Vector<T> _tasks;
};