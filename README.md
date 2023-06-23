# network_window_linux_one_hpp

hello !

this repo is for you if you want to use network to your project, support listener and socket for linux and windows in only one hpp

you can see exemple in src main.cpp if you want to use it and source code before be transform in one hpp

just include it and use namepass Ruru::

only thing to do in compilation (only windows) is add this `-lws2_32` in cmake that look like this:

```
if (WIN32)
    target_link_libraries(network -lws2_32)
endif()
```

if you want to debug your network use this `Ruru::Socket::setDebugMode(true)` at the start of project

if you want to remove Warning use this `Ruru::Socket::setIgnoreWarning(true)`

you can edit preconfig value here Net.hpp:18 : 

```
  constexpr auto PORT = 4242;
  constexpr auto IP = "127.0.0.1";
  constexpr auto TIMEOUT = 10;
```

value IP and PORT is used if you don't enter in arg

this repo doesn't host any documentation for use network

having fun !
