#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

constexpr auto PORT = 4242;
constexpr auto IP = "127.0.0.1";
constexpr auto TIMEOUT = 10;

namespace Ruru {
	class Socket final {
	public:
		Socket(const char* ip = IP, int port = PORT)
		{
			InitSocket();

			_serverAddress.sin_family = AF_INET;
			_serverAddress.sin_port = htons(static_cast<u_short>(port));
			_serverAddress.sin_addr.s_addr = inet_addr(ip);
		}
#ifdef WIN32
		Socket(const SOCKET& socket, const sockaddr_in& serverAddress) : _clientSocket(socket), _serverAddress(serverAddress) { _isConnected = true; }
#else
		Socket(const int& socket, const sockaddr_in& serverAddress) : _clientSocket(socket), _serverAddress(serverAddress) { _isConnected = true; }
#endif
		~Socket()
		{
#ifdef WIN32
			closesocket(_clientSocket);
			WSACleanup();
#else
			close(_clientSocket);
#endif
		}

		void connect()
		{
			if (::connect(_clientSocket, reinterpret_cast<sockaddr*>(&_serverAddress), sizeof(_serverAddress)) == -1) {
				throw std::runtime_error("Error connect : Socket connection.");
			}
			timeval timeout{};
			timeout.tv_sec = TIMEOUT;
			timeout.tv_usec = 0;
			if (setsockopt(_clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == -1) {
				throw std::runtime_error("Error connect : Socket timeout.");
			}
			_isConnected = true;
		}
		void send(const std::string& message)
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
				return;
			}
			if (::send(_clientSocket, message.c_str(), message.length(), 0) == -1) {
				throw std::runtime_error("Error send : Socket error.");
			}

		}
		bool isTimeout() const
		{
#ifdef WIN32
			if (WSAGetLastError() == WSAETIMEDOUT) {
				return true;
			}
			else {
				return false;
			}
#else
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return true;
			}
			else {
				return false;
			}
#endif
		}
		std::string receive()
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
					}
					else if (isTimeout() == true && rcvData.empty() == true) { // wait encore la rep
						continue;
					}
					else {
						throw std::runtime_error("Error read : Socket return -1");
					}
				}
				else if (readLen == 0) {
					throw std::runtime_error("Error read : get Disconnected by host");
				}
				else {
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
		std::string getIp() const
		{
			return std::string(inet_ntoa(_serverAddress.sin_addr));
		}

		static void setDebugMode(bool debugMode) { Socket::_debugMode = debugMode; }
		static void setIgnoreWarning(bool ignoreWarning) { Socket::_ignoreWarning = ignoreWarning; }

	private:
		static bool _debugMode;
		static bool _ignoreWarning;
		void InitSocket()
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
		bool _isConnected = false;
#ifdef WIN32
		WSADATA _wsaData;
		SOCKET _clientSocket;
#else
		int _clientSocket;
#endif
		sockaddr_in _serverAddress;
	};


	class Listener final {
	public:
		Listener(const int port = PORT)
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
		~Listener()
		{
#ifndef WIN32
			close(_serverSocket);
#else
			closesocket(_serverSocket);
			WSACleanup();
#endif
		}

		void listen(int number = 1) const
		{
			if (::listen(_serverSocket, number) == -1) {
				throw std::runtime_error("Error listen : Socket listening.");
			}
		}

		Socket accept() const
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

	private:
#ifndef WIN32
		int _serverSocket;
#else
		WSADATA _wsaData;
		SOCKET _serverSocket;
#endif
		sockaddr_in _serverAddress;
	};
};
inline bool Ruru::Socket::_debugMode = false;
inline bool Ruru::Socket::_ignoreWarning = false;
