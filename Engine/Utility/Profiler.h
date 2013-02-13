/*
Profiler.h
Written by Matthew Fisher

Profiler is used to record the percent of time spent in code.
*/

#ifdef PROFILING_ENABLED

class Profiler
{
public:
    void AddEntry(UINT index, const String &description);
    void Reset(UINT startState);
	__forceinline void ChangeState(UINT newState)
	{
		INT64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		_totalsPointer[_previousState] += currentTime - _previousTime;
		_previousState = newState;
		_previousTime = currentTime;
	}
    String Report();

private:
	INT64 *_totalsPointer;
	UINT _previousState;
	INT64 _previousTime;
	Vector<INT64> _totals;
    Vector<String> _descriptions;    
};

#else

class Profiler
{
public:
    void AddEntry(UINT index, const String &description);
    void Reset(UINT startState);
	__forceinline void ChangeState(UINT newState)
	{
		
	}
    String Report();

private:
	INT64 _startTime;
};

#endif