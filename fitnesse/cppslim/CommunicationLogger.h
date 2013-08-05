//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Communication.h"

namespace slim {

    /// This class can be used to log the incoming and outgoing data
    /// for debugging purpose.
    class CommunicationLogger :
        public Communication
    {
    public:
        CommunicationLogger(
            Communication&  master,
            std::ostream*   logReceived = nullptr,
            std::ostream*   logSent     = nullptr
        ) :
            m_master(master),
            m_logReceived(logReceived),
            m_logSent(logSent)
        { }

        virtual std::string readFrom(size_t numChars) {
            auto s = m_master.readFrom(numChars);
            if(m_logReceived) { (*m_logReceived) << s; }
            return s;
        }

        virtual bool writeTo(const std::string& chars) {
            if(m_logSent) { (*m_logSent) << chars; }
            return m_master.writeTo(chars);
        }

    private:
        Communication&      m_master;
        std::ostream* const m_logReceived;
        std::ostream* const m_logSent;
    };

} // namespace slim
