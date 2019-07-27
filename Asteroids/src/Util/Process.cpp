#include "Asteroids/Util/Process.hpp"
#include "Asteroids/Util/UnidirectionalPipe.hpp"

#include <Urho3D/IO/Log.h>

#if defined(_WIN32)
#   include "Windows.h"
#elif defined(__linux__)
#   include <unistd.h>
#   include <stdio.h>
#   include <sys/wait.h>
#   include <sys/prctl.h>
#   include <errno.h>
#   include <signal.h>
#   include <fcntl.h>
#   include <stdlib.h>
#endif

using namespace Urho3D;

namespace Asteroids {

enum
{
    READ = 0,
    WRITE = 1
};

// ----------------------------------------------------------------------------
void LogLastError(const char* pre)
{
    LPSTR lpMessageBuffer = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMessageBuffer, 0, NULL);

    URHO3D_LOGERRORF("%s: %s", pre, lpMessageBuffer);
    LocalFree(lpMessageBuffer);
}

// ----------------------------------------------------------------------------
Process::Process()
{
}

// ----------------------------------------------------------------------------
Process::~Process()
{
    Close();
}

// ----------------------------------------------------------------------------
bool Process::Open(StringVector args, IO options)
{
#if defined(_WIN32)
    HANDLE stdin_rd = nullptr;
    HANDLE stdin_wr = nullptr;
    HANDLE stdout_rd = nullptr;
    HANDLE stdout_wr = nullptr;
    HANDLE stderr_rd = nullptr;
    HANDLE stderr_wr = nullptr;

    // Pipe handles need to be inherited by the child process
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = nullptr;

    // Create pipes
    if ((options & STDIN) && !CreatePipe(&stdin_rd, &stdin_wr, &saAttr, 0))
        { LogLastError("Process::Open() - CreatePipe() failed"); goto pipe_failed; }
    if ((options & STDOUT) && !CreatePipe(&stdout_rd, &stdout_wr, &saAttr, 0))
        { LogLastError("Process::Open() - CreatePipe() failed"); goto pipe_failed; }
    if ((options & STDERR) && !CreatePipe(&stderr_rd, &stderr_wr, &saAttr, 0))
        { LogLastError("Process::Open() - CreatePipe() failed"); goto pipe_failed; }

    // Don't let child process inherit the ends of the pipes it doesn't need
    if ((options & STDIN) && !SetHandleInformation(stdin_wr, HANDLE_FLAG_INHERIT, 0))
        { LogLastError("Process::Open() - SetHandleInformation() failed"); goto set_handle_info_failed; }
    if ((options & STDOUT) && !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0))
        { LogLastError("Process::Open() - SetHandleInformation() failed"); goto set_handle_info_failed; }
    if ((options & STDERR) && !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0))
        { LogLastError("Process::Open() - SetHandleInformation() failed"); goto set_handle_info_failed; }

    // Init startup info with correct I/O handles
    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = (options & STDIN) ? stdin_rd : nullptr;
    siStartInfo.hStdOutput = (options & STDOUT) ? stdout_wr : nullptr;
    siStartInfo.hStdError = (options & STDERR) ? stderr_wr : nullptr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    char* lpApplicationName = (char*)malloc(sizeof(char) * (args[0].Length() + 1));
    strcpy(lpApplicationName, args[0].CString());

    int commandLineLength = 0;
    for (const auto& arg : args)
        commandLineLength += arg.Length() + 1;  // +1 for space
    char* lpCommandLine = (char*)malloc(sizeof(char) * commandLineLength);
    strcpy(lpCommandLine, args[0].CString());
    for (int i = 1; i < args.Size(); ++i)
    {
        strcat(lpCommandLine, " ");
        strcat(lpCommandLine, args[i].CString());
    }

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    BOOL bSuccess = CreateProcess(
        lpApplicationName,
        lpCommandLine,
        nullptr,            // process security attributes
        nullptr,            // primary thread security attributes
        TRUE,               // handles are inherited
        0,                  // creation flags
        nullptr,            // use parent's environment
        nullptr,            // use parent's current directory
        &siStartInfo,       // STARTUPINFO pointer
        &piProcInfo         // receives PROCESS_INFORMATION
    );

    free(lpCommandLine);
    free(lpApplicationName);

    if (!bSuccess)
    {
        LogLastError("Process::Open() - CreateProcess() failed");
        goto create_process_failed;
    }

    Close();
    
    CloseHandle(piProcInfo.hProcess);

    // Close ends of the pipees we don't need
    if (options & STDIN) CloseHandle(stdin_rd);
    if (options & STDOUT) CloseHandle(stdout_wr);
    if (options & STDERR) CloseHandle(stderr_wr);

    stdin_.Reset((options & STDIN) ? new UnidirectionalPipe(stdin_wr) : nullptr);
    stdout_.Reset((options & STDOUT) ? new UnidirectionalPipe(stdout_rd) : nullptr);
    stderr_.Reset((options & STDERR) ? new UnidirectionalPipe(stderr_rd) : nullptr);
    mainThreadHandle_ = piProcInfo.hThread;

    return true;

    create_process_failed:
    set_handle_info_failed:
    pipe_failed:
        if (stdin_rd)  CloseHandle(stdin_rd);
        if (stdin_wr)  CloseHandle(stdin_wr);
        if (stdout_rd) CloseHandle(stdout_rd);
        if (stdout_wr) CloseHandle(stdout_wr);
        if (stderr_rd) CloseHandle(stderr_rd);
        if (stderr_wr) CloseHandle(stderr_wr);

    return false;

