//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Communication.h"
#include "Exception.h"

#include <boost/asio.hpp>

namespace slim {

    /// This class implements the simple communication protocol for reading in SLIM
    /// messages and writing out SLIM results by using the boost asio socket
    /// implementation.
    /// TODO: This should be changed in the future to use a native platform socket
    /// implementation in order to remove the boost dependency again.
    class CommunicationSocket :
        public Communication
    {
    public:
        CommunicationSocket(uint16_t port) :
            m_ioService(),
            m_acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
            m_socket(m_ioService)
        {
            m_acceptor.accept(m_socket);
        }

        virtual std::string readFrom(size_t numChars) {
            assert(numChars > 0);

            std::vector<char> buffer(numChars);
            const size_t readChars = boost::asio::read(m_socket, boost::asio::buffer(buffer));

            if(readChars != buffer.size()) {
                throw (Exception() << "failed reading " << numChars << " characters from socket");
            }

            return std::string(buffer.data(), buffer.size());
        }

        virtual bool writeTo(const std::string& chars) {
            const size_t writtenChars = boost::asio::write(m_socket, boost::asio::buffer(chars), boost::asio::transfer_all());
            return (writtenChars == chars.size());
        }

    private:
        boost::asio::io_service         m_ioService;
        boost::asio::ip::tcp::acceptor  m_acceptor;
        boost::asio::ip::tcp::socket    m_socket;
    };

} // namespace slim
