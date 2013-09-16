//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Communication.h"
#include "Fixture.h"
#include "List.h"
#include "Registry.h"

namespace slim {
    
    struct Context::Impl {
        std::function<bool(const std::string&)> m_importHandler;
        std::map<std::string, std::function<List(const List&)>> m_commandDispatcher;
        std::map<std::string, std::shared_ptr<impl::FixtureBase> > m_objects;
        std::map<std::string, List> m_symbolTable;
        
    };

    inline Context::Context() : m_impl(std::make_shared<Impl>()) {
        // register the callbacks for the different predefined SLIM commands
        m_impl->m_commandDispatcher["import"]           = [this](const List& list) { return executeImportCommand(list);        };
        m_impl->m_commandDispatcher["make"]             = [this](const List& list) { return executeMakeCommand(list);          };
        m_impl->m_commandDispatcher["call"]             = [this](const List& list) { return executeCallCommand(list);          };
        m_impl->m_commandDispatcher["callAndAssign"]    = [this](const List& list) { return executeCallAndAssignCommand(list); };
    }

    /// This method allows to add a custom "import" handler
    inline void Context::registerImportHandler(
        std::function<bool(const std::string&)> importHandler
    ) {
        assert(importHandler);
        m_impl->m_importHandler = importHandler;
    }

    /// This method does the required communication and execution of
    /// the test code. This is actually the method you need to call
    /// from main() with an appropriate communication implementation
    /// (most likely the CommunicationSocket implementation for doing
    /// the proper communication with the fitnesse server).
    inline bool Context::handleSlimProtocol(
        Communication& com
    ) {
        // send supported Slim protocol version
        com.writeTo("Slim -- V0.3\n");

        try {
            while(true) {
                const List commandList = decodeNextMessage(com);
                if(!commandList.isList && (commandList.string == "bye")) {
                    // we are done now...

                    // clear the object cache now
                    m_impl->m_objects.clear();

                    return true;
                }

                List result = execute(commandList);

                // this is kind of ugly: we need to fix the length encoding here,
                // since in this case we need to return the bytes count(!) not the
                // chars...
                std::string resultStr = result.toSlim();
                const size_t bytes = resultStr.size() - 7;
                const std::string bytesStr = List::lengthString(bytes);
                std::copy(bytesStr.begin(), bytesStr.end(), resultStr.begin());

                com.writeTo(resultStr);
            }
        } catch(const std::exception& ex) {
            List msg; msg.elements.emplace_back(std::string("__EXCEPTION__:ABORT_SLIM_TEST:message:<< \"") + ex.what() + "\" >>");
            com.writeTo(msg.toSlim());
            return false;
        } catch(...) {
            List msg; msg.elements.emplace_back(std::string("__EXCEPTION__:ABORT_SLIM_TEST:message:<< \"unknown exception type\" >>"));
            com.writeTo(msg.toSlim());
            return false;
        }
    }

    /// helper method for converting SLIM data into real C++ data types
    template<typename TO>
    inline TO Context::slim2typeConv(const List& list) {
        TO result;
        slim2type(*this, list, result);
        return result;
    }

    /// helper method for converting real C++ data types into SLIM data
    template<typename FROM>
    inline List Context::type2slimConv(FROM&& from) {
        return type2slim(*this, std::forward<FROM>(from));
    }

    /// helper method for converting a C++ pointer/object into a SLIM reference
    template<typename T>
    inline List Context::object2slim(const std::shared_ptr<T>& obj) {
        if(!obj) {
            // this one is simple...
            return List(std::string("null"));
        }

        std::ostringstream nameStream; nameStream << obj.get();
        std::string name = nameStream.str();

        m_impl->m_objects[name] = obj;

        return List(std::move(name));
    }

    /// helper method for converting a SLIM reference back to a C++ pointer/object
    template<typename T>
    inline void Context::slim2object(const List& list, std::shared_ptr<T>& result) const {
        if(list.isList) {
            throw (Exception() << "argument is not a object: " << list);
        }

        if(list.string == "null") {
            // this one is simple...
            result = nullptr;
            return;
        }

        // find the corresponding object
        auto it = m_impl->m_objects.find(list.string);
        if(it == m_impl->m_objects.end()) {
            throw (Exception() << "object not found: " << list);
        }

        // check if the found object is of the correct data type
        result = std::dynamic_pointer_cast<T>(it->second);
        if(!result) {
            throw (Exception() << "object is of wrong type: " << list);
        }
    }