#else
    pid_t child_pid;

    int stdin_fd[2] = {-1, -1};
    int stdout_fd[2] = {-1, -1};
    int stderr_fd[2] = {-1, -1};
    int child_status_fd[2] = {-1, -1};

    // Create 3 new pipes for stdin, stdout and stderr
    if ((options & STDIN) && pipe(stdin_fd) != 0)
        { URHO3D_LOGERRORF("Process::Open() - pipe() failed: %s", strerror(errno)); goto pipe_failed; }
    if ((options & STDOUT) && pipe(stdout_fd) != 0)
        { URHO3D_LOGERRORF("Process::Open() - pipe() failed: %s", strerror(errno)); goto pipe_failed; }
    if ((options & STDERR) && pipe(stderr_fd) != 0)
        { URHO3D_LOGERRORF("Process::Open() - pipe() failed: %s", strerror(errno)); goto pipe_failed; }

    if (pipe2(child_status_fd, O_CLOEXEC) != 0)
        { URHO3D_LOGERRORF("Process::Open() - pipe() failed: %s", strerror(errno)); goto pipe_failed; }

    // Fork process
    if ((child_pid = fork()) == -1)
    {
        URHO3D_LOGERRORF("Process::Open() - fork() failed: %s", strerror(errno));
        goto fork_failed;
    }
    
    if (child_pid == 0)  // Child process
    {
        if (options & STDIN)
        {
            close(stdin_fd[WRITE]);   // Close writing end of pipe
            dup2(stdin_fd[READ], 0);  // redirect stdin to pipe
        }

        if (options & STDOUT)
        {
            close(stdout_fd[READ]);    // Close reading end of pipe
            dup2(stdout_fd[WRITE], 1); // redirect pipe to stdout
        }

        if (options & STDERR)
        {
            close(stderr_fd[READ]);    // Close reading end of pipe
            dup2(stderr_fd[WRITE], 2); // redirect pipe to stderr
        }

        close(child_status_fd[READ]);

        // execv needs a specific structure to work
        char** argv = (char**)malloc(args.Size() * (sizeof(char*) + 1));
        for (int i = 0; i != args.Size(); ++i)
        {
            argv[i] = (char*)malloc(sizeof(char) * (strlen(args[i].CString()) + 1));
            strcpy(argv[i], args[i].CString());
        }
        argv[args.Size()] = NULL;

        // Needed so negative PIDs can kill children of /bin/sh
        setpgid(child_pid, child_pid);
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        execv(argv[0], argv);

        // If all goes well this code is never reached
        char errorCode = '\1';
        write(child_status_fd[WRITE], &errorCode, 1);
        close(child_status_fd[WRITE]);
        exit(0);
    }

    // If the child process successfully starts, because of the O_CLOEXEC flag
    // we set on the child_status pipe, the writing end should be closed when
    // execv() succeeds. On our end, this appears as an EOF and no bytes should
    // have been written to the pipe. If execv() fails, then the child process
    // will write 0x01 to the pipe before exiting, letting us know an error
    // occurred.
    close(child_status_fd[WRITE]);
    char status;
    int bytesRead = read(child_status_fd[READ], &status, 1);
    close(child_status_fd[READ]);

    // Oh oh, child wrote an error byte to the pipe
    if (bytesRead == 1)
    {
        if (status != 1)
            URHO3D_LOGWARNING("Received unexpected byte from child through status pipe.")
        URHO3D_LOGERRORF("Failed to start subprocess %s", args[0].CString());
        goto spawn_subprocess_failed;
    }

    // Otherwise read() should have returned 0, indicating EOF.
    if (bytesRead != 0)
    {
        URHO3D_LOGERRORF("Encountered error while read()'ing status of child process : %s", strerror(errno));
        kill(child_pid, SIGTERM);
        goto spawn_subprocess_failed;
    }

    Close();
    
    if (options & STDIN) close(stdin_fd[READ]);
    if (options & STDOUT) close(stdout_fd[WRITE]);
    if (options & STDERR) close(stderr_fd[WRITE]);

    stdin_ = (options & STDIN) ? new UnidirectionalPipe(stdin_fd[WRITE], "w") : nullptr;
    stdout_ = (options & STDOUT) ? new UnidirectionalPipe(stdout_fd[READ], "r") : nullptr;
    stderr_ = (options & STDERR) ? new UnidirectionalPipe(stderr_fd[READ], "r") : nullptr;
    pid_ = child_pid;

    return true;

    spawn_subprocess_failed :
    fork_failed :
    pipe_failed :
        if (stdin_fd[READ] != -1)         close(stdin_fd[READ]);
        if (stdin_fd[WRITE] != -1)        close(stdin_fd[WRITE]);
        if (stdout_fd[READ] != -1)        close(stdout_fd[READ]);
        if (stdout_fd[WRITE] != -1)       close(stdout_fd[WRITE]);
        if (stderr_fd[READ] != -1)        close(stderr_fd[READ]);
        if (stderr_fd[WRITE] != -1)       close(stderr_fd[WRITE]);
        if (child_status_fd[WRITE] != -1) close(child_status_fd[WRITE]);
        if (child_status_fd[WRITE] != -1) close(child_status_fd[WRITE]);
    return false;
