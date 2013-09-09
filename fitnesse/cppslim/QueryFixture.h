//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Fixture.h"

namespace slim {

    /// Derive from this class and implement the query() method in case you want to deal with
    /// SliM query tables.
    /// Usage: SLIM_QUERY_DECISION_FIXTURE(MyQueryFixture) { ... };
    template<typename T>
    class QueryFixture :
        public slim::Fixture<T>
    {
    protected:
        typedef QueryFixture BaseClass;
    public:
    	/// Implement this method and call the addRow()-method for adding the individual rows
    	/// for the final table.
        virtual void query() = 0;

		/// Call this method to add rows to the resulting output table.
		virtual void addRow(std::map<std::string, std::string> row) {
			m_outputTable.push_back(std::move(row));
		}
        
        static void RegisterBaseMethods() {
            slim::Fixture<T>::RegisterBaseMethods();
            slim::Fixture<T>::RegisterMethod("query", &QueryFixture::queryImpl);
        }

    private:
        std::vector<std::map<std::string, std::string>> queryImpl() {
            m_outputTable.clear();
            query();
            return std::move(m_outputTable);
        }
        
        std::vector<std::map<std::string, std::string>> m_outputTable;
    };

} // namespace slim

#define SLIM_DEFINE_QUERY_FIXTURE(FIXTURE)              \
    struct FIXTURE;                                     \
    SLIM_REGISTER_FIXTURE(FIXTURE)                      \
    struct FIXTURE : public slim::QueryFixture<FIXTURE>