#include "Client/Process.hpp"

#include <Urho3D/IO/Log.h>

#if defined(_WIN32)
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
UnidirectionalPipe::UnidirectionalPipe(int fd, const char* mode)
{
    fp_ = fdopen(fd, mode);
}

// ----------------------------------------------------------------------------
UnidirectionalPipe::~UnidirectionalPipe()
{
    Close();
}

// ----------------------------------------------------------------------------
void UnidirectionalPipe::Close()
{
    if (fp_)
        fclose(fp_);

    fp_ = nullptr;
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Read(void* dest, unsigned size)
{
    return fread(dest, 1, size, fp_);
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Seek(unsigned position)
{
    return 0;
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Write(const void* data, unsigned size)
{
    return fwrite(data, 1, size, fp_);
}

// ----------------------------------------------------------------------------
bool UnidirectionalPipe::IsEof() const
{
    return feof(fp_);
}

// ----------------------------------------------------------------------------
Process::~Process()
{
    Close();
}

// ----------------------------------------------------------------------------
bool Process::Open(StringVector args, IO options)
{
    pid_t child_pid;

    int stdin_fd[2] = {-1, -1};
    int stdout_fd[2] = {-1, -1};
    int stderr_fd[2] = {-1, -1};
    int child_status_fd[2] = {-1, -1};

    UniquePtr<UnidirectionalPipe> newStdin;
    UniquePtr<UnidirectionalPipe> newStdout;
    UniquePtr<UnidirectionalPipe> newStderr;

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
    else
    {
        if (options & STDIN)
        {
            close(stdin_fd[READ]);
            newStdin = new UnidirectionalPipe(stdin_fd[WRITE], "w");
        }

        if (options & STDOUT)
        {
            close(stdout_fd[WRITE]);
            newStdout = new UnidirectionalPipe(stdout_fd[READ], "r");
        }

        if (options & STDERR)
        {
            close(stderr_fd[WRITE]);
            newStderr = new UnidirectionalPipe(stderr_fd[READ], "r");
        }

        close(child_status_fd[WRITE]);

        char status;
        int bytesRead = read(child_status_fd[READ], &status, 1);
        close(child_status_fd[READ]);

        if (bytesRead == 1 && status == '\1')
        {
            URHO3D_LOGERRORF("Failed to start subprocess %s", args[0].CString());
            goto spawn_subprocess_failed;
        }

        if (bytesRead != 0)
        {
            URHO3D_LOGERRORF("Encountered error while read()'ing status of child process : %s", strerror(errno));
            kill(child_pid, SIGTERM);
            goto spawn_subprocess_failed;
        }
    }

    Close();

    stdin_ = std::move(newStdin);
    stdout_ = std::move(newStdout);
    stderr_ = std::move(newStderr);
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
    if (pid_ == 0)
        return;

    if (kill(pid_, SIGTERM) == -1)
        URHO3D_LOGWARNINGF("kill() failed: %s", strerror(errno));

    stdin_.Reset();
    stdout_.Reset();
    stderr_.Reset();
}

// ----------------------------------------------------------------------------
void Process::Terminate()
{
    if (pid_ == 0)
        return;

    if (kill(pid_, SIGKILL) == -1)
        URHO3D_LOGWARNINGF("kill() failed: %s", strerror(errno));

    stdin_.Reset();
    stdout_.Reset();
    stderr_.Reset();
    pid_ = 0;
}

// ----------------------------------------------------------------------------
void Process::Wait()
{
    if (pid_ == 0)
        return;

    int status;
    if (waitpid(pid_, &status, WUNTRACED) == -1)
    {
        URHO3D_LOGERRORF("Process::Wait() - waitpid() returned error: %s", strerror(errno));
    }

    pid_ = 0;
}

}
