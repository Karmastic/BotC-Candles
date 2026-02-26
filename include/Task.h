#pragma once

#include "ITask.h"
#include "IDebugStream.h"

class Task : public ITask {
public:
    Task(IDebugStream *debugOutput)
        : debugOutput(debugOutput) {
    }

    virtual ~Task() {
    }

    virtual void onRemove() {
        this->debugOutput->printf("Deleting Task %s\n", this->Name());
        delete this;
    }

    virtual TaskState GetState() { return this->state; }
    virtual bool SetState(TaskState state) {
        bool ret = (this->state != state);
        this->state = state;

        return ret;
    }

protected:
    IDebugStream *debugOutput;
    TaskState state = TaskState::Inactive;
};