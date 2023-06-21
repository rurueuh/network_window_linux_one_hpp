#include "main.hpp"
#include "Ruru\Ruru.hpp"

int main() {
    try {
        Ruru ruru("192.168.1.47");
        ruru.connect();
        ruru.send("hey");
        std::cout << ruru.receive();

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
