#include "fitnesse/cppslim/CommunicationSocket_Boost.h"
#include "fitnesse/cppslim/ContextImpl.h"

#include <cstdlib>
#include <chrono>
#include <future>
#include <iostream>

slim::Registry& slim::Registry::Instance() { static slim::Registry s_reg; return s_reg; }

int main(int argc, char** argv) {
    std::wcout.precision(17);   std::cout.precision(17);
    std::wcerr.precision(17);   std::cerr.precision(17);

    try {

        int64_t port = 8085;
        const std::string PORT_TAG = "-port";
        const std::string TEST_TAG = "-test";
        std::string portStr;
        std::string testStr;

        for(int i = 1; i < argc; ++i) {
            if((argv[i] == PORT_TAG) && (i + 1 < argc)) { portStr = argv[i + 1]; ++i; continue; }
            if((argv[i] == TEST_TAG) && (i + 1 < argc)) { testStr = argv[i + 1]; ++i; continue; }
        }
        
        if(!portStr.empty()) {
            port = std::stoi(portStr.c_str());
        }
        if((port <= 0) || (65536 <= port)) {
            std::cerr << "invalid port: " << port << std::endl;
            return EXIT_FAILURE;
        }

        std::future<int> callingFitnesse;
        if(!testStr.empty()) {
            std::cout << "directly calling test: " << testStr << std::endl;
            
            const std::string JAVA_COMMAND = "java";
            const std::string FITNESSE_JAR = "fitnesse-standalone-20130530.jar";
    
            std::string command;
            command += JAVA_COMMAND + " ";
            command += "-DSLIM_PORT=" + std::to_string(port - 1) + " "; // not sure, why FitNesse starts with port+1...
            command += "-jar " + FITNESSE_JAR + " ";
            command += "-c \"" + testStr + "&remote_debug&format=text\" ";
    
            callingFitnesse = std::async([command]() { return std::system(command.c_str()); });
        }
        
        slim::CommunicationSocket_Boost comSocket(static_cast<uint16_t>(port));

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
