#ifndef KVICK_SERVER_HPP
#define KVICK_SERVER_HPP

#include "KVick.hpp"
#include <thread>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
using socket_t = int;
inline void socket_close(socket_t s) { ::close(s); }
inline int socket_errno()           { return errno; }
constexpr int POLL_READ  = POLLIN;
constexpr int POLL_ERROR = POLLERR;

class KVickServer : public KVick {
public:
    explicit KVickServer(int port = 8080) : port_(port) {}
    void start();

private:
    int port_;
    void handleClient(socket_t client);
};

#endif
