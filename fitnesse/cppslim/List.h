//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Exception.h"

#include <algorithm>

namespace slim {

    class Context;

    class List {
    public:
        explicit List() : isList(true) { }
        explicit List(std::string str) : string(std::move(str)), isList(false) { }

        std::string         string;
        std::vector<List>   elements;
        bool                isList;

        std::string toString() const;
        std::string toSlim() const;

    private:
        friend class Context;

        static std::string  lengthString(size_t len);

        static List         decode(const char*& str);
        static size_t       decodeLength(const char*& str);
        static std::string  decodeString(const char*& str);
        static List         decodeList(const char*& str);
    };

    inline std::ostream& operator<<(std::ostream& out, const List& list) {
        return (out << list.toString());
    }

    namespace impl {

        inline bool isDigit(const char c) {
            return (('0' <= c) && (c <= '9'));
        }

        inline size_t getDigit(const char*& str) {
            if(!isDigit(*str)) {
                throw (Exception() << "not a valid digit: " << str);
            }
            return (*str++ - '0');
        }

        inline void checkExpectedChar(const char*& str, const char c) {
            if(*str != c) {
                throw (Exception() << "expected char '" << c << "' missing: " << str);
            }
            ++str;
        }

        // see: http://en.wikipedia.org/wiki/UTF-8
        inline bool isUtf8LeadingByte(const uint8_t c) {
            return (((c >> 6) & 3) == 3);
        }

        // see: http://en.wikipedia.org/wiki/UTF-8
        inline bool isUtf8ContinuationByte(const uint8_t c) {
            return (((c >> 6) & 3) == 2);
        }

        // ignore all the continuation bytes, see: http://fitnesse.org/FitNesse.UserGuide.SliM.SlimProtocol
        inline size_t calcSlimStringLength(const std::string& s) {
            return std::count_if(
                s.begin(), s.end(), [](const uint8_t c) { return !isUtf8ContinuationByte(c); }
            );
        }

    } // namespace impl

    inline std::string List::toString() const {
        if(!isList) {
            return string;
        }

        std::string out;
        out += "[ ";
        for(size_t i = 0, iEnd = elements.size(); i < iEnd; ++i) {
            if(i > 0) { out += ", "; }
            out += elements[i].toString();
        }
        out += " ]";
        return out;
    }

    inline std::string List::toSlim() const {
        if(!isList) {
            return (lengthString(impl::calcSlimStringLength(string)) + string);
        }

        std::string out;
        out += '[';
        out += lengthString(elements.size());
        for(size_t i = 0, iEnd = elements.size(); i < iEnd; ++i) {
            out += elements[i].toSlim() += ':';
        }
        out += ']';

        return (lengthString(impl::calcSlimStringLength(out)) + out);
    }

    inline std::string List::lengthString(const size_t len) {
        char buf[32];
        ::sprintf(buf, "%012d:", int(len));
        return buf;
    }

    inline size_t List::decodeLength(
        const char*& str
    ) {
        size_t result = 0;
        while(*str != ':') {
            const auto c = impl::getDigit(str);
            if(result > std::numeric_limits<size_t>::max() / 10) {
                throw (Exception() << "length value is too large: " << result << c);
            }
            result = 10 * result + c;
        }
        impl::checkExpectedChar(str, ':');
        return result;
    }

    inline List List::decode(
        const char*& str
    ) {
        if(*str == '[') {
            return decodeList(str);
        } else {
            std::string s = decodeString(str);
            if((s.length() > 2) && (s.front() == '[') && (s.back() == ']')) {
                // decode string list recursively
                const char* str2 = s.c_str();
                return decodeList(str2);
            }
            return List(std::move(s));
        }
    }

    inline std::string List::decodeString(const char*& str) {
        const size_t len = decodeLength(str);

        std::string result;
        result.reserve(len);

        for(size_t i = 0; i < len; ++i) {
            if(*str == 0x00) {
                throw (Exception() << "parsed string is too short");
            }

            const char c = *str++;
            result.push_back(c);

            // handle multi-byte unicode chars
            if(impl::isUtf8LeadingByte(c)) {
                while(impl::isUtf8ContinuationByte(*str)) {
                    result.push_back(*str++);
                }
            }
        }

        impl::checkExpectedChar(str, ':');

        return result;
    }

    inline List List::decodeList(const char*& str) {
        impl::checkExpectedChar(str, '[');
        const size_t len = decodeLength(str);

        List result;
        result.elements.reserve(len);
        for(size_t i = 0; i < len; ++i) {
            result.elements.push_back(decode(str));
        }

        impl::checkExpectedChar(str, ']');

        return result;
    }

} // namespace slim
