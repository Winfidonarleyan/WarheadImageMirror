//
// Created by winfidonarleyan on 10.09.22.
//

#include "ServerMgr.h"
#include "AsyncCallbackMgr.h"

std::atomic<bool> ServerMgr::_stopEvent = false;
uint8 ServerMgr::_exitCode = SHUTDOWN_EXIT_CODE;

/*static*/ ServerMgr* ServerMgr::instance()
{
    static ServerMgr instance;
    return &instance;
}

void ServerMgr::Update(Milliseconds /*diff*/)
{
    sAsyncCallbackMgr->ProcessReadyCallbacks();
}