#endif
}

// ----------------------------------------------------------------------------
void Process::Close()
{
    RequestClose();
    Wait();
}

// ----------------------------------------------------------------------------
void Process::RequestClose()
{
#if defined(_WIN32)
    if (mainThreadHandle_ == nullptr)
        return;

    BOOL bSuccess = PostThreadMessage(
        GetThreadId(mainThreadHandle_),
        WM_CLOSE,
        0,
        0
    );
    if (!bSuccess)
        URHO3D_LOGWARNING("PostThreadMessage() failed");
#else
    if (pid_ == 0)
        return;

    if (kill(pid_, SIGTERM) == -1)
        URHO3D_LOGWARNINGF("kill() failed: %s", strerror(errno));
#endif

    stdin_.Reset();
    stdout_.Reset();
    stderr_.Reset();
}

// ----------------------------------------------------------------------------
void Process::Terminate()
{
#if defined(_WIN32)
    if (mainThreadHandle_ == nullptr)
        return;

    if (!TerminateProcess(mainThreadHandle_, 1))
        URHO3D_LOGWARNING("TerminateProcess() failed");
    
    CloseHandle(mainThreadHandle_);
    mainThreadHandle_ = nullptr;
#else
    if (pid_ == 0)
        return;

    if (kill(pid_, SIGKILL) == -1)
        URHO3D_LOGWARNINGF("kill() failed: %s", strerror(errno));
    pid_ = 0;
#endif
    stdin_.Reset();
    stdout_.Reset();
    stderr_.Reset();
}

// ----------------------------------------------------------------------------
void Process::Wait()
{
#if defined(_WIN32)
    if (mainThreadHandle_ == nullptr)
        return;

    if (WaitForSingleObject(mainThreadHandle_, INFINITE) != WAIT_OBJECT_0)
        URHO3D_LOGERROR("WaitForSingleObject() didn't return WAIT_OBJECT_0");

    CloseHandle(mainThreadHandle_);
    mainThreadHandle_ = nullptr;
#else
    if (pid_ == 0)
        return;

    int status;
    if (waitpid(pid_, &status, WUNTRACED) == -1)
    {
        URHO3D_LOGERRORF("Process::Wait() - waitpid() returned error: %s", strerror(errno));
    }

    pid_ = 0;
#endif
}

}
