include(ECMAddTests)

find_package(Qt5Test ${REQUIRED_QT_VERSION} CONFIG QUIET)

if(NOT Qt5Test_FOUND)
    message(STATUS "Qt5Test not found, autotests will not be built.")
    return()
endif()

ecm_add_tests(
    kcharsetstest.cpp
    kencodingprobertest.cpp
    rfc2047test.cpp
    base45test.cpp
    codectest.cpp
    kemailaddresstest.cpp
    LINK_LIBRARIES KF5::Codecs Qt5::Test
)

ecm_add_test(
    ${CMAKE_SOURCE_DIR}/src/kusasciitextcodec.cpp
    kusasciitextcodectest.cpp
    TEST_NAME kusasciitextcodectest
    LINK_LIBRARIES Qt5::Test
)
target_include_directories(kusasciitextcodectest
    PRIVATE "${CMAKE_SOURCE_DIR}/src"
)

# Benchmark, compiled, but not run automatically with ctest
add_executable(base64benchmark base64benchmark.cpp)
target_link_libraries(base64benchmark KF5::Codecs Qt5::Test)
