#include <HardwareSerial.h>

#include "Registry.h"

static Registry instance;

Registry::Registry()
{
}

bool Registry::AddInterface(const char *name, void *interface)
{
    Serial.printf("Registry::AddInterface %s\n", name);

    if (instance.GetInterface(name) != nullptr)
    {
        Serial.printf(" >> %s already registered\n", name);
        return false;
    }

    instance.registry[name] = interface;
    Serial.printf(" >> %s registered\n", name);

    return true;
}

bool Registry::RemoveInterface(const char *name)
{
    return instance.registry.erase(name);
}

void *Registry::GetInterface(const char *name)
{
    auto pair = instance.registry.find(name);
    return pair == instance.registry.end() ? nullptr : pair->second;
}
