#pragma once

#include <string>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class EngineProcess {
public:
    explicit EngineProcess(const std::string& enginePath);
    ~EngineProcess();

    EngineProcess(const EngineProcess&) = delete;
    EngineProcess& operator=(const EngineProcess&) = delete;

    void sendCommand(const std::string& command);

    std::vector<std::string> takeLines();

private:
    HANDLE processHandle_ = nullptr;
    HANDLE writeStdin_ = nullptr;
    HANDLE readStdout_ = nullptr;

    std::thread readerThread_;
    std::mutex lineMutex_;
    std::vector<std::string> pendingLines_;
    std::atomic<bool> running_{false};

    void readerLoop();
};