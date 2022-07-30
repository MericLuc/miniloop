### What is it ?

**miniloop** is nothing more than a tiny c++ wrapper around [libevent](https://github.com/libevent/libevent) 

Do not expect anything fancy, just the essentials of an event loop (C++ flavoured):
- **I/O** on file descriptors
- **Timeouts** (oneshot, recurrent, date-based...)
- **Signals** (UNIX)


### The things to know

- Basically every **miniloop** objects allow to register your callback.
- Include "_Loop.h_"
- Start the event loop ( `loop::Loop::singleton().run()` )
- Do your amazing _event-based_ stuff
- Stop the event loop ( `loop::Loop::singleton().exit()` )

### An example

Here is a very simple example on how to use **miniloop**.

This program will :
- display messages on the standard output every time an event occurs on a socket.
- remember you to drink a coffee every hour
- perform a gracefull exit when receiving a SIGINT signal.


```cplusplus
#include <cstdint>
#include <iostream>
#include <memory>
#include <signal.h>
#include <sys/socket.h>

#include "Loop.h"

using namespace loop;

int
main()
{
    auto evt_sigint = std::make_unique<Loop::UNIX_SIGNAL>(SIGINT);
    evt_sigint->onEvent([](int sig) {
        std::cout << "Gracefully exiting after receiving signal " << sig << '\n';
        Loop::singleton().exit();
    });

    auto evt_drink_coffee = std::make_unique<Loop::RecurrentTimeout>(3600000);
    evt_drink_coffee->onTimeout([]() { std::cout << "You should drink your coffee\n"; });

    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     Let's assume you error-checked and are doing something usefull with that socket
     ...
    */
    auto evt_file_io = std::make_unique<Loop::IO>(fd);
    evt_file_io->setRequestedEvents(Loop::IO::READ);
    evt_file_io->onEvent([](int) { std::cout << "Something happened on my magnificent socket!\n"; });

    Loop::singleton().run();

    return EXIT_SUCCESS;
}

```
