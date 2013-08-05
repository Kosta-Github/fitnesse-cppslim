//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct LoginDialogDriverFixture : public slim::Fixture<LoginDialogDriverFixture> {
    static void RegisterMethods() {
        RegisterCtor2<std::string, std::string>();
        RegisterMethod("loginWithUsernameAndPassword",  &LoginDialogDriverFixture::loginWithUsernameAndPassword);
        RegisterMember("numberOfLoginAttempts",         &LoginDialogDriverFixture::numberOfLoginAttempts);
        RegisterMember("loginMessage",                  &LoginDialogDriverFixture::loginMessage);
    }

    LoginDialogDriverFixture(const std::string& name_, const std::string& pw_) : name(name_), pw(pw_), loginMessage(), numberOfLoginAttempts(0) { }

    bool loginWithUsernameAndPassword(std::string userName, std::string password) {
        numberOfLoginAttempts++;
        const bool result = (userName == name) && (password == pw);
        if (result) {
            loginMessage = userName + " logged in.";
        } else {
            loginMessage = userName + " not logged in.";
        }
        return result;
    }

    std::string name;
    std::string pw;
    std::string loginMessage;
    int numberOfLoginAttempts;
};
SLIM_REGISTER_FIXTURE(LoginDialogDriverFixture);
