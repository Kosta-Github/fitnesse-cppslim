//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Exception.h"
#include "FixtureBase.h"

namespace slim {

    /// Helper class for wrapping an arbitrary class/object.
    /// No own method calls (via SLIM) will be supported.
    /// It is only used for object lifetime management.
    template<typename T>
    class ObjectWrapper :
        public impl::FixtureBase
    {
    public:
        ObjectWrapper(T o) : obj(std::move(o)) { }

        // provide easy and direct access to the wrapped object
        T obj;

        // just throw on each call on this one...
        virtual List callMethod(Context& ctx, const std::string& funcName, const List& args) {
            throw Exception() << "wrapped object does not support function calls";
        }
    };

} // namespace slim
