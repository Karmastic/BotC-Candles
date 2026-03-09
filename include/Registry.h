#pragma once

#include <map>

#include "Interfaces.h"

class Registry
{
public:
    Registry();

    static bool AddInterface(const char *name, void *interface);
    static bool RemoveInterface(const char *name);
    static void *GetInterface(const char *name);

private:
    std::map<const char *, void *> registry;
};
