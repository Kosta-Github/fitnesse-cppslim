//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include "Fixture.h"

namespace slim {

    /// Derive from this class and implement the query() method in case you want to deal with
    /// SliM table tables.
    /// Usage: SLIM_DEFINE_TABLE_FIXTURE(MyQueryFixture) { ... };
    template<typename T>
    class TableFixture :
        public slim::Fixture<T>
    {
    protected:
        typedef TableFixture BaseClass;
    public:
    
    	enum ResultType {
    		RT_Pass,	// green
    		RT_Warn,	// yellow
    		RT_Fail,	// red
    		RT_Ignore,	// grey
    		RT_Report	// no color
    	};
    	
    	/// Implement this method and call the addRow()-method for adding the individual rows
    	/// for the final table.
        virtual void query(size_t columns, size_t rows) = 0;

        static void RegisterBaseMethods() {
            slim::Fixture<T>::RegisterBaseMethods();
            slim::Fixture<T>::RegisterMethod("doTable", &TableFixture::queryImpl);
        }

	protected:
		const std::string& get(const size_t i, const size_t j) const {
			return m_inputTable.at(j).at(i);
		}
		
		void set(const size_t i, const size_t j, const ResultType type, std::string message = "") {
			m_outputTable.at(j).at(i) = CreateResultMessage(type, std::move(message));
		}
		
    private:
        std::vector<std::vector<std::string>> queryImpl(std::vector<std::vector<std::string>> inputTable) {
        	m_inputTable = std::move(inputTable);
            m_outputTable.clear();

			// initialize the output table with ignore for all cells            
            const std::string ignoreMessage = CreateResultMessage(RT_Ignore);
            for(auto&& i : m_inputTable) {
            	m_outputTable.emplace_back(i.size(), ignoreMessage);
            }
            
            query(m_outputTable.front().size(), m_outputTable.size());
            
            return std::move(m_outputTable);
        }
        
        static std::string CreateResultMessage(const ResultType type, std::string message = "") {
        	if(!message.empty()) { message = ":" + message; }
        	switch(type) {
        		case RT_Pass: 	return "pass" 	+ message;
        		case RT_Warn: 	return "error" 	+ message;
        		case RT_Fail: 	return "fail" 	+ message;
        		case RT_Ignore: return "ignore" + message;
        		case RT_Report:	return "report"	+ message;
        		default: 		return "fail" 	+ message;
        	}
        }
        
    	std::vector<std::vector<std::string>> m_inputTable;
        std::vector<std::vector<std::string>> m_outputTable;
    };

} // namespace slim

#define SLIM_DEFINE_TABLE_FIXTURE(FIXTURE)              \
    struct FIXTURE;                                     \
    SLIM_REGISTER_FIXTURE(FIXTURE)                      \
    struct FIXTURE : public slim::TableFixture<FIXTURE>
