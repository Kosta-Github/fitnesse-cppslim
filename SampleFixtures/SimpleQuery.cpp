//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/QueryFixture.h"
#include "fitnesse/cppslim/Registry.h"

SLIM_DEFINE_QUERY_FIXTURE(SimpleQuery) {
public:
    static void RegisterMethods() {
        RegisterSingleton();
    }

    virtual void query() {
        std::map<std::string, std::string> row;
        row["a"] = "1";
        row["b"] = "2";
        row["c"] = "3";
        addRow(row);

        row["a"] = "xx";
        row["b"] = "yy";
        row["c"] = "zz";
        addRow(row);
    }

};
