#include <iostream>
using namespace std;

#include <sstream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

class NetStream
{
    int sock;

public:
    NetStream(const std::string &host, int port)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            exit(1);
        }

        sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &server.sin_addr) <= 0)
        {
            perror("inet_pton");
            exit(1);
        }
        if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0)
        {
            perror("connect");
            exit(1);
        }
    }

    ~NetStream() { close(sock); }

    void sendCell(int row, int col, int val, const std::string &color)
    {
        std::ostringstream oss;
        oss << "(" << row << ", " << col << ", " << val << ", " << color << ")\n";
        std::string s = oss.str();
        ::send(sock, s.c_str(), s.size(), 0);
    }
};

void print_board(char board[9][9], int start = 0)
{
    for (int l = 0; l < start; l++)
        cout << "\t";
    for (int l = 0; l < 9; l++)
        cout << "-";
    cout << "\n";
    for (int i = 0; i < 9; i++)
    {
        for (int l = 0; l < start; l++)
            cout << "\t";
        for (int j = 0; j < 9; j++)
        {

            cout << board[i][j];
        }
        cout << "\n";
    }

    for (int l = 0; l < start; l++)
        cout << "\t";
    for (int l = 0; l < 9; l++)
        cout << "-";
    cout << "\n";
}

void send_board(NetStream &stream, char board[9][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {

            if (board[i][j] != ' ')
            {
                int v = board[i][j] - '0';
                stream.sendCell(i, j, v, "blue");
            }
        }
    }
}

bool check_row(char board[9][9], int i, char v)
{
    bool found = false;
    for (int j = 0; j < 9; j++)
    {
        if (board[i][j] == v)
        {
            found = true;

            break;
        }
    }
    return !found;
}

bool check_col(char board[9][9], int j, char v)
{
    bool found = false;
    for (int i = 0; i < 9; i++)
    {
        if (board[i][j] == v)
        {
            found = true;

            break;
        }
    }
    return !found;
}

bool check_square(char board[9][9], int n, char v)
{
    int i = (n / 3) * 3;
    int j = (n % 3) * 3;
    bool found = false;
    for (int di = 0; di < 3; di++)
    {
        for (int dj = 0; dj < 3; dj++)
        {
            if (board[di + i][dj + j] == v)
            {
                found = true;
                break;
            }
        }
    }
    return !found;
}

long solve(NetStream &stream, char board[9][9], int start = 0)
{
    long sum = 1;
    if (start == 81)
    {
        cout << "There is a solution \n";
        print_board(board);
        cout << "\n\n";
    }
    else
    {
#ifdef _DEBUG_
        print_board(board, start);
#endif
        int row = start / 9;
        int col = start % 9;
        if (board[row][col] == ' ')
        {
            // try all the valid values and then go to the next loc
            for (int v = 1; v < 10; v++)
            {
                int n = (row / 3) * 3 + (col / 3);
                char vchar = v + '0';
                bool check = check_row(board, row, vchar) && check_col(board, col, vchar) && check_square(board, n, vchar);
                if (!check)
                    continue;

                board[row][col] = vchar; //<<
                
                stream.sendCell(row, col, v, "red");
                sum = sum + solve(stream, board, start + 1);
                board[row][col] = ' '; //<<
                
                stream.sendCell(row, col, 0, "white");
            }
        }
        else
            sum = sum + solve(stream, board, start + 1);
    }
    return sum;
}


int main()
{
    NetStream server("127.0.0.1", 12345);

    
    char board[9][9] =
        {
            {' ', ' ', '1', '7', ' ', ' ', ' ', '4', '3'}, // row 0
            {' ', ' ', ' ', ' ', ' ', ' ', '9', ' ', ' '}, // row 1
            {' ', ' ', '6', ' ', '5', ' ', ' ', ' ', ' '}, // row 2
            {'4', ' ', ' ', ' ', ' ', ' ', ' ', '7', '2'}, // row 3
            {' ', '7', ' ', ' ', '6', ' ', ' ', ' ', ' '}, // row 4
            {'9', '2', '3', ' ', '8', ' ', ' ', ' ', ' '}, // row 5
            {' ', ' ', ' ', ' ', ' ', '5', '4', ' ', ' '}, // row 6
            {' ', '3', ' ', ' ', '1', ' ', '8', ' ', ' '}, // row 7
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', '5', '1'}  // row 8
        };

    send_board(server,board);
    long s = solve(server,board);
    cout << s;

    return 0;
}
