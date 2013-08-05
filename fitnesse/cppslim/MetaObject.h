//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Slim.h"

namespace slim {

    class Context;
    class List;

    namespace impl {

        template<typename T>
        struct MetaObject {
            std::map<size_t, std::function<std::shared_ptr<T>(Context&, const List&)> > ctors;
            std::map<std::pair<std::string, size_t>, std::function<List(Context&, T*, const List&)> > methods;
        };

    } // namespace impl

} // namespace slim
