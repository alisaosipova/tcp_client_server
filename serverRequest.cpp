#include <server.h>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        TCP::server server_instance;
        server_instance.init(argc, argv);
        server_instance.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}