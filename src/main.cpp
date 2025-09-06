#include "KVickServer.hpp"

int main() {
    try {
        KVickServer server(8080);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
