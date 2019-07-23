#pragma once

#include <memory>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/IO/AbstractFile.h>

namespace Asteroids {

class UnidirectionalPipe : public Urho3D::AbstractFile
{
public:
    /// Destruct and close.
    ~UnidirectionalPipe();

    void Close();

    /// Read bytes from the pipe without blocking if there is less data available. Return number of bytes actually read.
    unsigned Read(void* dest, unsigned size) override;
    /// Set position. No-op for pipes.
    unsigned Seek(unsigned position) override;
    /// Write bytes to the pipe. Return number of bytes actually written.
    unsigned Write(const void* data, unsigned size) override;
    /// Return whether pipe has no data available.
    bool IsEof() const override;
    /// Return the pipe name.
    const Urho3D::String& GetName() const override { return name_; }

private:
    friend class Process;

    Urho3D::String name_;

#ifdef _WIN32
    void* handle_;
#else
    /// Construct.
    UnidirectionalPipe(int fd, const char* mode);

    FILE* fp_;
#endif
};

class Process : public Urho3D::RefCounted
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

    ~Process();

    bool Open(Urho3D::StringVector argv, IO options=NONE);
    void Close();
    void RequestClose();
    void Wait();
    void Terminate();

private:
    Urho3D::UniquePtr<UnidirectionalPipe> stdin_;
    Urho3D::UniquePtr<UnidirectionalPipe> stdout_;
    Urho3D::UniquePtr<UnidirectionalPipe> stderr_;

#ifdef _WIN32
#else
    int pid_ = 0;
#endif
};

}
