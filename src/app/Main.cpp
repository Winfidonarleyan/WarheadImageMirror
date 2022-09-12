/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"
#include "Timer.h"
#include "Errors.h"
#include "IoContext.h"
#include "HttpServerMgr.h"
#include "Log.h"
#include "ServerMgr.h"
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>

#ifndef _WARHEAD_SERVER_CONFIG
#define _WARHEAD_SERVER_CONFIG "WarheadImageMirror.conf"
#endif

using namespace boost::program_options;
namespace fs = std::filesystem;

variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile);
void ServerUpdateLoop();

/// Launch the server
int main(int argc, char** argv)
{
    signal(SIGABRT, &Warhead::AbortHandler);

    // Command line parsing
    auto configFile = fs::path(sConfigMgr->GetConfigPath() + std::string(_WARHEAD_SERVER_CONFIG));
    auto vm = GetConsoleArguments(argc, argv, configFile);

    // exit if help or version is enabled
    if (vm.count("help"))
        return 0;

    // Add file and args in config
    sConfigMgr->Configure(configFile.generic_string());

    if (!sConfigMgr->LoadAppConfigs())
        return 1;

    // Init logging
    sLog->Initialize();

    LOG_INFO("server", "> Using configuration file:       {}", sConfigMgr->GetFilename());
    LOG_INFO("server", "> Using Boost version:            {}.{}.{}", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
    LOG_INFO("server", "> Using logs directory:           {}", sLog->GetLogsDir());
    LOG_INFO("server", "");

    std::shared_ptr<Warhead::Asio::IoContext> ioContext = std::make_shared<Warhead::Asio::IoContext>();

    // Initialize http server
    sHttpServerMgr->Initialize();

    std::shared_ptr<void> sHttpServerMgrHandle(nullptr, [](void*) { sHttpServerMgr->Stop(); });

    // Set signal handlers
    boost::asio::signal_set signals(*ioContext, SIGINT, SIGTERM);
#if WARHEAD_PLATFORM == WARHEAD_PLATFORM_WINDOWS
    signals.add(SIGBREAK);
#endif
    signals.async_wait([](boost::system::error_code const& error, int signalNumber)
    {
        LOG_WARN("server", "> Caught signal {}", signalNumber);

        if (!error)
            ServerMgr::StopNow(SHUTDOWN_EXIT_CODE);
    });

    // Start the Boost based thread
    std::shared_ptr<std::thread> ioContextThread(new std::thread([ioContext](){ ioContext->run();} ), [ioContext](std::thread* del)
    {
        ioContext->stop();
        del->join();
        delete del;
    });

    ServerUpdateLoop();

    LOG_INFO("server", "Halting process...");

    signals.cancel();

    // 0 - normal shutdown
    // 1 - shutdown at error
    return ServerMgr::GetExitCode();
}

variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile)
{
    options_description all("Allowed options");
    all.add_options()
        ("help,h", "print usage message")
        ("config,c", value<fs::path>(&configFile)->default_value(fs::path(sConfigMgr->GetConfigPath() + std::string(_WARHEAD_SERVER_CONFIG))), "use <arg> as configuration file");

    variables_map variablesMap;

    try
    {
        store(command_line_parser(argc, argv).options(all).allow_unregistered().run(), variablesMap);
        notify(variablesMap);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << "\n";
    }

    if (variablesMap.count("help"))
        std::cout << all << "\n";

    return variablesMap;
}

void ServerUpdateLoop()
{
    auto realCurrTime = 0ms;
    auto realPrevTime = GetTimeMS();

    // While we have not ServerMgr::_stopEvent, update the server
    while (!ServerMgr::IsStopped())
    {
        realCurrTime = GetTimeMS();

        auto diff = GetMSTimeDiff(realPrevTime, realCurrTime);
        if (diff == 0ms)
        {
            // sleep until enough time passes that we can update all timers
            std::this_thread::sleep_for(1ms);
            continue;
        }

        sServerMgr->Update(diff);
        realPrevTime = realCurrTime;
    }
}