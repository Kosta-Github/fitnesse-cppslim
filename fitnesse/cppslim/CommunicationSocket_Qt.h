//
// Copyright (C) 2014 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Communication.h"
#include "Exception.h"

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

#include <memory>

namespace slim {

    /// This class implements the simple communication protocol for reading in SLIM
    /// messages and writing out SLIM results by using the Qt's TCP socket
    /// implementation.
    /// TODO: This should be changed in the future to use a native platform socket
    /// implementation in order to remove the Qt dependency again.
    class CommunicationSocket_Qt :
        public Communication
    {
    public:
        enum TimeOut { Value = 5 * 1000 }; // 5 seconds

        CommunicationSocket_Qt(uint16_t port) {
            m_server.listen(QHostAddress::Any, static_cast<quint16>(port));
            m_server.waitForNewConnection(TimeOut::Value);
            m_socket.reset(m_server.nextPendingConnection());
        }

        virtual ~CommunicationSocket_Qt() {
            if(m_socket) {
                m_socket->disconnectFromHost();
                if(m_socket->state() != QTcpSocket::UnconnectedState) {
                    m_socket->waitForDisconnected(TimeOut::Value);
                }
            }
        }

        virtual std::string readFrom(size_t numChars) {
            assert(numChars > 0);

            if(!m_socket || !m_socket->isReadable()) {
                throw (Exception() << "failed to connect to the SLIM server");
            }

            std::vector<char> buffer(numChars);
            size_t readChars = 0;
            while(readChars < numChars) {
                m_socket->waitForReadyRead(TimeOut::Value);

                const auto readCur = m_socket->read(buffer.data() + readChars, numChars - readChars);
                if(readCur < 0) {
                    throw (Exception() << "failed reading " << numChars << " characters from socket");
                }

                readChars += readCur;
            }

            if(readChars != buffer.size()) {
                throw (Exception() << "failed reading " << numChars << " characters from socket");
            }

            return std::string(buffer.data(), buffer.size());
        }

        virtual bool writeTo(const std::string& chars) {
            if(!m_socket || !m_socket->isWritable()) {
                throw (Exception() << "failed to connect to the SLIM server");
            }

            const size_t writtenChars = m_socket->write(chars.data(), chars.size());
            m_socket->waitForBytesWritten(TimeOut::Value);

            return (writtenChars == chars.size());
        }

    private:
        QTcpServer m_server;
        std::unique_ptr<QTcpSocket> m_socket;
    };

} // namespace slim
