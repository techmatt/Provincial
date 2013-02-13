/*
FrameTimer.cpp
Origionally from the DirectX SDK
Modified by Matthew Fisher

The frame timer reads the system time and maintains the frames per second counter.
This is useful for making motion approximately the same speed from the user's 
perspective despite vast flucuation in the FPS.  If you want a unit to move
A units per second, just multiply it by the FrameTimer's seconds per frame to get
how many units it should move per frame.  It works by using the immediate time lapse
between frames combined with a decaying history of time lapse to avoid incredibly jerky
motion.
*/

Clock::Clock()
{
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    _TicksPerSecond = qwTicksPerSec.QuadPart;
    Start();
}

void Clock::Start()
{
    LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );
    _StartTime = qwTime.QuadPart;
}

double Clock::Elapsed()
{
    LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );
    return double(qwTime.QuadPart - _StartTime) / double(_TicksPerSecond);
}

ComponentTimer::ComponentTimer(const String &prompt)
{
    _prompt = prompt;
    Console::WriteLine(String("Begin: ") + prompt);
}

ComponentTimer::~ComponentTimer()
{
    Console::WriteLine(String("End: ") + String(_timer.Elapsed()) + String("s"));
}

void FrameTimer::Start(float fFramesPerSec)
{
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    TPS = qwTicksPerSec.QuadPart;                    //load TPS

    Prev_Time = GetTime();    //load the current time

    m_FPS = fFramesPerSec;
    m_SPF = 1.0f / m_FPS;
    Paused = false;
    NullFramesLeft = 0;        //initalize FPS, SPF, etc.
}

void FrameTimer::Pause()
{
    Paused = true;
    NullFramesLeft = 5;
}

void FrameTimer::Frame()
{
    __int64 curTime = GetTime();        //get the current time,

    if(!Paused)                            //if we're not paused,
    {
        m_SPF = float(curTime - Prev_Time) / float(TPS) * 0.2f + 0.8f * m_SPF;    //compute the SPF as a weighted average of
                                                                                //the intantaneous SPF and the stored SPF.
        m_FPS = 1.0f / m_SPF;                    //FPS is just the inverse of SPF
    } else {
        if(NullFramesLeft) NullFramesLeft--;    //if we are paused decrement the number of paused frames left
        else Paused = false;
    }
    Prev_Time = curTime;                //store this time as the previous time
}

void FrameTimer::SetFPS(float NewFPS)
{
    __int64 curTime = GetTime();
    m_FPS = NewFPS;
    m_SPF = 1.0f / m_FPS;            //load the requested FPS and SPF
    Prev_Time = curTime;
}

__int64 FrameTimer::GetTime()
{
    LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );    //query the high performance timer
    return qwTime.QuadPart;
}

void FrameTimer::IgnoreTime()
{
    Prev_Time = GetTime();        //make it seem as if the previous frame was at the time of this call
}
