//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Context.h"
#include "FixtureBase.h"
#include "List.h"
#include "MetaObject.h"
#include "TypeConverters.h"

#include <iostream>
#include <locale>

namespace slim {

    /// Derive from this class in order to specify a new fixture class available for testing.
    /// Usage: class MyFixture : public slim::Fixture<MyFixture> { ... };
    template<typename T>
    class Fixture :
        public impl::FixtureBase
    {
    public:
        Fixture() {
            if(!s_registered) {
                throw (Exception() << "class \'" << typeid(T).name() << "\' not registered");
            }
        }

        static std::shared_ptr<T> Make(
            Context& ctx,
            const std::string& className,
            const List& args
        ) {
            auto ctor = MetaObject().ctors.find(args.elements.size());
            if(ctor == MetaObject().ctors.end()) {
                throw (Exception() << "no matching constructor found for '" << className << "' with the given arguments: " << args);
            }

            std::shared_ptr<T> obj = ctor->second(ctx, args);
            if(!obj) {
                throw (Exception() << "construction failed for '" << className << "' with argument: " << args);
            }

            return obj;
        }

        static void RegisterBaseMethods() {
            RegisterMethod("table",         &Fixture::table);
            RegisterMethod("beginTable",    &Fixture::beginTable);
            RegisterMethod("reset",         &Fixture::reset);
            RegisterMethod("execute",       &Fixture::execute);
            RegisterMethod("endTable",      &Fixture::endTable);
        }

        // can be overridden in derived classes if required
        virtual void table(List args)   { }
        virtual void beginTable()       { }
        virtual void reset()            { }
        virtual void execute()          { }
        virtual void endTable()         { }

    protected:
        virtual List callMethod(
            Context& ctx,
            const std::string& funcName,
            const List& args
        ) {
            T* const obj = dynamic_cast<T*>(this);
            assert(obj); // ensure that this is still the right class

            if(!args.isList) {
                throw (Exception() << "no list of arguments given for method call \'" << funcName << "\': " << args);
            }

            auto method = MetaObject().methods.find(std::make_pair(funcName, args.elements.size()));
            if(method == MetaObject().methods.end()) {
                throw (Exception() << "no matching method \'" << funcName << "\' found for the given arguments: " << args);
            }

            return method->second(ctx, obj, args);
        }

    protected:
        static void RegisterSingleton() {
            AddCtor(0, [](Context& ctx, const List& args) -> std::shared_ptr<T> { Fixture<T>::CheckArgs(0, args); static auto instance = std::make_shared<T>(); return instance; });
        }

    protected:
        static void RegisterCtor0() {
            AddCtor(0, [](Context& ctx, const List& args) -> std::shared_ptr<T> { Fixture<T>::CheckArgs(0, args); return std::make_shared<T>(); });
        }

        template<typename ARG1>
        static void RegisterCtor1() {
            AddCtor(1, [](Context& ctx, const List& args) -> std::shared_ptr<T> { Fixture<T>::CheckArgs(1, args); return std::make_shared<T>(ctx.slim2typeConv<ARG1>(args.elements[0])); });
        }

        template<typename ARG1, typename ARG2>
        static void RegisterCtor2() {
            AddCtor(2, [](Context& ctx, const List& args) -> std::shared_ptr<T> { Fixture<T>::CheckArgs(2, args); return std::make_shared<T>(ctx.slim2typeConv<ARG1>(args.elements[0]), ctx.slim2typeConv<ARG2>(args.elements[1])); });
        }

    protected:
        template<typename S>
        static void RegisterMethod(std::string name, void (S::*method)()) {
            AddMethod(name, 0, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(0, args); (obj->*method)(); return type2slim(ctx); });
        }

