/*
Pipe.cpp
Written by Matthew Fisher

A pipe is a connection between two programs, possibly on different computers.
*/

#include <aclapi.h>

Pipe::Pipe()
{
    _Handle = NULL;
}

Pipe::~Pipe()
{
    ClosePipe();
}

void Pipe::ClosePipe()
{
    if(_Handle != NULL)
    {
        FlushFileBuffers(_Handle);
        DisconnectNamedPipe(_Handle);
        CloseHandle(_Handle);
        _Handle = NULL;
    }
}

void Pipe::CreatePipe(const String &PipeName, bool block)
{
    ClosePipe();
    const UINT PipeBufferSize = 100000;

    DWORD dwRes;
    PSID pEveryoneSID = NULL, pAdminSID = NULL;
    PACL pACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea[1];
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    SECURITY_ATTRIBUTES Attributes;
    HKEY hkSub = NULL;

    // Create a well-known SID for the Everyone group.
    BOOL Success = AllocateAndInitializeSid(&SIDAuthWorld, 1,
                                            SECURITY_WORLD_RID,
                                            0, 0, 0, 0, 0, 0, 0,
                                            &pEveryoneSID);
    Assert(Success != FALSE, "AllocateAndInitializeSid failed in Pipe::CreatePipe");

    // Initialize an EXPLICIT_ACCESS structure for an ACE.
    // The ACE will allow Everyone read access to the key.
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions = FILE_ALL_ACCESS;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance= NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

    // Create a new ACL that contains the new ACEs.
    dwRes = SetEntriesInAcl(1, ea, NULL, &pACL);
    Assert(dwRes == ERROR_SUCCESS, "SetEntriesInAcl failed in Pipe::CreatePipe");

    // Initialize a security descriptor.  
    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
    Assert(pSD != NULL, "LocalAlloc failed in Pipe::CreatePipe");
    
    Success = InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
    Assert(Success != FALSE, "InitializeSecurityDescriptor failed in Pipe::CreatePipe");
    
    // Add the ACL to the security descriptor. 
    Success = SetSecurityDescriptorDacl(pSD, 
                TRUE,     // bDaclPresent flag
                pACL, 
                FALSE);
    Assert(Success != FALSE, "SetSecurityDescriptorDacl failed in Pipe::CreatePipe");
    
    // Initialize a security attributes structure.
    Attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    Attributes.lpSecurityDescriptor = pSD;
    Attributes.bInheritHandle = FALSE;

    String FullPipeName = String("\\\\.\\pipe\\") + PipeName;
    _Handle = CreateNamedPipe( 
        FullPipeName.CString(),     // pipe name 
        PIPE_ACCESS_DUPLEX,         // read/write access 
        PIPE_TYPE_MESSAGE |         // message type pipe 
        PIPE_READMODE_MESSAGE |     // message-read mode 
        PIPE_WAIT,                  // blocking mode 
        PIPE_UNLIMITED_INSTANCES,   // max. instances  
        PipeBufferSize,             // output buffer size 
        PipeBufferSize,             // input buffer size 
        NMPWAIT_USE_DEFAULT_WAIT,   // client time-out 
        &Attributes);               // default security attribute
    Assert(_Handle != INVALID_HANDLE_VALUE, "CreateNamedPipe failed in Pipe::CreatePipe");

    //
    // Block until a connection comes in
    //
    if(block)
    {
        BOOL Connected = (ConnectNamedPipe(_Handle, NULL) != 0);
        Assert(Connected != FALSE, "ConnectNamedPipe failed in Pipe::CreatePipe");
    }
}

void Pipe::ConnectToLocalPipe(const String &PipeName)
{
    ConnectToPipe(String("\\\\.\\pipe\\") + PipeName);
}

