#pragma once

#include <string>
#include <windows.h>

class EngineProcess {
public:
    explicit EngineProcess(const std::string& enginePath);
    ~EngineProcess();

    EngineProcess(const EngineProcess&) = delete;
    EngineProcess& operator=(const EngineProcess&) = delete;

private:
    HANDLE processHandle_ = nullptr;
    HANDLE writeStdin_ = nullptr;
    HANDLE readStdout_ = nullptr;
};