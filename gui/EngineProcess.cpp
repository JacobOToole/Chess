//
// Created by jacob on 7/20/2026.
//

#include "EngineProcess.h"
#include <iostream>
#include <stdexcept>

EngineProcess::EngineProcess(const std::string& enginePath) {
    // Set up security attributes so pipe handles can be inherited by the child.
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    // Create pipe for the engine's stdin: we write, engine reads.
    HANDLE readStdin;
    if (!CreatePipe(&readStdin, &writeStdin_, &sa, 0)) {
        throw std::runtime_error("Failed to create stdin pipe");
    }
    SetHandleInformation(writeStdin_, HANDLE_FLAG_INHERIT, 0);

    // Create pipe for the engine's stdout: engine writes, we read.
    HANDLE writeStdout;
    if (!CreatePipe(&readStdout_, &writeStdout, &sa, 0)) {
        throw std::runtime_error("Failed to create stdout pipe");
    }
    SetHandleInformation(readStdout_, HANDLE_FLAG_INHERIT, 0);

    // Configure the child process to use our pipes as its stdin/stdout.
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = readStdin;
    si.hStdOutput = writeStdout;
    si.hStdError = writeStdout;

    PROCESS_INFORMATION pi{};

    std::string mutablePath = enginePath;
    if (!CreateProcessA(nullptr, mutablePath.data(),
                        nullptr, nullptr, TRUE, 0,
                        nullptr, nullptr, &si, &pi)) {
        throw std::runtime_error("Failed to launch engine: " + enginePath);
    }

    // We don't need the child's ends of the pipes anymore.
    CloseHandle(readStdin);
    CloseHandle(writeStdout);
    CloseHandle(pi.hThread);

    processHandle_ = pi.hProcess;

    running_ = true;
    readerThread_ = std::thread(&EngineProcess::readerLoop, this);

    std::cout << "Engine launched: " << enginePath << "\n";
}

EngineProcess::~EngineProcess() {
    running_ = false;
    if (readerThread_.joinable()) {
        // Closing the pipe unblocks the ReadFile call in the reader thread.
        if (readStdout_) { CloseHandle(readStdout_); readStdout_ = nullptr; }
        readerThread_.join();
    }
    if (processHandle_) {
        TerminateProcess(processHandle_, 0);
        WaitForSingleObject(processHandle_, 1000);
        CloseHandle(processHandle_);
    }
    if (writeStdin_)  CloseHandle(writeStdin_);
    std::cout << "Engine terminated\n";
}

void EngineProcess::sendCommand(const std::string& command) {
    std::string line = command + "\n";
    DWORD written = 0;
    if (!WriteFile(writeStdin_, line.data(),
                   static_cast<DWORD>(line.size()),
                   &written, nullptr)) {
        std::cerr << "Failed to send command: " << command << "\n";
    }
}

void EngineProcess::readerLoop() {
    std::string buffer;
    char ch;
    DWORD bytesRead = 0;

    while (running_) {
        BOOL success = ReadFile(readStdout_, &ch, 1, &bytesRead, nullptr);
        if (!success || bytesRead == 0) break;   // pipe closed or error

        if (ch == '\n') {
            std::lock_guard<std::mutex> lock(lineMutex_);
            pendingLines_.push_back(buffer);
            buffer.clear();
        } else if (ch != '\r') {  // ignore carriage returns
            buffer += ch;
        }
    }
}

std::vector<std::string> EngineProcess::takeLines() {
    std::lock_guard<std::mutex> lock(lineMutex_);
    std::vector<std::string> lines = std::move(pendingLines_);
    pendingLines_.clear();
    return lines;
}
