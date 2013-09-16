//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/DecisionFixture.h"
#include "fitnesse/cppslim/Registry.h"

SLIM_DEFINE_DECISION_FIXTURE(SimpleDecision) {
public:
    static void RegisterMethods() {
        RegisterSingleton();
    }

    virtual std::map<std::string, std::string> evaluate(std::map<std::string, std::string> args) {
        std::map<std::string, std::string> result;
        result["d"] = args["a"];
        result["e"] = args["b"];
        result["f"] = args["c"];
        return result;
    }

};

SLIM_DEFINE_DECISION_FIXTURE(RevaDecision) {
public:
    static void RegisterMethods() {
        RegisterCtor1<std::string>();
    }
    
    RevaDecision(std::string standardName_) : standardName(standardName_) { }
    const std::string standardName;

    virtual void setValue(const std::string& key, const std::string& value) {
        BaseClass::setValue(AdjustKey(key), value);
    }

    virtual std::string getValue(const std::string& key) {
        return BaseClass::getValue(AdjustKey(key));
    }

    virtual std::map<std::string, std::string> evaluate(std::map<std::string, std::string> args) {
        std::map<std::string, std::string> result;
        result["max-speed"]             = args["curve-length"];
        result["minimum-curve-radius"]  = args["normal-speed"];
        result["num-lanes"]             = args["max-lanes"];
        result["standard-name"]         = standardName;
        return result;
    }

    static std::string AdjustKey(const std::string& key) {
        std::string newKey;
        for(char c : key) {
            if(std::isupper(c, std::locale::classic())) { newKey += '-'; }
            newKey += std::tolower(c, std::locale::classic());
        }
        return newKey;
    }

};
