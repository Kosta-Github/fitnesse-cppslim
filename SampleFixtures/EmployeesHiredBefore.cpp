//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct EmployeesHiredBefore : public slim::Fixture<EmployeesHiredBefore> {
    static void RegisterMethods() {
        RegisterCtor1<std::string>();
        RegisterMethod("query", &EmployeesHiredBefore::query);
    }

    EmployeesHiredBefore(const std::string& date) { }

    std::vector<std::vector<std::vector<std::string> > > query() const {
        std::vector<std::vector<std::vector<std::string> > > result;

        result.resize(2);
        result[0].resize(4);
        result[0][0].push_back("employee number");  result[0][0].push_back("1429");
        result[0][1].push_back("first name");       result[0][1].push_back("Bob");
        result[0][2].push_back("last name");        result[0][2].push_back("Martin");
        result[0][3].push_back("hire date");        result[0][3].push_back("10-Oct-1974");

        result[1].resize(4);
        result[1][0].push_back("employee number");  result[1][0].push_back("8832");
        result[1][1].push_back("first name");       result[1][1].push_back("James");
        result[1][2].push_back("last name");        result[1][2].push_back("Grenning");
        result[1][3].push_back("hire date");        result[1][3].push_back("15-Dec-1979");

        return result;
    }
};
SLIM_REGISTER_FIXTURE(EmployeesHiredBefore);
