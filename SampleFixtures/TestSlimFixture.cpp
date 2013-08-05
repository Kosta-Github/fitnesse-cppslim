//
// Copyright (C) 2012 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct TestSlimFixture : public slim::Fixture<TestSlimFixture> {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterCtor1<std::string>();
        RegisterMethod("createTestSlimWithString",  &TestSlimFixture::createTestSlimWithString);
        RegisterMember("stringArg",                 &TestSlimFixture::stringArg);
        RegisterMethod("isSame",                    &TestSlimFixture::isSame);
        RegisterMethod("stringArgFromOther",        &TestSlimFixture::stringArgFromOther);
    }

    TestSlimFixture(std::string a = "") : stringArg(a) { }

    std::string stringArg;

    std::shared_ptr<TestSlimFixture> createTestSlimWithString(std::string test) {
        return std::make_shared<TestSlimFixture>(test);
    }

    bool isSame(std::shared_ptr<const TestSlimFixture> obj) const {
        return (this == obj.get());
    }

    std::string stringArgFromOther(std::shared_ptr<const TestSlimFixture> obj) const {
        if(!obj) { return "<null>"; }
        return obj->stringArg;
    }

};
SLIM_REGISTER_FIXTURE(TestSlimFixture);
