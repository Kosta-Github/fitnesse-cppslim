//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct ShouldBuyMilk : public slim::Fixture<ShouldBuyMilk> {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterMember("cashInWallet",          &ShouldBuyMilk::cashInWallet);
        RegisterMember("creditCard",            &ShouldBuyMilk::creditCard);
        RegisterMember("pintsOfMilkRemaining",  &ShouldBuyMilk::pintsOfMilkRemaining);
        RegisterMethod("goToStore",             &ShouldBuyMilk::goToStore);
    }

    ShouldBuyMilk() : cashInWallet(0.0), creditCard(false), pintsOfMilkRemaining(0) { }

    double cashInWallet;
    bool creditCard;
    int pintsOfMilkRemaining;

    bool goToStore() const { return ((pintsOfMilkRemaining == 0) && ((cashInWallet > 2.0) || creditCard)); }
};
SLIM_REGISTER_FIXTURE(ShouldBuyMilk);
