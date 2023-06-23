#include "Listener.hpp"

Ruru::Listener::Listener(const int port)
{
    #ifdef WIN32
        if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0) {
			throw std::runtime_error("Error init : WSAStartup.");
		}
    #endif
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        throw std::runtime_error("Error init : Socket.");
    }
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(static_cast<u_short>(port));
    _serverAddress.sin_addr.s_addr = inet_addr(IP);

    if (bind(_serverSocket, reinterpret_cast<sockaddr*>(&_serverAddress), sizeof(_serverAddress)) == -1) {
        throw std::runtime_error("Error bind : Socket binding.");
    }
}

Ruru::Listener::~Listener()
{
    #ifndef WIN32
        close(_serverSocket);
    #else
        closesocket(_serverSocket);
		WSACleanup();
    #endif
}

void Ruru::Listener::listen(int number) const
{
    if (::listen(_serverSocket, number) == -1) {
        throw std::runtime_error("Error listen : Socket listening.");
    }
}

Ruru::Socket Ruru::Listener::accept() const
{
    #ifdef WIN32
        SOCKET clientSocket;
        sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);
        clientSocket = ::accept(_serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Error accept : Socket accepting.");
        }
        return Socket(clientSocket, clientAddress);
    #else
        int clientSocket;
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        clientSocket = ::accept(_serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
        if (clientSocket == -1) {
            throw std::runtime_error("Error accept : Socket accepting.");
        }
        return Socket(clientSocket, clientAddress);
    #endif
}
