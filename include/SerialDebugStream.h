#pragma once

#include <HardwareSerial.h>

#include "IDebugStream.h"

class SerialDebugStream : public IDebugStream {
public:
    SerialDebugStream(HardwareSerial& serialPort) :
        serialPort(serialPort) {
    };

    virtual size_t print(const char *message) {
        auto ret = this->serialPort.print(message);
        this->serialPort.flush();
        return ret;
    };

    size_t println(void)
    {
        return this->print("\r\n");
    }

    virtual size_t print(const Printable& x)
    {
        auto ret = x.printTo(serialPort);
        this->serialPort.flush();
        return ret;
    }

    virtual size_t println(const char message[]) {
        auto ret = this->serialPort.println(message);
        this->serialPort.flush();
        return ret;
    };

    virtual size_t println(const Printable& x)
    {
        size_t n = this->print(x);
        n += this->println();
        return n;
    }

    virtual size_t printf(const char *format, ...) {
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        size_t ret = this->serialPort.print(buffer);
        this->serialPort.flush();
        va_end(args);

        return ret;
    };

private:
    HardwareSerial& serialPort;
};
