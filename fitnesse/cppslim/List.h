//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Exception.h"

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

        static List decode(const std::string& str);

    private:
        friend class Context;
        static List decode(const char* str, int begin, int end);
        static size_t decodeLength(const char* str, int begin, int end);
    };

    inline std::ostream& operator<<(std::ostream& out, const List& list) {
        return (out << list.toString());
    }

    namespace impl {

        inline bool isNum(char c) { return (('0' <= c) && (c <= '9')); }

        inline bool isList(const char* str, int begin, int end) {
            return (
                (end - begin >= 10) &&
                (str[begin + 0] == '[') &&
                (str[begin + 7] == ':') &&
                (str[end   - 2] == ':') &&
                (str[end   - 1] == ']')
            );
        }

    } // namespace impl

    inline std::string List::toString() const {
        if(!isList) {
            return string;
        }

        std::ostringstream out;
        out << "[ ";
        for(size_t i = 0, iEnd = elements.size(); i < iEnd; ++i) {
            if(i > 0) { out << ", "; }
            out << elements[i];
        }
        out << " ]";
        return out.str();
    }

    inline std::string List::toSlim() const {
        char buf[32];
        if(!isList) {
            ::sprintf(buf, "%06d", int(string.size()));
            return (std::string(buf) + ':' + string);
        }

        std::ostringstream out;
        out << '[';

        ::sprintf(buf, "%06d", int(elements.size()));
        out << buf << ':';

        for(size_t i = 0, iEnd = elements.size(); i < iEnd; ++i) {
            out << elements[i].toSlim() << ':';
        }
        out << ']';
        const std::string combined = out.str();

        ::sprintf(buf, "%06d", int(combined.size()));
        return (std::string(buf) + ':' + combined);

        return out.str();
    }

    inline size_t List::decodeLength(
        const char* str, int begin, int end
    ) {
        if(
            (end - begin != 7) ||
            !impl::isNum(str[begin + 0]) || !impl::isNum(str[begin + 1]) || !impl::isNum(str[begin + 2]) ||
            !impl::isNum(str[begin + 3]) || !impl::isNum(str[begin + 4]) || !impl::isNum(str[begin + 5]) ||
            (str[begin + 6] != ':')
        ) {
            throw (Exception() << "not a valid length encoding: " << std::string(str + begin, str + end));
        }

        return (
            size_t(str[begin + 0] - '0') * 100000 + size_t(str[begin + 1] - '0') * 10000 + size_t(str[begin + 2] - '0') * 1000 +
            size_t(str[begin + 3] - '0') *    100 + size_t(str[begin + 4] - '0') *    10 + size_t(str[begin + 5] - '0') *    1
        );
    }

    inline List List::decode(
        const std::string& str
    ) {
        return decode(str.c_str(), 0, static_cast<int>(str.length()));
    }

    inline List List::decode(
        const char* str, int begin, int end
    ) {
        assert(begin <= end);

        if(!impl::isList(str, begin, end)) {
            return List(std::string(str + begin, str + end));
        }

        List result;

        begin += 1; // skip opening '['
        end   -= 1; // skip closing ']'

        // decode list length
        const size_t listLen = decodeLength(str, begin, begin + 7);
        begin += 7;

        for(size_t i = 0; i < listLen; ++i) {
            const size_t subLen = decodeLength(str, begin, begin + 7);
            begin += 7;

            if((subLen > size_t(end - begin)) || (str[begin + subLen] != ':')) {
                throw (Exception() << "invalid list entry detected: " << std::string(str + begin, str + end));
            }

            result.elements.emplace_back(decode(str, begin, static_cast<int>(begin + subLen)));
            begin += subLen + 1; // skip string plus terminating ':'
        }

        return result;
    }

} // namespace slim
