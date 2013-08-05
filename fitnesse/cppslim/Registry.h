//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "FixtureBase.h"
#include "List.h"

namespace slim {

    /// This class is used for doing the static registration of available
    /// fixture classes.
    class Registry {
    public:

        static Registry& Instance();

        /// This method registers the class T. This is required in order to be
        /// able to create objects of type T within the SLIM scripts. Class T
        /// must be derived from slim::Fixture<T> and implement a static method
        /// "T::RegisterMethods()" in which the registration of the class'
        /// constructors and method takes place.
        /// Usage: slim::Registry::Instance().registerFixture<MyFixture>("MyFixture");
        /// Note, that there is also a conveniance macro which hides all the details:
        /// SLIM_REGISTER_FIXTURE(MyFixture)
        template<typename T>
        bool registerFixture(
            std::string className
        ) {
            assert(!m_ctors[className] && "class already registered");

            T::RegisterBaseMethods();
            T::RegisterMethods();
            m_ctors[std::move(className)] = &T::Make;

            return true;
        }

        /// This method gets called by the context in order to create a new object
        /// of the given "className" and with the given parameters passed into
        /// the constructor.
        std::shared_ptr<impl::FixtureBase> create(
            Context& ctx,
            const std::string& className,
            const List& args
        ) {
            // find the correct constructor callback function
            auto ctor = m_ctors.find(className);
            if(ctor == m_ctors.end()) {
                // explicit ctor not found, maybe there is a "catch all" ctor?
                ctor = m_ctors.find("*");
                if(ctor == m_ctors.end()) {
                    throw (Exception() << "cannot create an object of type \'" << className << "\': " << args);
                }
            }

            // call the constructor callback and create the corresponding object
            std::shared_ptr<impl::FixtureBase> obj = ctor->second(ctx,className, args);
            if(!obj) {
                throw (Exception() << "object creation of type \'" << className << "\' failed: " << args);
            }

            return obj;
        }

    private:
        Registry() : m_ctors() { }

        Registry(const Registry& other); // = delete;
        Registry& operator=(const Registry& other); // = delete;

        typedef std::map<std::string, std::function<std::shared_ptr<impl::FixtureBase>(Context&, const std::string&, const List&)> > Constructors;
        Constructors m_ctors;
    };

} // namespace slim
