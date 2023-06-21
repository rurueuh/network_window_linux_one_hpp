#include "../main.hpp"

constexpr auto PORT = 4242;
constexpr auto IP = "127.0.0.1";
constexpr auto TIMEOUT = 10;

class Ruru final {
public:
	Ruru(const char *ip = IP, int port = PORT);
	~Ruru();
	
	void connect();
	void send(const std::string &message);
	bool isTimeout() const;
	std::string receive();

	static void setDebugMode(bool debugMode) { Ruru::_debugMode = debugMode; }
	static void setIgnoreWarning(bool ignoreWarning) { Ruru::_ignoreWarning = ignoreWarning ;}
	
private:
	static bool _debugMode;
	static bool _ignoreWarning;
	void InitSocket();
	bool _isConnected = false;
	WSADATA _wsaData;
	SOCKET _clientSocket;
	sockaddr_in _serverAddress;
};