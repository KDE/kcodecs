include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION}Test ${REQUIRED_QT_VERSION} CONFIG QUIET)

if(NOT TARGET Qt${QT_MAJOR_VERSION}::Test)
    message(STATUS "Qt${QT_MAJOR_VERSION}Test not found, autotests will not be built.")
    return()
endif()

ecm_add_tests(
    kencodingprobertest.cpp
    rfc2047test.cpp
    base45test.cpp
    codectest.cpp
    kemailaddresstest.cpp
    LINK_LIBRARIES KF5::Codecs Qt${QT_MAJOR_VERSION}::Test
)

if (TARGET Qt6::Core5Compat)
    set(_qt5_compat_libs Qt6::Core5Compat) # QTextCodec
endif()

ecm_add_test(
    kcharsetstest.cpp
    LINK_LIBRARIES KF5::Codecs Qt${QT_MAJOR_VERSION}::Test ${_qt5_compat_libs}
)

ecm_add_test(
    ${CMAKE_SOURCE_DIR}/src/kusasciitextcodec.cpp
    kusasciitextcodectest.cpp
    TEST_NAME kusasciitextcodectest
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test ${_qt5_compat_libs}
)
target_include_directories(kusasciitextcodectest
    PRIVATE "${CMAKE_SOURCE_DIR}/src"
)

# Benchmark, compiled, but not run automatically with ctest
add_executable(base64benchmark base64benchmark.cpp)
target_link_libraries(base64benchmark KF5::Codecs Qt${QT_MAJOR_VERSION}::Test)