    inline bool Context::hasObject(const List& list) const {
        if(list.isList) { return false; }
        if(list.string == "null") { return true; }
        return (m_impl->m_objects.find(list.string) != m_impl->m_objects.end());
    }

    /// This method executes all the commands for a whole SLIM table.
    inline List Context::execute(
        const List& commandList
    ) {
        if(!commandList.isList) {
            throw (Exception() << "invalid SLIM command list: " << commandList);
        }

        // the "result" list will collect the results of all commands
        // requested for execution by the passed in "command list"
        List result;

        // iterate over all commands in the list
        for(size_t i = 0, iEnd = commandList.elements.size(); i < iEnd; ++i) {
            const List& command = commandList.elements[i];

            if(!command.isList || (command.elements.size() < 2)) {
                throw (Exception() << "invalid SLIM command: " << command);
            }

            // dipatch the current command
            const std::string& id = command.elements[0].string;
            List commandResult = dispatchCommand(command);

            // and feed the result of the command back into the "result" list
            List resultList;
            resultList.elements.push_back(List(id));
            resultList.elements.push_back(std::move(commandResult));
            result.elements.push_back(std::move(resultList));
        }

        return result;
    }

    /// This method dispatches the single command to the corresponding
    /// function callback registered for that command.
    inline List Context::dispatchCommand(
        const List& command
    ) {
        try {
            assert(command.isList && (command.elements.size() >= 2)); // already checked in execute()...

            // get the command ID from the SLIM command
            const std::string& commandID = command.elements[1].string;

            // find the corresponding command callback function
            auto c = m_impl->m_commandDispatcher.find(commandID);
            if(c == m_impl->m_commandDispatcher.end()) {
                throw (Exception() << "unknown command: " << commandID);
            }

            // execute the command
            return c->second(command);
        } catch(const std::exception& ex) {
            return List(std::string("__EXCEPTION__:message:<< \"") + ex.what() + "\" >>");
        } catch(...) {
            return List(std::string("__EXCEPTION__:message:<< \"unknown exception type\" >>"));
        }
    }

    /// This method implements the SLIM "import" command.
    inline List Context::executeImportCommand(
        const List& command
    ) {
        try {
            assert(command.isList && (command.elements.size() == 3));

            // get the name of the import from the SLIM command
            const std::string& import = command.elements[2].string;

            if(!m_impl->m_importHandler) {
                throw (Exception() << "no import handler registered; could not import: " << import);
            }

            if(!m_impl->m_importHandler(import)) {
                throw (Exception() << "import failed for: " << import);
            }

            // execute the command
            return List("OK");
        } catch(const std::exception& ex) {
            return List(std::string("__EXCEPTION__:message:<< \"") + ex.what() + "\" >>");
        } catch(...) {
            return List(std::string("__EXCEPTION__:message:<< \"unknown exception type\" >>"));
        }
    }

    /// This method implements the SLIM "make" command.
    inline List Context::executeMakeCommand(
        const List& command
    ) {
        if(!command.isList || (command.elements.size() < 4)) {
            throw (Exception() << "not enough parameters for \'make\' command; exported at least 4 but got " << command.elements.size() << ": " << command);
        }

        const std::string& objID        = command.elements[2].string;
        const std::string& className    = command.elements[3].string;

        // extract only the args from the command, starting at the 5th element
        List args; args.elements.assign(command.elements.begin() + 4, command.elements.end());

        // substitute contained symbols
        substituteSymbols(args);

        // check if the object is actually an object reference to
        // an already existing object
        auto existingObj = m_impl->m_symbolTable.find(className);
        if(existingObj != m_impl->m_symbolTable.end()) {
            auto existingObj2 = m_impl->m_objects.find(existingObj->second.toString());
            if(existingObj2 == m_impl->m_objects.end()) {
                throw (Exception() << "not a valid object reference: " << className);
            }

            m_impl->m_objects[objID] = existingObj2->second; // add an additional reference here
            return List(std::string("OK"));
        }

        // construct the object
        std::shared_ptr<impl::FixtureBase> obj = Registry::Instance().create(*this, className, args);
        assert(obj);

        // register the created object and ensure proper lifetime management
        // by putting it into a smart pointer
        m_impl->m_objects[objID] = obj;

        // signal a successful object creation
        return List(std::string("OK"));
    }

