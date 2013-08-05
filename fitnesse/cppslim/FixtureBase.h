//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Slim.h"

namespace slim {

    class Context;
    class List;

    namespace impl {

        /// Internal base class for the templated Fixture<T> class.
        class FixtureBase {
        public:
            virtual ~FixtureBase() { }

            virtual List callMethod(Context& ctx, const std::string& funcName, const List& args) = 0;
        };

    } // namespace impl

} // namespace slim
