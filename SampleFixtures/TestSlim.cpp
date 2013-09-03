//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/DecisionFixture.h"
#include "fitnesse/cppslim/Registry.h"

SLIM_DEFINE_FIXTURE(TestSlim) {
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
