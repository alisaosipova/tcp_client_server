#pragma once
#include "messageLogger.h"
#include <cstdint>
#include <atomic>
#include <stdexcept>


namespace TCP
{
    class server
    {
    private:
        const char* connection_address{ "0.0.0.0" };
        int endpoint{ 0 };
        int port{ 0 };
        uint16_t buffer_size{ 4096 };
        std::atomic<bool> init_flag{ false };
        std::atomic<bool> stop_flag{ false };
        std::atomic<uint16_t> connection_count{ 0 };

    private:
        messageLogger mlogger;

    private:
        inline uint16_t has_connections();
        void accept_connection();
        void concrete_connection(int client_socket);

    public:
        server(const char* file_name = "log.txt");
        ~server();
        void init(int argc, char* argv[]);
        void run();
        void stopServer();
    };

} // tcp