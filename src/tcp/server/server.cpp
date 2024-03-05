#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>

#include "server.h"

namespace TCP
{
    namespace context
    {
        namespace mutexes
        {
            std::mutex count_connections;
            std::mutex stop_server;
        }
    }

    inline uint16_t server::has_connections()
    {
        return connection_count;
    }

    void server::accept_connection()
    {
        try {

            while (true)
            {
                struct sockaddr_in client;
                socklen_t clientSize = sizeof(client);
                int client_socket = accept(endpoint, reinterpret_cast<sockaddr*>(&client), &clientSize);

                if (client_socket > 0)
                {

                    std::thread th(&server::concrete_connection, this, client_socket);
                    th.detach();

                    context::mutexes::count_connections.lock();
                    ++connection_count;
                    context::mutexes::count_connections.unlock();
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Connection acceptance error: " << e.what() << '\n';
        }
    }

    void server::concrete_connection(int client_socket)
    {
        try {
            char buffer[buffer_size];
            int bytes_recv{ 0 };
            bool close_connection_flag{ false };
            std::future<void> ft;
            std::chrono::milliseconds span{ 250 };

            while (true)
            {

                memset(buffer, 0, buffer_size);


                ft = std::async([&]()
                    { bytes_recv = recv(client_socket, buffer, buffer_size, 0); });


                while (ft.wait_for(span) == std::future_status::timeout)
                {
                    context::mutexes::stop_server.lock();
                    if (stop_flag)
                    {
                        close_connection_flag = true;
                    }
                    context::mutexes::stop_server.unlock();
                }

                // Checks a bytes count of the "recv" and handle client message
                if (bytes_recv == -1 || bytes_recv == 0 || close_connection_flag)
                {
                    break;
                }
                else
                {
                    ft = std::async([&]()
                        { mlogger.saveMessage(buffer); });
                    ft.wait();
                }
            }
            close(client_socket);

            if (close_connection_flag)
            {
                std::cout << ". " << std::flush;
            }

            context::mutexes::count_connections.lock();
            --connection_count;
            context::mutexes::count_connections.unlock();
        }
        catch (const std::exception& e) {
            std::cerr << "Error in processing the client's message:" << e.what() << '\n';
        }
    }

    server::server(const char* file_name) : mlogger{ file_name }
    {
    }

    server::~server()
    {
        if (endpoint > 0)
        {
            close(endpoint);
        }
    }

    void server::init(int argc, char* argv[])
    {

        if (argc != 2)
        {
            throw std::invalid_argument("Incorrect argument format. Use: ./server 3000");
        }


        endpoint = socket(AF_INET, SOCK_STREAM, 0);
        if (endpoint == -1)
        {
            throw std::runtime_error("Failed to create a socket");
        }


        port = std::stoi(argv[1]);

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, connection_address, &hint.sin_addr);


        if (bind(endpoint, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1)
        {
            throw std::runtime_error("Failed to bind to IP/port");
        }

        init_flag = true;
    }

    void server::run()
    {
        if (!init_flag)
        {
            std::cerr << "The server is not initialized\n";
            return;
        }

        std::cout << "The server is running. Current port:" << port << '\n';


        if (listen(endpoint, SOMAXCONN) == -1)
        {
            std::cerr << "Couldn't start listening\n";
            return;
        }

        std::thread th1(&server::accept_connection, this);
        th1.detach();

        while (true)
        {
            std::string command;
            std::cout << "Enter \"exit\" to stop the server:";
            std::cin >> command;

            if (command == "exit"){
            {
                std::lock_guard<std::mutex> lock(context::mutexes::stop_server);
                stop_flag = true;
            }

                while (has_connections())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
                close(endpoint);
            }
        }
        std::cout << "\nThe server is stopped\n";
    }

    void server::stopServer() {
        context::mutexes::stop_server.lock();
        stop_flag = true;
        context::mutexes::stop_server.unlock();
    }

} // TCP
