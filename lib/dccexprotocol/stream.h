#ifndef _STREAM_H
#define _STREAM_H

#include <stddef.h>

class Stream {
public:
    virtual ~Stream() {}

    virtual int available() const = 0;
    virtual int read() = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;
    virtual void flush() = 0;
    virtual void println(const char* format, ...) = 0;
    virtual void print(const char* format, ...) = 0;
};

#endif