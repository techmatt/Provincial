/*
FrameTimer.h
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

class Clock
{
public:
    Clock();
    void Start();
    double Elapsed();

    __forceinline __int64 StartTime()
    {
        return _StartTime;
    }
    __forceinline __int64 TicksPerSecond()
    {
        return _TicksPerSecond;
    }

private:
    __int64 _StartTime;
    __int64 _TicksPerSecond;
};

class ComponentTimer
{
public:
    ComponentTimer(const String &prompt);
    ~ComponentTimer();

private:
    Clock _timer;
    String _prompt;
};

class FrameTimer
{
public:

    void Start(float fFramesPerSec);    //begin at some reasonable frames per second
    void Frame();                        //call this each frame
    
    void SetFPS(float NewFPS);    //set the frames per second to a fixed value used for
                                //things like video capture where you want to trick
                                //the rest of the application into thinking the FPS is normal
    
    void IgnoreTime();            //ignores the time lapsed since the previous call to Frame
                                //useful when you have processes (such as video capture)
                                //that you don't want to be reigstered in the FPS calculation
    
    void Pause();                //Pauses the frame timer for 5 frames.  Used after screen resizing
                                //to avoid sudden jerkiness due to the time it takes to adjust to the
                                //new screen size

    float FPS();                //returns the frames per second
    float SPF();                //returns the seconds per frame

    __int64 GetTime();            //returns the current time

protected:
    bool Paused;                //true if Pause was called within 5 turns
    int NullFramesLeft;            //how many frames since Pause was called
    float m_FPS,m_SPF;            //local FPS and SPF variables
    __int64 TPS;                //ticks per second
    __int64 Prev_Time;            //the time since Frame was last called
};

inline float
FrameTimer::FPS()
{
    return float(m_FPS);
}

inline float
FrameTimer::SPF()
{
    return float(m_SPF);
}
