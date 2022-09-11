//
// Created by winfidonarleyan on 10.09.22.
//

#include "HttpServerMgr.h"
#include "AsyncCallbackMgr.h"
#include "Config.h"
#include "Log.h"
#include "ServerMgr.h"
#include "ImageMgr.h"
#include "StopWatch.h"
#include <httplib.h>

HttpServerMgr* HttpServerMgr::instance()
{
    static HttpServerMgr instance;
    return &instance;
}

void HttpServerMgr::Initialize()
{
    // Make new httplib::Server instance
    _server = std::make_unique<httplib::Server>();

    // Set max size for files from config
    auto payLoadMaxLength = sConfigMgr->GetOption<uint32>("PayloadLengthMax", 10);
    _server->set_payload_max_length(1024 * 1024 * payLoadMaxLength);

    // Added default post functions implementations
    AddDefaultPosts();

    // Run http server
    sAsyncCallbackMgr->AddAsyncCallback([this]()
    {
        // Get configs option
        auto binIP = sConfigMgr->GetOption<std::string>("BindIP", "0.0.0.0");
        auto bindPort = sConfigMgr->GetOption<uint16>("BindPort", 8080);

        LOG_INFO("server", "Network thread starting. IP: {}. Port: {}", binIP, bindPort);

        // Try bind socket
        if (!_server->listen(binIP, bindPort))
        {
            LOG_ERROR("http", "Failed to initialize network. IP: {}. Port: {}", binIP, bindPort);
            ServerMgr::StopNow(ERROR_EXIT_CODE);
        }

        LOG_INFO("http", "Stop http server");
    });
}

void HttpServerMgr::Stop()
{
    // Stop http server and clear memory
    _server->stop();
    _server.reset();
}

void HttpServerMgr::AddDefaultPosts()
{
    _server->Post("/", [&](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader)
    {
        if (req.is_multipart_form_data())
        {
             LOG_ERROR("http", "> Multiple files don't support. Skip");
             return;
        }

        StopWatch sw;
        std::string binaryData;

        content_reader([&binaryData](const char *data, size_t data_length)
        {
            binaryData.append(data, data_length);
            return true;
        });

        LOG_DEBUG("http", "> End data read");
        std::string mirrorImageBinary;

        if (!sImageMgr->GetBinaryMirrorImage(binaryData, mirrorImageBinary))
            return; // Stop job if failed get binary data for image

        // Start send data to client
        res.set_content(mirrorImageBinary, "application/octet-stream");

        LOG_INFO("http", "> End send binary data for client. Elapsed: {}", sw);
     });
}