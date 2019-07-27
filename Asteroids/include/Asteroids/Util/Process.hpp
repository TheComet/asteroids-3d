#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Core/Variant.h>

namespace Asteroids {

class UnidirectionalPipe;

class ASTEROIDS_PUBLIC_API Process : public Urho3D::RefCounted
{
public:
    enum IO
    {
        STDIN  = 0x01,
        STDOUT = 0x02,
        STDERR = 0x04,
        NONE = 0,
        ALL = STDIN | STDOUT | STDERR
    };

    Process();
    /// Calls Close()
    ~Process();

    /*!
     * @brief Spawns a new child process.
     *
     * @param[in] args This is the argv[] argument that gets passed to the
     * child's main() function. The first entry, args[0], is mandatory and
     * must be the file name (absolute path or relative to the calling process'
     * working directory) of the binary to execute. The proceeding arguments
     * args[1..] are all optional.
     *
     * @param[in] options This controls which standard streams to pipe back to
     * the calling process. By default, nothing is piped and the child process
     * will output to stdin, stdout and stderr like usual. The functions
     * GetStdIn(), GetStdOut() and GetStdErr() in this case will return NULL.
     * You can specify an OR'd combination of the flags STDIN, STDOUT and STDERR
     * which lets you read from and write to the child process' standard streams
     * via the corresponding GetStdIn(), GetStdOut() and GetStdErr() pipes.
     */
    bool Open(Urho3D::StringVector args, IO options=NONE);

    /*!
     * @brief Tries to shut down the child process gracefully.
     * @note This function returns immediately and the child process will
     * probably still be running. You can wait for it to terminate by calling
     * Wait().
     *
     * On linux, this causes SIGTERM to be sent to the child process.
     */
    void RequestClose();

    /*!
     * @brief Waits for the child process to terminate.
     *
     * On linux, this calls waitpid().
     */
    void Wait();

    /*!
     * @brief A convenience function that first calls RequestClose() and then Wait().
     */
    void Close();

    /*!
     * @brief Kills the child process immediately. You shouldn't normally have 
     * to use this.
     *
     * On linux, this causes SIGKILL to be sent to the child process.
     */
    void Terminate();

    /*!
     * @brief Retrieves the stdin pipe, if it was specified to Open(). Will be
     * NULL otherwise.
     * @note You can only write to this pipe.
     */
    UnidirectionalPipe* GetStdIn() const;

    /*!
     * @brief Retrieves the stdout pipe, if it was specified to Open(). Will be
     * NULL otherwise.
     * @note You can only read from this pipe.
     */
    UnidirectionalPipe* GetStdOut() const;

    /*!
     * @brief Retrieves the stderr pipe, if it was specified to Open(). Will be
     * NULL otherwise.
     * @note You can only read from this pipe.
     */
    UnidirectionalPipe* GetStdErr() const;

private:
    Urho3D::UniquePtr<UnidirectionalPipe> stdin_;
    Urho3D::UniquePtr<UnidirectionalPipe> stdout_;
    Urho3D::UniquePtr<UnidirectionalPipe> stderr_;

#ifdef _WIN32
    void* mainThreadHandle_ = nullptr;
#else
    int pid_ = 0;
#endif
};

}
