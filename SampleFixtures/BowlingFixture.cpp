//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct BowlingFixture : public slim::Fixture<BowlingFixture> {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterMethod("doTable", &BowlingFixture::doTable);
    }

    slim::List doTable(slim::List table) const {
        bool passOrFail = true;
        dummyProcessTable(table, passOrFail);
        return table;
    }

    static void dummyProcessTable(slim::List& table, bool& passOrFail) {
        if(!table.isList) { table.string = (passOrFail ? "pass" : "fail"); passOrFail = !passOrFail; }
        for(size_t i = 0, iEnd = table.elements.size(); i < iEnd; ++i) {
            dummyProcessTable(table.elements[i], passOrFail);
        }
    }
};
SLIM_REGISTER_FIXTURE(BowlingFixture);
