cmake_minimum_required(VERSION 2.8)

project(fitnesse_cppslim C CXX)
message("configure: fitnesse/cppslim/SampleFixtures")

# switch to the right compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(APPLE)

# configure boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON )
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS system)

# configure pthread lib
find_package(Threads REQUIRED)

include_directories(
    ../
    ${Boost_INCLUDE_DIRS}
)

add_definitions(
)

set(
    SRC_fitnesse_cppslim_sample_fixtures
    BowlingFixture.cpp
    EmployeesHiredBeforeFixture.cpp
    LoginDialogDriverFixture.cpp
    ShouldIBuyMilkFixture.cpp
    TestSlimFixture.cpp
)

add_executable(
    SampleFixtures
    main.cpp
    ${SRC_fitnesse_cppslim_sample_fixtures}
)

target_link_libraries(
    SampleFixtures
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)


# configure unit tests
enable_testing()

# download fitnesse server package
file(
    DOWNLOAD
    http://fitnesse.org/fitnesse-standalone.jar?responder=releaseDownload&release=20130530
    ${CMAKE_CURRENT_BINARY_DIR}/fitnesse-standalone.jar
    EXPECTED_MD5 c357d8717434947ed4dbbf8de51a8016
)

add_test(
    NAME SampleFixturesTest
    COMMAND java -jar fitnesse-standalone.jar -c "?names"
)
