include(ECMAddTests)

find_package(Qt5Test ${REQUIRED_QT_VERSION} CONFIG QUIET)

if(NOT Qt5Test_FOUND)
    message(STATUS "Qt5Test not found, autotests will not be built.")
    return()
endif()

ecm_add_tests(
    kcharsetstest.cpp
    kencodingprobertest.cpp
    base64benchmark.cpp
    rfc2047test
    codectest.cpp
    LINK_LIBRARIES KF5::Codecs Qt5::Test
)
