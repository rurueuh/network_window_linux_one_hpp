#include "main.hpp"
#include "Ruru/Network.hpp"

void test_listener(void)
{
    Ruru::Listener listener{ 4242 };
    listener.listen();
    Ruru::Socket socket = listener.accept();
    std::cout << socket.getIp() << std::endl;
    std::cout << socket.receive() << std::endl;
    socket.send("Hello World\n");
}

void test_connexion(void)
{
    Ruru::Socket ruru;
    ruru.connect();
    std::cout << ruru.getIp() << std::endl;
    ruru.send("test\n");
    std::cout << ruru.receive();
}

int main() 
{
    try {
        test_listener();
        test_connexion();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
