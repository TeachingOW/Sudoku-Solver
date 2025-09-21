#include <iostream>
#include <sstream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

class NetStream {
    int sock;
public:
    NetStream(const std::string& host, int port) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) { perror("socket"); exit(1); }

        sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &server.sin_addr) <= 0) {
            perror("inet_pton"); exit(1);
        }
        if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
            perror("connect"); exit(1);
        }
    }

    ~NetStream() {
        close(sock);
    }

    template<typename T>
    NetStream& operator<<(const T& data) {
        std::ostringstream oss;
        oss << data;
        std::string s = oss.str();
        ::send(sock, s.c_str(), s.size(), 0);
        ::send(sock, " ", 1, 0);
        return *this;
    }

    void endmsg() {
        ::send(sock, "\n", 1, 0);
    }
};

int main() {
    NetStream server("127.0.0.1", 12345);

    // Example: fill cell row 0, col 0 with 7
    server << 0 << 0 << 7;
    server.endmsg();

    // Example: fill cell row 4, col 5 with 9
    server << 4 << 4 << 9;
    server.endmsg();

    server << 3 << 3 << 8;
    server.endmsg();

    
    return 0;
}
