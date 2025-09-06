#include "KVickServer.hpp"
#include <sstream>

#ifdef _WIN32
    static void winsock_init() {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            throw std::runtime_error("WSAStartup failed");
    }
#endif

void KVickServer::start() {
#ifdef _WIN32
    winsock_init();
#endif

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

    // event loop
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
#ifdef _WIN32
            int len = sizeof(cli);
#else
            socklen_t len = sizeof(cli);
#endif
            socket_t cliSock = ::accept(srv, reinterpret_cast<sockaddr*>(&cli), &len);
            if (cliSock == (socket_t)-1)  continue;

            std::thread(&KVickServer::handleClient, this, cliSock).detach();
        }
    }
}

void KVickServer::handleClient(socket_t sock) {
    char buf[1024]{};
#ifdef _WIN32
    int r = ::recv(sock, buf, sizeof(buf)-1, 0);
#else
    int r = ::read(sock, buf, sizeof(buf)-1);
#endif
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
#ifdef _WIN32
            ::send(sock, out.c_str(), (int)out.size(), 0);
#else
            ::write(sock, out.c_str(), out.size());
#endif
        } catch (const std::exception& e) {
            std::string err = "ERR " + std::string(e.what());
#ifdef _WIN32
            ::send(sock, err.c_str(), (int)err.size(), 0);
#else
            ::write(sock, err.c_str(), err.size());
#endif
        }
    } else if (op == "SET") {
        std::getline(iss, val);
        if (!val.empty() && val.front() == ' ') val.erase(0,1);
        set(key, val);
        const char ok[] = "OK";
#ifdef _WIN32
        ::send(sock, ok, sizeof(ok)-1, 0);
#else
        ::write(sock, ok, sizeof(ok)-1);
#endif
    }
    socket_close(sock);
}
