cmake_minimum_required(VERSION 3.20)
project(monte_carlo_pricer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Library ----------
add_library(pricer
        src/Option.cpp
        src/MonteCarlo.cpp
        src/BlackScholes.cpp
        src/ImpliedVol.cpp
)
target_include_directories(pricer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MSVC)
    target_compile_options(pricer PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(pricer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------- Demo executable ----------
add_executable(proj_monte_carlo_pricer src/main.cpp)
target_link_libraries(proj_monte_carlo_pricer PRIVATE pricer)

# ---------- Tests (GoogleTest via Git) ----------
option(ENABLE_TESTS "Build unit tests" ON)
if (ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        main
            GIT_SHALLOW    TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)

    if (TEST_SOURCES)
        add_executable(unit_tests ${TEST_SOURCES}
                tests/TestUtils.h
                tests/MathUtilsTest.cpp
                tests/OptionTest.cpp
                tests/BlackScholesTest.cpp
                tests/MonteCarloTest.cpp
                tests/ImpliedVolTest.cpp
                tests/PricerInterfaceTest.cpp
                tests/PutCallParityTest.cpp
                tests/ParamGridTest.cpp
                tests/CachingAndStateTest.cpp)
        target_link_libraries(unit_tests PRIVATE pricer gtest_main)
        target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        include(GoogleTest)
        gtest_discover_tests(unit_tests)
    endif()
endif()

# --------- Documentation (DoxyGen) ---------------
option(BUILD_DOCS "Generate Doxygen docs" OFF)

if (BUILD_DOCS)
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        set(DOXYFILE_IN  ${CMAKE_SOURCE_DIR}/docs/Doxyfile)
        set(DOXYFILE_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
    else()
        message(STATUS "Doxygen not found – docs target will be skipped")
    endif()
endif()

