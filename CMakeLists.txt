cmake_minimum_required(VERSION 2.8)

project(fitnesse_cppslim_samples C CXX)
message("configure: fitnesse/cppslim/SampleFixtures")

# Check for cxx0x, cxx11, and libcxx11 options
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" HAS_CXX0x)
CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++11 -stdlib=libc++" HAS_LIBCXX11)

# switch to the right compiler
if(HAS_LIBCXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif(HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(HAS_CXX0x)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(MSVC)
    add_definitions(
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -DWINVER=0x0501
        -D_WIN32_WINNT=0x0501
        -DNOMINMAX
    )
endif()

# configure boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON )
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS system thread date_time regex)

include_directories(
    .
    ${Boost_INCLUDE_DIRS}
)

add_definitions(
)

set(
    SRC_fitnesse_cppslim_sample_fixtures
    SampleFixtures/Bowling.cpp
    SampleFixtures/EmployeesHiredBefore.cpp
    SampleFixtures/LoginDialogDriver.cpp
    SampleFixtures/SimpleDecision.cpp
    SampleFixtures/SimpleQuery.cpp
    SampleFixtures/SimpleTable.cpp
    SampleFixtures/ShouldBuyMilk.cpp
    SampleFixtures/TestSlim.cpp
)

set(
    HDR_fitnesse_cppslim
    fitnesse/cppslim/Communication.h
    fitnesse/cppslim/CommunicationLogger.h
    fitnesse/cppslim/CommunicationSocket.h
    fitnesse/cppslim/CommunicationStream.h
    fitnesse/cppslim/Context.h
    fitnesse/cppslim/ContextImpl.h
    fitnesse/cppslim/DecisionFixture.h
    fitnesse/cppslim/Exception.h
    fitnesse/cppslim/Fixture.h
    fitnesse/cppslim/FixtureBase.h
    fitnesse/cppslim/List.h
    fitnesse/cppslim/MetaObject.h
    fitnesse/cppslim/ObjectWrapper.h
    fitnesse/cppslim/QueryFixture.h
    fitnesse/cppslim/Registry.h
    fitnesse/cppslim/Slim.h
    fitnesse/cppslim/TableFixture.h
    fitnesse/cppslim/TypeConverters.h
)

add_executable(
     SampleFixtures
     SampleFixtures/main.cpp
     ${SRC_fitnesse_cppslim_sample_fixtures}
     ${HDR_fitnesse_cppslim}
)
 
target_link_libraries(
    SampleFixtures
    ${Boost_LIBRARIES} pthread
)


# configure unit tests
enable_testing()

# download fitnesse server package
set(FITNESSE_SERVER_JAR fitnesse-standalone-20130530.jar)
#message("fetching fitnesse server package")
#file(
#    DOWNLOAD
#    http://fitnesse.org/fitnesse-standalone.jar?responder=releaseDownload&release=20130530
#    ${CMAKE_CURRENT_BINARY_DIR}/fitnesse-standalone.jar
#    EXPECTED_MD5 c357d8717434947ed4dbbf8de51a8016
#    SHOW_PROGRESS
#)

# add a custom target for creating a symblic links for the FitNesseRoot folder...
add_custom_target(SampleFixturesData ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink \"${CMAKE_CURRENT_SOURCE_DIR}/${FITNESSE_SERVER_JAR}\" \"${CMAKE_CURRENT_BINARY_DIR}/${FITNESSE_SERVER_JAR}\"
    COMMAND ${CMAKE_COMMAND} -E create_symlink \"${CMAKE_CURRENT_SOURCE_DIR}/SampleFixtures/FitNesseRoot\" \"${CMAKE_CURRENT_BINARY_DIR}/FitNesseRoot\"
)

add_test(
    NAME SampleFixturesTest_Call_from_FitNesse_Server
    COMMAND java -DSLIM_PORT=8989 -jar ./${FITNESSE_SERVER_JAR} -c "SampleFixturesSuite?suite&format=text"
)

#add_test(
#    NAME SampleFixturesTest_Call_Directly
#    COMMAND ./SampleFixtures -test "SampleFixturesSuite?suite" -port 9999
#)
