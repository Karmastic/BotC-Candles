#include <vector>

#include "IDebugStream.h"
#include "ITask.h"

class AppTasks {
public:
    AppTasks(IDebugStream *debugOutput);

    static AppTasks *Instance();

    ITask *LookupTask(const char *taskName);

    void AddTask(ITask *task);
    ITask *RemoveTask(const char *taskName);

    void ActivateTask(const char *taskName);
    void DeactivateTask(const char *taskName);

    void ProcessLoop();

    IDebugStream *GetDebugOutput() { return this->debugOutput; }

private:
    std::vector<ITask*> tasks;

    IDebugStream *debugOutput;
};
