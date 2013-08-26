//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Fixture.h"

namespace slim {

    /// Derive from this class and implement the evaluate() method in case you want to deal with arbitrarily
    /// named input and output columns.
    /// Usage: class MyDecisionFixture : public slim::DecisionFixture<MyDecisionFixture> { ... };
    template<typename T>
    class DecisionFixture :
        public slim::Fixture<T>
    {
    protected:
        typedef DecisionFixture BaseClass;
    public:
        virtual void reset() {
            m_inputs.clear();
            m_outputs.clear();
        }

        virtual void execute() {
            m_outputs = evaluate(m_inputs);
        }

        virtual void setValue(const std::string& key, const std::string& value) {
            m_inputs[key] = value;
        }

        virtual std::string getValue(const std::string& key) {
            auto&& i = m_outputs.find(key);
            if(i == m_outputs.end()) {
                throw (slim::Exception() << "no value for: " << key);
            }
            return i->second;
        }
        
        virtual std::map<std::string, std::string> evaluate(std::map<std::string, std::string> args) = 0;
        
    private:
        virtual slim::List callMethod(slim::Context& ctx, const std::string& funcName, const slim::List& args) {
            try {
                // try to dispatch to a registered function
                return slim::Fixture<T>::callMethod(ctx, funcName, args);
            } catch(const slim::Exception& ex) {
                // no registered function found => try to set or get a value
                switch(args.elements.size()) {
                    case 0: {
                        // get a value
                        return slim::List(getValue(funcName));
                    }
                    case 1: {
                        // set a value
                        if((funcName.size() < 4) || (funcName.substr(0, 3) != "set")) {
                            throw (slim::Exception() << "invalid input variable name: " << funcName);
                        }
                        auto&& key = funcName.substr(3); // strip away leading "set"
                        key[0] = std::tolower(key[0]); // first letter to lower case
                        setValue(key, args.elements[0].toString());
                        return slim::List();
                    }
                    default: {
                        // invalid number of args => rethrow original exception
                        throw ex;
                    }
                }
            }
        }

    private:
        std::map<std::string, std::string> m_inputs;
        std::map<std::string, std::string> m_outputs;
    };

} // namespace slim
