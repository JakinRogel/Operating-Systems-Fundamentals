```mermaid
graph TD;
    Physical_Hardware["Physical Hardware"] --> Hypervisor["Hypervisor"];
    Hypervisor --> VM1["Virtual Machine 1"];
    Hypervisor --> VM2["Virtual Machine 2"];
    Hypervisor --> VM3["Virtual Machine 3"];
    VM1 --> IDE["IDE"];
    VM1 --> DevTools["Development Tools"];
    VM2 --> TestEnv["Testing Environment"];
    VM3 --> Database["Database"];
    VM3 --> WebServer["Web Server"];
    Application["Application"] --> IDE;
    Application --> DevTools;
    Application --> TestEnv;
    Application --> Database;
    Application --> WebServer;

