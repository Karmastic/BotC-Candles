#pragma once

class ITask {
public:
    virtual const char *Name() = 0;

    virtual void setup() = 0;

    // Run loop
    virtual void loop() = 0;

    virtual void onRemove() = 0;

    enum class TaskState {
        Inactive,
        Active,
        Terminal
    };

    virtual TaskState GetState() = 0;
    virtual bool SetState(TaskState state) = 0;
};
