//
// Created by winfidonarleyan on 10.09.22.
//

#ifndef WARHEADIMAGEMIRROR_SERVER_H
#define WARHEADIMAGEMIRROR_SERVER_H

#include "Define.h"
#include "Duration.h"
#include <atomic>

enum WH_SERVER_API ShutdownExitCode : uint8
{
    SHUTDOWN_EXIT_CODE,
    ERROR_EXIT_CODE
};

class WH_SERVER_API ServerMgr
{
public:
    static ServerMgr* instance();

    void Update(Milliseconds diff);

    static uint8 GetExitCode() { return _exitCode; }
    static void StopNow(uint8 exitcode) { _stopEvent = true; _exitCode = exitcode; }
    static bool IsStopped() { return _stopEvent; }

private:
    static std::atomic<bool> _stopEvent;
    static uint8 _exitCode;

    ServerMgr() = default;
    ~ServerMgr() = default;
    ServerMgr(ServerMgr const&) = delete;
    ServerMgr(ServerMgr&&) = delete;
    ServerMgr& operator=(ServerMgr const&) = delete;
    ServerMgr& operator=(ServerMgr&&) = delete;
};

#define sServerMgr ServerMgr::instance()

#endif //WARHEADIMAGEMIRROR_SERVER_H
