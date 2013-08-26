cmake_minimum_required(VERSION 2.8)

project(fitnesse_cppslim_samples C CXX)
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
find_package(Boost REQUIRED COMPONENTS system thread)

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
    SampleFixtures/ShouldBuyMilk.cpp
    SampleFixtures/TestSlim.cpp
)

add_executable(
    SampleFixtures
    SampleFixtures/main.cpp
    ${SRC_fitnesse_cppslim_sample_fixtures}
)

target_link_libraries(
    SampleFixtures
    ${Boost_LIBRARIES}
)


# configure unit tests
enable_testing()

# download fitnesse server package
message("fetching fitnesse server package")
file(
    DOWNLOAD
    http://fitnesse.org/fitnesse-standalone.jar?responder=releaseDownload&release=20130530
    ${CMAKE_CURRENT_BINARY_DIR}/fitnesse-standalone.jar
    EXPECTED_MD5 c357d8717434947ed4dbbf8de51a8016
    SHOW_PROGRESS
)

# add a custom target for creating a symblic links for the FitNesseRoot folder...
add_custom_target(SampleFixturesData ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink \"${CMAKE_CURRENT_SOURCE_DIR}/SampleFixtures/FitNesseRoot\" \"${CMAKE_CURRENT_BINARY_DIR}/FitNesseRoot\"
)

add_test(
    NAME SampleFixturesTest
    COMMAND java -jar fitnesse-standalone.jar -c "SampleFixturesSuite?suite&format=text"
)
