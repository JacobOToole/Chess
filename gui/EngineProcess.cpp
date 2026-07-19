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
    std::cout << "Engine launched: " << enginePath << "\n";
}

EngineProcess::~EngineProcess() {
    if (processHandle_) {
        TerminateProcess(processHandle_, 0);
        WaitForSingleObject(processHandle_, 1000);
        CloseHandle(processHandle_);
    }
    if (writeStdin_)  CloseHandle(writeStdin_);
    if (readStdout_)  CloseHandle(readStdout_);
    std::cout << "Engine terminated\n";
}
