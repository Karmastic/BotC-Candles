#include <algorithm>

#include "AppTasks.h"

static AppTasks *instance = nullptr;

AppTasks::AppTasks(IDebugStream *debugOutput) : debugOutput(debugOutput)
{
    instance = this;
}

AppTasks *AppTasks::Instance()
{
    return instance;
}

ITask *AppTasks::LookupTask(const char *taskName)
{
    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it)
    {
        if (0 == strcmp((*it)->Name(), taskName))
        {
            return *it;
        }
    }

    return nullptr;
}

void AppTasks::AddTask(ITask *task)
{
    auto taskName = task->Name();
    this->debugOutput->printf("[AppTasks] Adding Task %s...\n", taskName);

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it)
    {
        if (0 == strcmp((*it)->Name(), taskName))
        {
            this->debugOutput->printf("!!!! [AppTasks] Task with name %s already exists.  Not adding. !!!!\n", taskName);
            return;
        }
    }

    this->tasks.push_back(task);
}

ITask *AppTasks::RemoveTask(const char *taskName)
{
    this->debugOutput->printf("[AppTasks.RemoveTask] %s...\n", taskName);

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it)
    {
        ITask *task = *it;
        if (0 == strcmp(task->Name(), taskName))
        {
            if (task->GetState() != ITask::TaskState::Terminal)
            {
                this->DeactivateTask(taskName);
                this->debugOutput->printf("[AppTasks] Marking Task %s as Terminal\n", taskName);
                task->SetState(ITask::TaskState::Terminal);
            }

            return task;
        }
    }

    return nullptr;
}

void AppTasks::ActivateTask(const char *taskName)
{
    this->debugOutput->printf("[AppTasks.ActivateTask] %s...\n", taskName);

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it)
    {
        ITask *task = *it;
        if (0 == strcmp(task->Name(), taskName))
        {
            if (task->GetState() == ITask::TaskState::Active)
            {
                this->debugOutput->printf("!!!! [AppTasks] Task %s already active - skipping. !!!!\n", taskName);
            }
            else if (task->GetState() == ITask::TaskState::Inactive)
            {
                this->debugOutput->printf("[AppTasks] Activating Task %s\n", taskName);

                // Run setup for task immediately upon activation
                // Run before changing state so we don't call its loop() method before setup().
                task->setup();

                task->SetState(ITask::TaskState::Active);
            }

            return;
        }
    }

    this->debugOutput->printf("!!!! [AppTasks] Can't activate unknown Task %s. !!!!\n", taskName);
}

void AppTasks::DeactivateTask(const char *taskName)
{
    this->debugOutput->printf("[AppTasks.DeactivateTask] %s...\n", taskName);

    for (auto it = this->tasks.begin(); it != this->tasks.end(); ++it)
    {
        ITask *task = *it;
        if (0 == strcmp(task->Name(), taskName))
        {
            if (task->GetState() == ITask::TaskState::Active)
            {
                this->debugOutput->printf("[AppTasks] Deactivating Task %s\n", taskName);
                task->SetState(ITask::TaskState::Inactive);
            }
            break;
        }
    }
}

void AppTasks::ProcessLoop()
{
    for (auto it = this->tasks.begin(); it != this->tasks.end();)
    {
        ITask *task = *it;
        if (task->GetState() == ITask::TaskState::Active)
        {
            task->loop();
        }

        if (task->GetState() == ITask::TaskState::Terminal)
        {
            this->debugOutput->printf("[AppTasks] Removing Terminal Task %s\n", task->Name());
            it = this->tasks.erase(it);
            task->onRemove();
        }
        else
        {
            ++it;
        }
    }
}
