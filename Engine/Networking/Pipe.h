/*
Pipe.h
Written by Matthew Fisher

A pipe is a connection between two programs, possibly on different computers.
*/

class Pipe
{
public:
    Pipe();
    ~Pipe();
    
    //
    // Connection
    //
    void ClosePipe();
    void CreatePipe(const String &PipeName, bool block);
    void ConnectToLocalPipe(const String &PipeName);
    void ConnectToPipe(const String &PipeName);

    //
    // Messaging
    //
    bool MessagePresent();
    bool ReadMessage(Vector<BYTE> &Message);
    void SendMessage(const BYTE *Message, UINT MessageLength);
    void SendMessage(const Vector<BYTE> &Message);

    //
    // Query
    //
    UINT ActiveInstances();
    String UserName();
    __forceinline bool Valid()
    {
        return (_Handle != NULL);
    }
    

private:
    HANDLE _Handle;
};
