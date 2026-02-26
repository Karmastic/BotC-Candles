#include "AppTasks.h"

static AppTasks *instance = nullptr;

AppTasks::AppTasks(IDebugStream *debugOutput) : debugOutput(debugOutput) {
    instance = this;
}

AppTasks *AppTasks::Instance() {
    return instance;
}

ITask *AppTasks::LookupTask(const char *taskName) {
    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            return *it;
        }
    }

    return nullptr;
}

void AppTasks::AddTask(ITask *task) {
    auto taskName = task->Name();
    this->debugOutput->printf("[AppTasks] Adding Task %s...\n", taskName);

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            this->debugOutput->printf("!!!! [AppTasks] Task with name %s already exists.  Not adding. !!!!\n", taskName);
            return;
        }
    }

    this->tasks.push_back(task);
}

ITask *AppTasks::RemoveTask(const char *taskName) {
    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            this->DeactivateTask(taskName);
            this->tasks.erase(it);

            return *it;

        }
    }

    return nullptr;
}

void AppTasks::ActivateTask(const char *taskName) {
    for (auto it = this->activeTasks.begin(); it != this->activeTasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            this->debugOutput->printf("!!!! [AppTasks] Task %s already active - skipping. !!!!\n", taskName);

            return;
        }
    }

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            this->activeTasks.push_back(*it);

            // Run setup for task immediately upon activation
            (*it)->setup();

            return;
        }
    }

    this->debugOutput->printf("!!!! [AppTasks] Can't activate unknown Task %s. !!!!\n", taskName);
}

void AppTasks::DeactivateTask(const char *taskName) {
    for (auto it = this->activeTasks.begin(); it != this->activeTasks.end(); ++it) {
        if ((*it)->Name() == taskName) {
            this->debugOutput->printf("[AppTasks] Deactivating Task %s\n", (*it)->Name());
            this->activeTasks.erase(it);

            break;
        }
    }
}

void AppTasks::ProcessLoop() {
    std::vector<ITask*> completedTasks;

    for (auto it = this->activeTasks.begin(); it != this->activeTasks.end(); ++it) {
        if ((*it)->loop()) {
            this->debugOutput->printf("[AppTasks] Task %s completed - pending for deactivation.\n", (*it)->Name());
            completedTasks.push_back(*it);
        }
    }

    for (auto it = completedTasks.begin(); it != completedTasks.end(); ++it) {
        this->DeactivateTask((*it)->Name());
    }
}
