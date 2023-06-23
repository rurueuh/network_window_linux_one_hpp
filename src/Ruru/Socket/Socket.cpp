#include "Socket.hpp"

bool Ruru::Socket::_debugMode = false;
bool Ruru::Socket::_ignoreWarning = false;

Ruru::Socket::Socket(const char *ip, int port)
{
    InitSocket();
    
    _serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(static_cast<u_short>(port));
    _serverAddress.sin_addr.s_addr = inet_addr(ip);
}

void Ruru::Socket::InitSocket()
{
    #ifdef WIN32
        if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0) {
            throw std::runtime_error("Error init : Winsock.");
        }
        _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (_clientSocket == INVALID_SOCKET) {
            throw std::runtime_error("Error init : Socket.");
        }
    #else
        _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (_clientSocket == -1) {
            throw std::runtime_error("Error init : Socket.");
        }
    #endif
}

Ruru::Socket::~Socket()
{
    #ifdef WIN32
        closesocket(_clientSocket);
        WSACleanup();
    #else
        close(_clientSocket);
    #endif
}

void Ruru::Socket::connect()
{
    if (::connect(_clientSocket, reinterpret_cast<sockaddr*>(&_serverAddress), sizeof(_serverAddress)) == -1) {
        throw std::runtime_error("Error connect : Socket connection.");
    }
    timeval timeout {};
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(_clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == -1) {
        throw std::runtime_error("Error connect : Socket timeout.");
    }
    _isConnected = true;
}

void Ruru::Socket::send(const std::string& message)
{
    if (_isConnected == false) {
        throw std::runtime_error("Error send : Socket not connected.");
    }
    if (message.back() != '\n') {
        std::string dup = message;
        dup += "\n";
        if (!_ignoreWarning)
            std::cerr << "WARNING : send data without '\\n' at the end (can make some latence)" << std::endl;
        if (::send(_clientSocket, dup.c_str(), dup.length(), 0) == -1) {
            throw std::runtime_error("Error send : Socket error.");
        }
        if (_debugMode)
            std::cout << "DEBUG RURU SEND : " << dup;
        return;
    }
    if (::send(_clientSocket, message.c_str(), message.length(), 0) == -1) {
        throw std::runtime_error("Error send : Socket error.");
    }
    if (_debugMode)
        std::cout << "DEBUG RURU SEND : " << message;
}

bool Ruru::Socket::isTimeout() const
{
    #ifdef WIN32
        if (WSAGetLastError() == WSAETIMEDOUT) {
            return true;
        } else {
            return false;
        }
    #else
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        } else {
            return false;
        }
    #endif
}

std::string Ruru::Socket::receive()
{
    const auto SIZE_BUFFER = 1024;
    std::vector<char> buff(SIZE_BUFFER);
    std::vector<char> rcvData;
    
    while (true) {
        int readLen = recv(_clientSocket, buff.data(), SIZE_BUFFER, 0);
        if (readLen == -1) {
            if (isTimeout() == true && rcvData.empty() == false) {
                if (!_ignoreWarning)
                    std::cerr << "WARNING : socket recv timeout (maybe don't end by '\\n'" << std::endl;
                break;
            } else if (isTimeout() == true && rcvData.empty() == true) { // wait encore la rep
                continue;
            } else {
                throw std::runtime_error("Error read : Socket return -1");
            }
        } else if (readLen == 0) {
            throw std::runtime_error("Error read : get Disconnected by host");
        } else {
            if (_debugMode == true) {
                std::cout << "DEBUG RURU RCV : " << std::string(buff.begin(), buff.end()) << std::endl;
            }
            rcvData.insert(rcvData.end(), buff.begin(), buff.begin() + readLen);
            if (rcvData.back() == '\n') {
                break;
            }
        }
    }
    return std::string(rcvData.begin(), rcvData.end());
}

std::string Ruru::Socket::getIp() const
{
    return std::string(inet_ntoa(_serverAddress.sin_addr));
}
