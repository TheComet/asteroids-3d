#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/IO/AbstractFile.h>

namespace Asteroids {

class ASTEROIDS_PUBLIC_API UnidirectionalPipe : public Urho3D::AbstractFile
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
    UnidirectionalPipe(void* handle);

    void* handle_;
#else
    /// Construct.
    UnidirectionalPipe(int fd, const char* mode);

    FILE* fp_;
#endif
};

}
