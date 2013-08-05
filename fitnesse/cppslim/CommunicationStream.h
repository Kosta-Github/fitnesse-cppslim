//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Communication.h"
#include "Exception.h"

namespace slim {

    /// This class implements the simple communication protocol for reading in SLIM
    /// messages and writing out SLIM results by using C++ IO streams.
    class CommunicationStream :
        public Communication
    {
    public:
        CommunicationStream(
            std::istream& in,
            std::ostream& out
        ) :
            m_in(in), m_out(out)
        { }

        virtual std::string readFrom(size_t numChars) {
            std::string result;
            result.reserve(numChars);

            char c = 0;
            while((result.size() < numChars) && (m_in >> c)) {
                result += c;
            }

            if(result.size() != numChars) {
                throw (Exception() << "failed reading " << numChars << " characters from stream");
            }

            return result;
        }

        virtual bool writeTo(const std::string& chars) {
            m_out << chars;
            return m_out.good();
        }

    private:
        std::istream& m_in;
        std::ostream& m_out;
    };

} // namespace slim
