//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Slim.h"

namespace slim {

    /// This class defines the simple communication protocol for reading in SLIM
    /// messages and writing out SLIM results.
    class Communication {
    public:
        virtual ~Communication() { }

        virtual std::string readFrom(size_t numChars) = 0;
        virtual bool writeTo(const std::string& chars) = 0;
    };

} // namespace slim
