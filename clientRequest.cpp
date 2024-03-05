#include <client.h>

int main(int argc, char* argv[]) {
    try {
        TCP::client client_instance;
        client_instance.init(argc, argv);
        client_instance.run();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}