        template<typename S>
        static void RegisterMethod(std::string name, void (S::*method)() const) {
            AddMethod(name, 0, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(0, args); (obj->*method)(); return type2slim(ctx); });
        }

        template<typename S, typename RESULT>
        static void RegisterMethod(std::string name, RESULT (S::*method)()) {
            AddMethod(name, 0, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(0, args); return ctx.type2slimConv((obj->*method)()); });
        }

        template<typename S, typename RESULT>
        static void RegisterMethod(std::string name, RESULT (S::*method)() const) {
            AddMethod(name, 0, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(0, args); return ctx.type2slimConv((obj->*method)()); });
        }

        template<typename S, typename ARG1>
        static void RegisterMethod(std::string name, void (S::*method)(ARG1)) {
            AddMethod(name, 1, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(1, args); (obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0])); return type2slim(ctx); });
        }

        template<typename S, typename ARG1>
        static void RegisterMethod(std::string name, void (S::*method)(ARG1) const) {
            AddMethod(name, 1, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(1, args); (obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0])); return type2slim(ctx); });
        }

        template<typename S, typename RESULT, typename ARG1>
        static void RegisterMethod(std::string name, RESULT (S::*method)(ARG1)) {
            AddMethod(name, 1, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(1, args); return ctx.type2slimConv((obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]))); });
        }

        template<typename S, typename RESULT, typename ARG1>
        static void RegisterMethod(std::string name, RESULT (S::*method)(ARG1) const) {
            AddMethod(name, 1, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(1, args); return ctx.type2slimConv((obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]))); });
        }

        template<typename S, typename ARG1, typename ARG2>
        static void RegisterMethod(std::string name, void (S::*method)(ARG1, ARG2)) {
            AddMethod(name, 2, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(2, args); (obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]), ctx.slim2typeConv<ARG2>(args.elements[1])); return type2slim(ctx); });
        }

        template<typename S, typename ARG1, typename ARG2>
        static void RegisterMethod(std::string name, void (S::*method)(ARG1, ARG2) const) {
            AddMethod(name, 2, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(2, args); (obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]), ctx.slim2typeConv<ARG2>(args.elements[1])); return type2slim(ctx); });
        }

        template<typename S, typename RESULT, typename ARG1, typename ARG2>
        static void RegisterMethod(std::string name, RESULT (S::*method)(ARG1, ARG2)) {
            AddMethod(name, 2, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(2, args); return ctx.type2slimConv((obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]), ctx.slim2typeConv<ARG2>(args.elements[1]))); });
        }

        template<typename S, typename RESULT, typename ARG1, typename ARG2>
        static void RegisterMethod(std::string name, RESULT (S::*method)(ARG1, ARG2) const) {
            AddMethod(name, 2, [method](Context& ctx, S* obj, const List& args) -> List { Fixture<T>::CheckArgs(2, args); return ctx.type2slimConv((obj->*method)(ctx.slim2typeConv<ARG1>(args.elements[0]), ctx.slim2typeConv<ARG2>(args.elements[1]))); });
        }

    protected:
        template<typename MEMBER>
        static void RegisterMember(std::string name, MEMBER T::* member) {
            // getter
            AddMethod(name, 0, [member](Context& ctx, T* obj, const List& args) -> List { Fixture<T>::CheckArgs(0, args); return ctx.type2slimConv(obj->*member); });

            // setter
            std::string setterName = "set" + name;
            setterName[3] = std::toupper(setterName[3], std::locale::classic());
            AddMethod(setterName, 1, [member](Context& ctx, T* obj, const List& args) -> List { Fixture<T>::CheckArgs(1, args); (obj->*member) = ctx.slim2typeConv<MEMBER>(args.elements[0]); return type2slim(ctx); });
        }

    private:
        static void AddCtor(size_t argCount, std::function<std::shared_ptr<T>(Context& ctx, const List&)> ctor) {
            if(MetaObject().ctors[argCount]) {
            	MethodAlreadyRegistered("constructor", argCount);
            }
            MetaObject().ctors[argCount] = std::move(ctor);
        }

        static void AddMethod(std::string name, size_t argCount, std::function<List(Context& ctx, T*, const List&)> func) {
            if(MetaObject().methods[std::make_pair(name, argCount)]) {
            	MethodAlreadyRegistered(name, argCount);
            }
            MetaObject().methods[std::pair<std::string, size_t>(std::move(name), argCount)] = std::move(func);
        }

        static void CheckArgs(size_t expected, const List& got) {
            if(!got.isList || (got.elements.size() != expected)) {
                throw (Exception() << "invalid number of arguments; " << expected << " expected but got " << got.elements.size() << ": " << got);
            }
        }

		static void MethodAlreadyRegistered(const std::string& name, size_t argCount) {
            std::cerr << "***** fixture: " << typeid(T).name() << ": method '" << name << "' with " << argCount << " arguments already registered!" << std::endl;
        }
        
    private:
        typedef impl::MetaObject<T> Registry;
        static Registry& MetaObject() { static Registry s_reg; return s_reg; }
        static const bool s_registered;
    };

} // namespace slim

#define SLIM_REGISTER_FIXTURE(FIXTURE) \
    template<> const bool slim::Fixture<FIXTURE>::s_registered = slim::Registry::Instance().registerFixture<FIXTURE>(#FIXTURE);

#define SLIM_DEFINE_FIXTURE(FIXTURE)                \
    struct FIXTURE;                                 \
    SLIM_REGISTER_FIXTURE(FIXTURE)                  \
    struct FIXTURE : public slim::Fixture<FIXTURE>
