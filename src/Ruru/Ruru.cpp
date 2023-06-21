#include "Ruru.hpp"

bool Ruru::_debugMode = false;
bool Ruru::_ignoreWarning = false;

Ruru::Ruru(const char *ip, int port)
{
    InitSocket();
    
    _serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(static_cast<u_short>(port));
    _serverAddress.sin_addr.s_addr = inet_addr(ip);
}

void Ruru::InitSocket()
{
    if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0) {
        throw std::runtime_error("Error init : Winsock.");
    }
    _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Error init : Socket.");
    }
}

Ruru::~Ruru()
{
    closesocket(_clientSocket);
    WSACleanup();
}

void Ruru::connect()
{
    if (::connect(_clientSocket, reinterpret_cast<sockaddr*>(&_serverAddress), sizeof(_serverAddress)) == SOCKET_ERROR) {
		throw std::runtime_error("Error connect : Socket connection.");
	}
    timeval timeout {};
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(_clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR) {
        throw std::runtime_error("Error connect : Socket timeout.");
    }
    _isConnected = true;
}

void Ruru::send(const std::string& message)
{
    if (_isConnected == false) {
		throw std::runtime_error("Error send : Socket not connected.");
	}
    if (message.back() != '\n') {
        std::string dup = message;
        dup += "\n";
        if (!_ignoreWarning)
            std::cerr << "WARNING : send data without '\\n' at the end (can make some latence)" << std::endl;
        if (::send(_clientSocket, dup.c_str(), dup.length(), 0) == SOCKET_ERROR) {
            throw std::runtime_error("Error send : Socket error.");
        }
        return;
    }
    if (::send(_clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
		throw std::runtime_error("Error send : Socket error.");
	}
}

bool Ruru::isTimeout() const
{
    if (WSAGetLastError() == WSAETIMEDOUT) {
        return true;
    } else {
        return false;
    }
}

std::string Ruru::receive()
{
    const auto SIZE_BUFFER = 1024;
    std::vector<char> buff(SIZE_BUFFER);
    std::vector<char> rcvData;

    while (true) {
        int readLen = recv(_clientSocket, buff.data(), SIZE_BUFFER, 0);
        if (readLen == SOCKET_ERROR) {
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
                std::cout << "DEBUG RURU : " << std::string(buff.begin(), buff.end()) << std::endl;
            }
            rcvData.insert(rcvData.end(), buff.begin(), buff.begin() + readLen);
            if (rcvData.back() == '\n') {
                break;
            }
        }
    }
    return std::string(rcvData.begin(), rcvData.end());
}
