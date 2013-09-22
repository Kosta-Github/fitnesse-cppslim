//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/TableFixture.h"
#include "fitnesse/cppslim/Registry.h"

SLIM_DEFINE_TABLE_FIXTURE(SimpleTable) {
public:
    static void RegisterMethods() {
        RegisterSingleton();
    }

    virtual void query(const size_t columns, const size_t rows) {
    	for(size_t j = 0; j < rows; ++j) {
    	    for(size_t i = 0; i < columns; ++i) {
    	    	set(i, j, RT_Pass, std::to_string(i) + "/" + std::to_string(j) + ": " + get(i, j));
    	    }
    	}
    }

};
