#pragma once

#include <stddef.h>

#include <Print.h>

class IDebugStream {
public:
    virtual size_t print(const char *message) = 0;
    virtual size_t print(const Printable& x) = 0;
    virtual size_t println(const char message[]) = 0;
    virtual size_t println(const Printable& x) = 0;
    virtual size_t printf(const char * format, ...) = 0;
};
