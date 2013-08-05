//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#include "fitnesse/cppslim/Fixture.h"
#include "fitnesse/cppslim/Registry.h"

struct ShouldIBuyMilkFixture : public slim::Fixture<ShouldIBuyMilkFixture> {
    static void RegisterMethods() {
        RegisterCtor0();
        RegisterMember("cashInWallet",          &ShouldIBuyMilkFixture::cashInWallet);
        RegisterMember("creditCard",            &ShouldIBuyMilkFixture::creditCard);
        RegisterMember("pintsOfMilkRemaining",  &ShouldIBuyMilkFixture::pintsOfMilkRemaining);
        RegisterMethod("goToStore",             &ShouldIBuyMilkFixture::goToStore);
    }

    ShouldIBuyMilkFixture() : cashInWallet(0.0), creditCard(false), pintsOfMilkRemaining(0) { }

    double cashInWallet;
    bool creditCard;
    int pintsOfMilkRemaining;

    bool goToStore() const { return ((pintsOfMilkRemaining == 0) && ((cashInWallet > 2.0) || creditCard)); }
};
SLIM_REGISTER_FIXTURE(ShouldIBuyMilkFixture);
