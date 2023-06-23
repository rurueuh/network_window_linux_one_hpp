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


namespace Ruru {
	constexpr auto PORT = 4242;
	constexpr auto IP = "127.0.0.1";
	constexpr auto TIMEOUT = 10;

	class Socket final {
	public:
		Socket(const char *ip = IP, int port = PORT);
		#ifdef WIN32
			Socket(const SOCKET &socket, const sockaddr_in &serverAddress) : _clientSocket(socket), _serverAddress(serverAddress) { _isConnected = true;}
		#else
			Socket(const int &socket, const sockaddr_in &serverAddress) : _clientSocket(socket), _serverAddress(serverAddress) {_isConnected = true;}
		#endif
		~Socket();
		
		void connect();
		void send(const std::string &message);
		bool isTimeout() const;
		std::string receive();
		std::string getIp() const;

		static void setDebugMode(bool debugMode) { Socket::_debugMode = debugMode; }
		static void setIgnoreWarning(bool ignoreWarning) { Socket::_ignoreWarning = ignoreWarning ;}
		
	private:
		static bool _debugMode;
		static bool _ignoreWarning;
		void InitSocket();
		bool _isConnected = false;
		#ifdef WIN32
			WSADATA _wsaData;
			SOCKET _clientSocket;
		#else
			int _clientSocket;
		#endif
		sockaddr_in _serverAddress;
	};
}