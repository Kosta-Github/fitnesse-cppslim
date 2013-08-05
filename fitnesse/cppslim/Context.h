//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Slim.h"

namespace slim {
    
    namespace impl {
        class FixtureBase;
    }

    template<typename T> class Fixture;
    class Communication;
    class List;

    /// This class provides a context the whole test execution can
    /// be carried out within.
    class Context {
    public:
        Context();

        /// This method allows to add a custom "import" handler
        void registerImportHandler(std::function<bool(const std::string&)> importHandler);

        /// This method does the required communication and execution of
        /// the test code. This is actually the method you need to call
        /// from main() with an appropriate communication implementation
        /// (most likely the CommunicationSocket implementation for doing
        /// the proper communication with the fitnesse server).
        bool handleSlimProtocol(Communication& com);

    public:
        /// helper method for converting SLIM data into real C++ data types
        template<typename TO>
        TO slim2typeConv(const List& list);

        /// helper method for converting real C++ data types into SLIM data
        template<typename FROM>
        List type2slimConv(FROM&& from);

        /// helper method for converting a C++ pointer/object into a SLIM reference
        template<typename T>
        List object2slim(const std::shared_ptr<T>& obj);

        /// helper method for converting a SLIM reference back to a C++ pointer/object
        template<typename T>
        void slim2object(const List& list, std::shared_ptr<T>& result) const;

        /// checks if the given list references a known object
        bool hasObject(const List& list) const;

    private:

        /// This method executes all the commands for a whole SLIM table.
        List execute(const List& commandList);

        /// This method dispatches the single command to the corresponding
        /// function callback registered for that command.
        List dispatchCommand(const List& command);

        /// This method implements the SLIM "import" command.
        List executeImportCommand(const List& command);

        /// This method implements the SLIM "make" command.
        List executeMakeCommand(const List& command);

        /// This method implements the SLIM "call" command.
        List executeCallCommand(const List& command);

        /// This method implements the SLIM "callAndAssign" command.
        List executeCallAndAssignCommand(const List& command);

        /// This method implements the actual SLIM "call" command.
        List callCommand(
            const std::string& objID,
            const std::string& symbolName,
            const std::string& funcName,
            List args
        );

        /// This method replaces all known symbols from the symbol table
        /// in-place in the given (nested) SLIM list.
        void substituteSymbols(List& list) const;

        /// This method decodes the next message from the given communication
        /// channel.
        static List decodeNextMessage(Communication& com);

    private:
        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

} // namespace slim

#include "ContextImpl.h"
