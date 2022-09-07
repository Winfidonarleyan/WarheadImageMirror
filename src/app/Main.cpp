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
#include "Errors.h"
#include "IoContext.h"
#include "Log.h"
#include <boost/asio/signal_set.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <csignal>
#include <filesystem>
#include <iostream>

#ifndef _WARHEAD_SERVER_CONFIG
#define _WARHEAD_SERVER_CONFIG "WarheadImageMirror.conf"
#endif

using namespace boost::program_options;
namespace fs = std::filesystem;

void SignalHandler(std::weak_ptr<Warhead::Asio::IoContext> ioContextRef, boost::system::error_code const& error, int /*signalNumber*/);
variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile);

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

    LOG_INFO("server.authserver", "> Using configuration file:       {}", sConfigMgr->GetFilename());
    LOG_INFO("server.authserver", "> Using Boost version:            {}.{}.{}", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
    LOG_INFO("server.authserver", "> Using logs directory:           {}", sLog->GetLogsDir());

    std::shared_ptr<Warhead::Asio::IoContext> ioContext = std::make_shared<Warhead::Asio::IoContext>();

    // Start the listening port (acceptor) for auth connections
    int32 port = sConfigMgr->GetOption<int32>("RealmServerPort", 3724);
    if (port < 0 || port > 0xFFFF)
    {
        LOG_ERROR("server.authserver", "Specified port out of allowed range (1-65535)");
        return 1;
    }

    std::string bindIp = sConfigMgr->GetOption<std::string>("BindIP", "0.0.0.0");

    /*if (!sAuthSocketMgr.StartNetwork(*ioContext, bindIp, port))
    {
        LOG_ERROR("server.authserver", "Failed to initialize network");
        return 1;
    }*/

    // std::shared_ptr<void> sAuthSocketMgrHandle(nullptr, [](void*) { sAuthSocketMgr.StopNetwork(); });

    // Set signal handlers
    boost::asio::signal_set signals(*ioContext, SIGINT, SIGTERM);
#if WARHEAD_PLATFORM == WARHEAD_PLATFORM_WINDOWS
    signals.add(SIGBREAK);
#endif
    signals.async_wait(std::bind(&SignalHandler, std::weak_ptr<Warhead::Asio::IoContext>(ioContext), std::placeholders::_1, std::placeholders::_2));

    // Start the io service worker loop
    ioContext->run();

    LOG_INFO("server.authserver", "Halting process...");

    signals.cancel();

    return 0;
}

void SignalHandler(std::weak_ptr<Warhead::Asio::IoContext> ioContextRef, boost::system::error_code const& error, int /*signalNumber*/)
{
    if (!error)
        if (std::shared_ptr<Warhead::Asio::IoContext> ioContext = ioContextRef.lock())
            ioContext->stop();
}

variables_map GetConsoleArguments(int argc, char** argv, fs::path& configFile)
{
    options_description all("Allowed options");
    all.add_options()
        ("help,h", "print usage message")
        ("version,v", "print version build info")
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
