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

    // Send row, col, val, optional color
    void sendCell(int row, int col, int val, const std::string& color="") {
        std::ostringstream oss;
        oss << row << " " << col << " " << val;
        if(!color.empty()) oss << " " << color;
        oss << "\n";
        std::string s = oss.str();
        ::send(sock, s.c_str(), s.size(), 0);
    }
};

int main() {
    NetStream server("127.0.0.1", 12345);

    // Example: fill some cells with custom colors
    server.sendCell(0, 0, 7, "#ff0000"); // red
    server.sendCell(1, 1, 5, "#00ff00"); // green
    server.sendCell(6, 6, 9, "#0000ff"); // blue
    server.sendCell(3, 3, 8);            // default color
    server.sendCell(3, 4, 8);            // default color
    server.sendCell(3, 5, 8);            // default color
    server.sendCell(0, 5, 8);            // default color
    return 0;
}
