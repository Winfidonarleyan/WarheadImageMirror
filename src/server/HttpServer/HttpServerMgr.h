//
// Created by winfidonarleyan on 10.09.22.
//

#ifndef WARHEADIMAGEMIRROR_HTTPSERVER_H
#define WARHEADIMAGEMIRROR_HTTPSERVER_H

#include "Define.h"
#include <memory>

namespace httplib
{
    class Server;
}

class WH_SERVER_API HttpServerMgr
{
public:
    static HttpServerMgr* instance();

    void Initialize();
    void Stop();
    void AddDefaultPosts();

private:
    std::unique_ptr<httplib::Server> _server;

    HttpServerMgr() = default;
    ~HttpServerMgr() = default;
    HttpServerMgr(HttpServerMgr const&) = delete;
    HttpServerMgr(HttpServerMgr&&) = delete;
    HttpServerMgr& operator=(HttpServerMgr const&) = delete;
    HttpServerMgr& operator=(HttpServerMgr&&) = delete;
};

#define sHttpServerMgr HttpServerMgr::instance()

#endif //WARHEADIMAGEMIRROR_HTTPSERVER_H
