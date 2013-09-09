//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "List.h"

namespace slim {

    inline List type2slim(Context& ctx) { return List(std::string("__VOID__")); }

    inline List type2slim(Context& ctx, const std::string& str) { return List(str); }

    inline const List& type2slim(Context& ctx, const List& list) { return list; }

    inline List type2slim(Context& ctx, const bool& b) { return List(std::string(b ? "true" : "false")); }

    inline List type2slim(Context& ctx, const int64_t&     i) { return List(std::to_string(i)); }
    inline List type2slim(Context& ctx, const uint64_t&    i) { return List(std::to_string(i)); }
    inline List type2slim(Context& ctx, const double&      d) { std::ostringstream buf; buf.precision(17); buf << d; return List(buf.str()); }

    inline List type2slim(Context& ctx, const int8_t&          i) { return type2slim(ctx, int64_t(i));     }
    inline List type2slim(Context& ctx, const uint8_t&         i) { return type2slim(ctx, uint64_t(i));    }
    inline List type2slim(Context& ctx, const int16_t&         i) { return type2slim(ctx, int64_t(i));     }
    inline List type2slim(Context& ctx, const uint16_t&        i) { return type2slim(ctx, uint64_t(i));    }
    inline List type2slim(Context& ctx, const int32_t&         i) { return type2slim(ctx, int64_t(i));     }
    inline List type2slim(Context& ctx, const uint32_t&        i) { return type2slim(ctx, uint64_t(i));    }
    inline List type2slim(Context& ctx, const float&           f) { return type2slim(ctx, double(f));      }

    template<typename T>
    inline List type2slim(Context& ctx, const std::vector<T>& vec);

    template<typename K, typename V>
    inline List type2slim(Context& ctx, const std::map<K, V>& m);

    inline void slim2type(Context& ctx, const List& list, bool& result) {
        if(!list.isList && (list.string == "true"))  { result = true;  return; }
        if(!list.isList && (list.string == "false")) { result = false; return; }
        throw (Exception() << "\'" << list << "\' is not a bool value");
    }

    inline void slim2type(Context& ctx, const List& list, std::string& result) { result = list.toString(); }

    inline void slim2type(Context& ctx, const List& list, List& result) { result = list; }

    namespace impl {

        template<typename T>
        inline void slim2numberType(Context& ctx, const List& list, T& result) {
            std::istringstream str(list.string);
            std::string dummy;
            str >> result >> dummy;
            if(list.isList || !dummy.empty()) {
                throw (Exception() << "\'" << list << "\' is not a valid \'" << typeid(T).name() << "\'");
            }
        }

    } // namespace impl
    
} // namespace slim

#include "Context.h"

namespace slim {
    
    template<typename T>
    inline List type2slim(Context& ctx, const std::vector<T>& vec) {
        List result;
        for(size_t i = 0, iEnd = vec.size(); i < iEnd; ++i) {
            result.elements.push_back(ctx.type2slimConv(vec[i]));
        }
        return result;
    }
    
    template<typename K, typename V>
    inline List type2slim(Context& ctx, const std::map<K, V>& m) {
        List result;
        for(auto&& i : m) {
            List item;
            item.elements.push_back(ctx.type2slimConv(i.first));
            item.elements.push_back(ctx.type2slimConv(i.second));
            result.elements.push_back(std::move(item));
        }
        return result;
    }
    
    inline void slim2type(Context& ctx, const List& list, int8_t&          result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, uint8_t&         result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, int16_t&         result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, uint16_t&        result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, int32_t&         result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, uint32_t&        result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, int64_t&         result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, uint64_t&        result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, float&           result) { impl::slim2numberType(ctx, list, result); }
    inline void slim2type(Context& ctx, const List& list, double&          result) { impl::slim2numberType(ctx, list, result); }


    template<typename T>
    inline List type2slim(Context& ctx, const std::shared_ptr<T>& obj) {
        return ctx.object2slim(obj);
    }

    template<typename T>
    inline void slim2type(Context& ctx, const List& list, std::shared_ptr<T>& obj) {
        ctx.slim2object<T>(list, obj);
    }

} // namespace slim
