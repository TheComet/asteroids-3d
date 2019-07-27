#include "Asteroids/Util/UnidirectionalPipe.hpp"
#include <Windows.h>

namespace Asteroids {
   

// ----------------------------------------------------------------------------
#if defined(_WIN32)
UnidirectionalPipe::UnidirectionalPipe(void* handle) :
    handle_(handle)
{
}
#else
UnidirectionalPipe::UnidirectionalPipe(int fd, const char* mode)
{
    fp_ = fdopen(fd, mode);
}
#endif

// ----------------------------------------------------------------------------
UnidirectionalPipe::~UnidirectionalPipe()
{
    Close();
}

// ----------------------------------------------------------------------------
void UnidirectionalPipe::Close()
{
#if defined(_WIN32)
    if (handle_)
        CloseHandle(handle_);

    handle_ = nullptr;
#else
    if (fp_)
        fclose(fp_);

    fp_ = nullptr;
#endif
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Read(void* dest, unsigned size)
{
#if defined(_WIN32)
    return 0;
#else
    return fread(dest, 1, size, fp_);
#endif
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Seek(unsigned position)
{
    return 0;
}

// ----------------------------------------------------------------------------
unsigned UnidirectionalPipe::Write(const void* data, unsigned size)
{
#if defined(_WIN32)
    return 0;
#else
    return fwrite(data, 1, size, fp_);
#endif
}

// ----------------------------------------------------------------------------
bool UnidirectionalPipe::IsEof() const
{
#if defined(_WIN32)
    return true;
#else
    return feof(fp_);
#endif
}

}
