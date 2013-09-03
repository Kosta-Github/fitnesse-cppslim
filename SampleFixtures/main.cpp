#include "fitnesse/cppslim/CommunicationSocket.h"
#include "fitnesse/cppslim/ContextImpl.h"

#include <iostream>

slim::Registry& slim::Registry::Instance() { static slim::Registry s_reg; return s_reg; }

int main(int argc, char** argv) {

    std::wcout.precision(17);   std::cout.precision(17);
    std::wcerr.precision(17);   std::cerr.precision(17);

    try {

        int64_t port = 8050;
        if(argc < 2) {
            std::cout << "using default port: " << port << std::endl;
        } else {
            port = std::stoi(argv[1]);
        }
        
        if((port <= 0) || (65536 <= port)) {
            std::cerr << "invalid port: " << port << std::endl;
            return EXIT_FAILURE;
        }

        slim::CommunicationSocket comSocket(static_cast<uint16_t>(port));

        slim::Context ctx;
        ctx.handleSlimProtocol(comSocket);
        
        return EXIT_SUCCESS;

    } catch(const std::exception& ex) {
        std::cerr << "unhandled exception detected: " << ex.what() << std::endl;
    } catch(...) {
        std::cerr << "unhandled unknown exception detected" << std::endl;
    }

    return EXIT_FAILURE;
}
