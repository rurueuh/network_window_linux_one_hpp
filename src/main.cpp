#include "main.hpp"
#include "Ruru/Network.hpp"

int main() 
{
    try {
        Ruru::Listener listener {4242};
        listener.listen();
        Ruru::Socket socket = listener.accept();
        std::cout << socket.getIp() << std::endl;
        std::cout << socket.receive() << std::endl;
        socket.send("Hello World\n");
        return 0;
        // Ruru::Socket ruru;
        // ruru.connect();
        // std::cout << ruru.getIp() << std::endl;
        // std::cout << ruru.getPort() << std::endl;
        // ruru.send("\n");
        // std::cout << ruru.receive();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
