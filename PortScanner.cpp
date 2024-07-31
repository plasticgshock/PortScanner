#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

bool WinSockInit() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return false;
    }
    return true;
}


string ResolveHostName(const string& hostname) {
    addrinfo hints = {}, * res;
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), NULL, &hints, &res) != 0) {
        cerr<< "Failed to resolve hostname" << hostname << endl;
        WSACleanup();
        exit(1);
    }
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, ipstr, sizeof(ipstr));
    freeaddrinfo(res);
    return string(ipstr);
}


bool IsPortOpen(const string& ip, int port) {
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        cerr << "Socket creation failed (fuck you!): " << WSAGetLastError << endl;
        return false;
    }

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    int result = connect(sockfd, (sockaddr*)&target, sizeof(target));

    closesocket(sockfd);
    return (result == 0);
}


int main(int argc, char* argv[])
{
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <hostname> <start_port> <end_port> " << endl;
        return 1;
    }

    if (!WinSockInit()) {
        return 1;
    }

    string hostName = argv[1];
    int startPort = stoi(argv[2]);
    int endPort = stoi(argv[3]);

    string ip = ResolveHostName(hostName);
    cout << "Scanning " << ip << "[" << hostName << "] from port " << startPort << " to " << endPort << endl;
    
    for (int port = startPort; port < endPort; ++port) {
        if (IsPortOpen(ip, port)) {
            cout << "Port " << port << " is open" << endl;
        }
        else {
            cout << "Fuck you! Port " << port << " is closed." << endl;
        }
    }


    WSACleanup();
    return 0;
}