void Pipe::ConnectToPipe(const String &PipeName)
{
    ClosePipe();
    bool Done = false;
    while(!Done)
    {
        _Handle = CreateFile( 
            PipeName.CString(),           // pipe name 
            GENERIC_READ |                // read and write access 
            GENERIC_WRITE, 
            0,                            // no sharing 
            NULL,                         // default security attributes
            OPEN_EXISTING,                // opens existing pipe 
            0,                            // default attributes 
            NULL);                        // no template file
        if(_Handle != INVALID_HANDLE_VALUE)
        {
            Done = true;
        }
        Sleep(100);
    }

    DWORD Mode = PIPE_READMODE_MESSAGE;
    BOOL Success = SetNamedPipeHandleState( 
        _Handle,  // pipe handle 
        &Mode,    // new pipe mode 
        NULL,     // don't set maximum bytes 
        NULL);    // don't set maximum time 
    Assert(Success != FALSE, "SetNamedPipeHandleState failed in Pipe::ConnectToPipe");
}

bool Pipe::MessagePresent()
{
    Assert(_Handle != NULL, "Pipe invalid in Pipe::MessagePresent");
    DWORD BytesReady  = 0;
    BOOL Success = PeekNamedPipe(
        _Handle,
        NULL,
        0,
        NULL,
        &BytesReady,
        NULL);
    //Assert(Success != FALSE, "PeekNamedPipe failed in Pipe::MessagePresent");
    return (BytesReady > 0);
}

bool Pipe::ReadMessage(Vector<BYTE> &Message)
{
    Assert(_Handle != NULL, "Pipe invalid in Pipe::ReadMessage");
    DWORD BytesReady  = 0;
    BOOL Success = PeekNamedPipe(
        _Handle,
        NULL,
        0,
        NULL,
        &BytesReady,
        NULL);
    Assert(Success != FALSE, "PeekNamedPipe failed in Pipe::ReadMessage");
    Message.Allocate(BytesReady);
    if(BytesReady == 0)
    {
        return false;
    }

    DWORD BytesRead;
    Success = ReadFile( 
        _Handle,                // handle to pipe 
        Message.CArray(),       // buffer to receive data 
        Message.Length(),       // size of buffer 
        &BytesRead,             // number of bytes read 
        NULL);                  // not overlapped I/O 
    Assert(Success != FALSE && BytesRead > 0, "ReadFile failed in Pipe::ReadMessage");
    return true;
}

void Pipe::SendMessage(const Vector<BYTE> &Message)
{
    SendMessage(Message.CArray(), Message.Length());
}

void Pipe::SendMessage(const BYTE *Message, UINT MessageLength)
{
    Assert(_Handle != NULL, "Pipe invalid in Pipe::SendMessage");

    DWORD BytesWritten;
    BOOL Success = WriteFile( 
        _Handle,               // pipe handle
        Message,               // message
        MessageLength,         // message length
        &BytesWritten,         // bytes written
        NULL);                 // not overlapped
    Assert(Success != FALSE, "WriteFile failed in Pipe::ReadMessage");
    Assert(BytesWritten == MessageLength, "WriteFile failed to send entire message in Pipe::ReadMessage");
}

UINT Pipe::ActiveInstances()
{
    Assert(_Handle != NULL, "Pipe invalid in Pipe::ActiveInstances");
    DWORD Instances;
    BOOL Success = GetNamedPipeHandleState(
        _Handle,
        NULL,
        &Instances,
        NULL,
        NULL,
        NULL,
        0);
    Assert(Success != FALSE, "GetNamedPipeHandleState failed in Pipe::ActiveInstances");
    return Instances;
}

String Pipe::UserName()
{
    Assert(_Handle != NULL, "Pipe invalid in Pipe::UserName");
    char Buffer[512];
    BOOL Success = GetNamedPipeHandleState(
        _Handle,
        NULL,
        NULL,
        NULL,
        NULL,
        Buffer,
        512);
    Assert(Success != FALSE, "GetNamedPipeHandleState failed in Pipe::UserName");
    return String(Buffer);
}