    /// This method implements the SLIM "call" command.
    inline List Context::executeCallCommand(
        const List& command
    ) {
        if(!command.isList || (command.elements.size() < 4)) {
            throw (Exception() << "not enough parameters for \'call\' command; expected at least 4 but got " << command.elements.size() << ": " << command);
        }

        const std::string objID         = command.elements[2].string;
        const std::string funcName      = command.elements[3].string;
        List args;   args.elements.assign(command.elements.begin() + 4, command.elements.end());

        return callCommand(objID, "", funcName, args);
    }

    /// This method implements the SLIM "callAndAssign" command.
    inline List Context::executeCallAndAssignCommand(
        const List& command
    ) {
        if(!command.isList || (command.elements.size() < 5)) {
            throw (Exception() << "not enough parameters for \'callAndAssign\' command; expected at least 5 but got " << command.elements.size() << ": " << command);
        }

        // extract the actual symbol name from the command
        const std::string symbolName    = command.elements[2].string;
        const std::string objID         = command.elements[3].string;
        const std::string funcName      = command.elements[4].string;
        List args;   args.elements.assign(command.elements.begin() + 5, command.elements.end());

        if(symbolName.empty()) {
            throw (Exception() << "symbol name cannot be empty; command: " << command);
        }

        return callCommand(objID, "$" + symbolName, funcName, args);
    }

    /// This method implements the actual SLIM "call" command.
    inline List Context::callCommand(
        const std::string& objID,
        const std::string& symbolName,
        const std::string& funcName,
        List args
    ) {
        if(!args.isList) {
            throw (Exception() << "no parameters given: " << args);
        }

        // substitute contained symbols
        substituteSymbols(args);

        // find the correct object the command is referencing
        auto obj = m_impl->m_objects.find(objID);
        if(obj == m_impl->m_objects.end()) {
            throw (Exception() << "object \'" << objID << "\' not found: " << funcName);
        }

        // call the requested function of the object
        List result = obj->second->callMethod(*this, funcName, args);

        // and store the result in the symbol table
        if(!symbolName.empty()) {
            m_impl->m_symbolTable[symbolName] = result;
        }

        return result;
    }

    /// This method replaces all known symbols from the symbol table
    /// in-place in the given (nested) SLIM list.
    inline void Context::substituteSymbols(List& list) const {
        if(list.isList) {
            for(size_t i = 0, iEnd = list.elements.size(); i < iEnd; ++i) {
                substituteSymbols(list.elements[i]);
            }
            return;
        }

        // iterate over all known symbols
        for(auto it = m_impl->m_symbolTable.begin(); it != m_impl->m_symbolTable.end(); ++it) {
            const std::string& symbol = it->first;
            assert(!symbol.empty());

            if(list.string == symbol) {
                // an exact match => replace it directly
                list = it->second;
            } else {
                // do an in-string search-and-replacement
                std::string::size_type pos;
                while(pos = list.string.find(symbol), pos != std::string::npos) {
                    list.string.replace(pos, symbol.size(), it->second.toString());
                }
            }
        }
    }

    /// This method decodes the next message from the given communication
    /// channel.
    inline List Context::decodeNextMessage(
        Communication& com
    ) {
        // first read 7 chars encoding the string length follow by a collon ':'
        std::string header = com.readFrom(7);
        const char* headerPtr = header.c_str();
        const size_t len = List::decodeLength(headerPtr);

        // and now read the actual SLIM encoded string and return the decoded
        // version of it
        std::string content = com.readFrom(len);
        const char* contentPtr = content.c_str();
        return List::decode(contentPtr);
    }

} // namespace slim
