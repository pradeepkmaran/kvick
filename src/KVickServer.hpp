#ifndef KVICK_SERVER_HPP
#define KVICK_SERVER_HPP

#include "KVick.hpp"
#include <thread>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sstream>

#ifdef _WIN32    
    #define NOMINMAX
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using socket_t = SOCKET;
    inline void socket_close(socket_t s) { closesocket(s); }
    inline int socket_errno()           { return WSAGetLastError(); }
    constexpr int POLL_READ  = POLLRDNORM;
    constexpr int POLL_ERROR = POLLERR;
    #define USE_WS_POLL                  
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <poll.h>
    using socket_t = int;
    inline void socket_close(socket_t s) { ::close(s); }
    inline int socket_errno()           { return errno; }
    constexpr int POLL_READ  = POLLIN;
    constexpr int POLL_ERROR = POLLERR;
#endif

class KVickServer : public KVick {
public:
    explicit KVickServer(int port = 8080) : port_(port) {}
    void start();

private:
    int     port_;
    void    handleClient(socket_t client);
};

#endif
