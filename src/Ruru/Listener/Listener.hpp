#include "../Socket/Socket.hpp"

namespace Ruru {
    class Listener final {
        public:
            Listener(const int port = PORT);
            ~Listener();

            void listen(int number = 1) const;

            Socket accept() const;

        private:
            #ifndef WIN32
                int _serverSocket;
            #else
                WSADATA _wsaData;
                SOCKET _serverSocket;
            #endif
            sockaddr_in _serverAddress;
    };
}