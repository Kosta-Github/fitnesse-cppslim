//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

SLIM_DEFINE_FIXTURE(Bowling) {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterMethod("doTable", &Bowling::doTable);
    }

    slim::List doTable(slim::List table) const {
        dummyProcessTable(table);
        return table;
    }

    static void dummyProcessTable(slim::List& table) {
        if(!table.isList) {
            table.string = "pass"; // in case of a value mismatch put "fail" into the cell...
        }
        for(size_t i = 0, iEnd = table.elements.size(); i < iEnd; ++i) {
            dummyProcessTable(table.elements[i]);
        }
    }
};
