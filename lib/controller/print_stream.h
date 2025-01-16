#ifndef _PRINT_STREAM_H
#define _PRINT_STREAM_H

#include <stdio.h>
#include <stream.h>
#include <stdarg.h>

// void println(const char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     vprintf(format, args);
//     va_end(args);
//     printf("\n");
// }


class PrintStream : public Stream {
private:
public:
    // Check if data is available to read
    int available() const {
        return 0;
    }

    // Read a single byte from the socket
    int read() {
        return 0;
    }

    // Write a buffer to the socket
    size_t write(const uint8_t *buffer, size_t size) {
        return 0; // Error
    }

    // No-op for sockets (no explicit flushing needed)
    void flush() {}

    // Send a string with a newline
    void println(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

    // Send a string
    void print(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }


};

#endif