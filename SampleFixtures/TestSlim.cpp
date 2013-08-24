//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"


#include <iostream>


struct TestSlim : public slim::Fixture<TestSlim> {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterCtor1<std::string>();
        RegisterMethod("createTestSlimWithString",  &TestSlim::createTestSlimWithString);
        RegisterMember("stringArg",                 &TestSlim::stringArg);
        RegisterMethod("isSame",                    &TestSlim::isSame);
        RegisterMethod("stringArgFromOther",        &TestSlim::stringArgFromOther);
    }

    TestSlim(std::string a = "") : stringArg(a) { }

    std::string stringArg;

    std::shared_ptr<TestSlim> createTestSlimWithString(std::string test) {
        return std::make_shared<TestSlim>(test);
    }

    bool isSame(std::shared_ptr<const TestSlim> obj) const {
        return (this == obj.get());
    }

    std::string stringArgFromOther(std::shared_ptr<const TestSlim> obj) const {
        if(!obj) { return "<null>"; }
        return obj->stringArg;
    }

};
SLIM_REGISTER_FIXTURE(TestSlim);


class SimpleDecision :
    public slim::impl::FixtureBase
{
public:
    SimpleDecision(const slim::List& args) {
        for(auto&& arg : args.elements) {
            m_ctorArgs.push_back(arg.toString());
        }
    }

    virtual slim::List callMethod(slim::Context& ctx, const std::string& funcName, const slim::List& args) {
        std::cerr << "func: " << funcName << ": " << args.toString() << std::endl;

        if(funcName == "beginTable") { return slim::List(); }
        if(funcName == "table") { return slim::List(); }
        if(funcName == "endTable") { return slim::List(); }

        if(funcName == "reset") {
            m_inputs.clear();
            m_outputs.clear();
            return slim::List();
        }

        if(funcName == "execute") {
            m_outputs = evaluate(m_inputs);
throw slim::Exception() << "execution failed: " << funcName;
            return slim::List();
        }

        switch(args.elements.size()) {
            case 0: {
                auto&& i = m_outputs.find(funcName);
                if(i == m_outputs.end()) {
                    throw slim::Exception() << "no value for: " << funcName;
                }
                return slim::List(i->second);
            }
            case 1: {
                if((funcName.size() < 4) || (funcName.substr(0, 3) != "set")) {
                    throw slim::Exception() << "invalid input variable name: " << funcName;
                }
                auto&& argName = funcName.substr(3); // strip of leading "set"
                argName[0] = std::tolower(argName[0]);
                m_inputs[argName] = args.elements[0].toString();
                return slim::List();
            }
            default: {
                throw slim::Exception() << "no function calls with num of args: " << args.elements.size();
            }
        }
    }

    virtual std::map<std::string, std::string> evaluate(std::map<std::string, std::string> args) {
        std::map<std::string, std::string> result;
        result["d"] = args["a"];
        result["e"] = args["b"];
        result["f"] = args["c"];
        return result;
    }

public:
    static std::shared_ptr<SimpleDecision> Make(slim::Context& ctx, const std::string& className, const slim::List& args) {
        return std::make_shared<SimpleDecision>(args);
    }

    static void RegisterBaseMethods() { }
    static void RegisterMethods() { }

private:
    std::vector<std::string> m_ctorArgs;
    std::map<std::string, std::string> m_inputs;
    std::map<std::string, std::string> m_outputs;
};
SLIM_REGISTER_FIXTURE(SimpleDecision);
