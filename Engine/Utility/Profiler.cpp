/*
Profiler.cpp
Written by Matthew Fisher

Profiler is used to record the percent of time spent in code.
*/

#ifdef PROFILING_ENABLED

void Profiler::AddEntry(UINT index, const String &description)
{
	_descriptions.ReSize(index + 1);
	_descriptions[index] = description;
}
void Profiler::Reset(UINT startState)
{
	_totals.Allocate(_descriptions.Length());
	_totals.Clear(0);
	_totalsPointer = _totals.CArray();
	QueryPerformanceCounter((LARGE_INTEGER*)&_previousTime);
	_previousState = startState;
}
String Profiler::Report()
{
	ChangeState(_previousState);

	INT64 ticksPerSecond;
	QueryPerformanceFrequency( (LARGE_INTEGER*)&ticksPerSecond );

	INT64 totalTime = 0;
	for(UINT index = 0; index < _totals.Length(); index++)
	{
		totalTime += _totals[index];
	}

	String result;
	result = String("Total time: ") + String(double(totalTime) / double(ticksPerSecond)) + String("s\n");
	for(UINT index = 0; index < _totals.Length(); index++)
	{
		result += _descriptions[index] + String(": ") + String(double(_totals[index]) / double(totalTime) * 100.0f) + String("%, ") + String(double(_totals[index]) / double(ticksPerSecond)) + String("s\n");
	}
	return result;
}

#else

void Profiler::AddEntry(UINT index, const String &description)
{
		
}

void Profiler::Reset(UINT startState)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&_startTime);
}

String Profiler::Report()
{
	INT64 currentTime, ticksPerSecond;
	QueryPerformanceFrequency( (LARGE_INTEGER*)&ticksPerSecond );
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	String result;
	result = String("Total time: ") + String(double(currentTime - _startTime) / double(ticksPerSecond)) + String("s\n");
	return result;
}

#endif