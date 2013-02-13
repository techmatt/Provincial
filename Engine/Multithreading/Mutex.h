/*
Mutex.h
Written by Matthew Fisher

A mutex is an object that grants a thread exclusive access to a given section of code.
*/

//
// Note: CRITICAL_SECTION stores a ref count each time EnterCriticalSection is called,
// and LeaveCriticalSection must be called once for each EnterCriticalSection.
//

struct Mutex
{
public:
    __forceinline Mutex()
    {
        InitializeCriticalSection(&_criticalSection);
    }
    __forceinline ~Mutex()
    {
        DeleteCriticalSection(&_criticalSection);
    }
    __forceinline bool TryAcquire()
    {
        return (TryEnterCriticalSection(&_criticalSection) != 0);
    }
    __forceinline void Acquire()
    {
        EnterCriticalSection(&_criticalSection);
    }
    __forceinline void Release()
    {
        LeaveCriticalSection(&_criticalSection);
    }

private:
    CRITICAL_SECTION _criticalSection;
};
