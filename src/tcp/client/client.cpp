#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <future>
#include <thread>

#include "timeGet.h"
#include "client.h"

namespace TCP {
    client::~client() {
        close(sock);
    }
    void client::init(int argc, char** argv) {
        if (argc != 4) {
            std::cerr << "Use the format: ./client\"username\" \"port\" \"timeout\"\n";
            init_flag = false;
            return;
        }
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            std::cout << "Failed to create socket\n";
            init_flag = false;
            return;
        }


        struct sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(std::stoi(argv[2]));
        inet_pton(AF_INET, ip_address, &hint.sin_addr);


        int connect_res = connect(sock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));
        if (connect_res == -1)
        {
            std::cout << "Failed to connect to the server\n";
            init_flag = false;
            return;
        }

        name = argv[1];
        timeout = std::stoi(argv[3]);

        init_flag = true;
    }

    void client::run()
    {
        if (!init_flag)
        {
            return;
        }

        char buffer[buffer_size];
        int send_res{};
        std::string user_input(buffer_size, '\0');
        std::future<void> ft;

        while (true)
        {
            user_input = '[';
            user_input += get_timeTCP();
            user_input += "] ";
            user_input += name;
            user_input += "  ";
            user_input += std::to_string(timeout);

            ft = std::async(std::launch::async, [&]()
                { send_res = send(sock, user_input.c_str(), sizeof(user_input) + 1, 0); });

            std::this_thread::sleep_for(std::chrono::milliseconds(timeout * 1000));

            if (send_res == -1 || send_res == 0)
            {
                break;
            }
        }
    }

    bool client::openConnection() {
        if (!init_flag) {
            return false;
        }



        return true;
    }

    void client::closeConnection() {
        if (init_flag) {
            close(sock);
            init_flag = false;
        }
    }

    bool client::sendData(const std::string& data) {
        if (!init_flag) {
            return false;
        }

        int send_res = send(sock, data.c_str(), data.size(), 0);
        if (send_res == -1) {
            std::cerr << "Error sending data\n";
            return false;
        }

        return true;
    }

    std::string client::receiveData() {
        if (!init_flag) {
            return "";
        }

        char buffer[buffer_size];
        memset(buffer, 0, buffer_size);

        int bytes_received = recv(sock, buffer, buffer_size, 0);
        if (bytes_received == -1) {
            std::cerr << "Error receiving data\n";
            return "";
        }

        return std::string(buffer, bytes_received);
    }

} // TCP

