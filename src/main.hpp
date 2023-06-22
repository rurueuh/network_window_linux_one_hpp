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