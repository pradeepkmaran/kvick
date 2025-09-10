#include "KVickServer.hpp"
#include <sstream>


KVickServer::KVickServer(int port = 8080) : port_(port) {
    loadFromFile("/app/data/kvick_data.json");
    enableAutoPersist("/app/data/kvick_data.json", 30);
}

KVickServer::~KVickServer() {
    disableAutoPersist();
    saveToFile("/app/data/kvick_data.json");
    std::cout << "Data persisted on shutdown" << std::endl;
}

void KVickServer::start() {
    socket_t srv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (srv == (socket_t)-1)
        throw std::runtime_error("socket() failed");

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port_);

    if (::bind(srv, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        throw std::runtime_error("bind() failed");

    if (::listen(srv, 8) != 0)
        throw std::runtime_error("listen() failed");

    std::cout << "KVickServer listening on port " << port_ << '\n';

    for (;;) {
#ifdef USE_WS_POLL
        WSAPOLLFD pfd{ srv, POLLRDNORM, 0 };
        int n = WSAPoll(&pfd, 1, -1);
#else
        struct pollfd pfd{ srv, POLL_READ, 0 };
        int n = poll(&pfd, 1, -1);
#endif
        if (n > 0) {
            sockaddr_in cli;
            socklen_t len = sizeof(cli);
            socket_t cliSock = ::accept(srv, reinterpret_cast<sockaddr*>(&cli), &len);
            if (cliSock == (socket_t)-1)  continue;

            std::thread(&KVickServer::handleClient, this, cliSock).detach();
        }
    }
}

void KVickServer::handleClient(socket_t sock) {
    char buf[1024]{};
    int r = ::read(sock, buf, sizeof(buf)-1);
    if (r <= 0) { socket_close(sock); return; }

    std::string cmd(buf);
    std::istringstream iss(cmd);
    std::string op, key, val;
    iss >> op >> key;

    if (op == "GET") {
        try {
            auto v = get(key);
            std::ostringstream oss; 
            std::visit([&oss](auto&& x) {
                using T = std::decay_t<decltype(x)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << x;
                } else if constexpr (std::is_same_v<T, int>) {
                    oss << std::to_string(x);
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << std::to_string(x);
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (x ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<std::string>> ||
                                   std::is_same_v<T, std::vector<int>> ||
                                   std::is_same_v<T, std::vector<double>> ||
                                   std::is_same_v<T, std::vector<bool>>) {
                    oss << "[";
                    for (size_t i = 0; i < x.size(); ++i) {
                        if constexpr (std::is_same_v<T, std::vector<bool>>) {
                            oss << (x[i] ? "true" : "false");
                        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                            std::is_same_v<T, std::vector<double>>) {
                            oss << std::to_string(x[i]);
                        } else {
                            oss << x[i];
                        }
                        if (i + 1 < x.size()) oss << ", ";
                    }
                    oss << "]";
                }
            }, v);

            std::string out = oss.str();
            ::write(sock, out.c_str(), out.size());
        } catch (const std::exception& e) {
            std::string err = "ERR " + std::string(e.what());
            ::write(sock, err.c_str(), err.size());
        }
    } else if (op == "SET") {
        std::getline(iss, val);
        if (!val.empty() && val.front() == ' ') val.erase(0,1);
        set(key, val);
        const char ok[] = "OK";
        ::write(sock, ok, sizeof(ok)-1);
    }
    socket_close(sock);
}

