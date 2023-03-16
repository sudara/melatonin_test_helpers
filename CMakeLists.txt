cmake_minimum_required(VERSION 3.20)

project(MelatoninTestHelpers VERSION 1.0.0 LANGUAGES CXX
        DESCRIPTION "JUCE module for inspecting Components"
        HOMEPAGE_URL "https://github.com/sudara/melatonin_inspector")

if (MelatoninTestHelpers_IS_TOP_LEVEL)
    include(FetchContent)
    message(STATUS "Cloning JUCE...")

    FetchContent_Declare(JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG origin/master
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            FIND_PACKAGE_ARGS 7.0.5)

    FetchContent_MakeAvailable(JUCE)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.3.2)
    FetchContent_MakeAvailable(Catch2) # find_package equivalent

    enable_testing()

    file(GLOB_RECURSE TestFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")

    add_executable(Tests ${TestFiles})

    include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
    catch_discover_tests(Tests)

    # this flag allows parent projects to run tests as well
    target_compile_definitions(Tests PRIVATE RUN_MELATONIN_TESTS=1)

endif ()

if (NOT COMMAND juce_add_module)
    message(FATAL_ERROR "JUCE must be added to your project before melatonin_test_helpers!")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

juce_add_module("${CMAKE_CURRENT_LIST_DIR}")

add_library(Melatonin::TestHelpers ALIAS melatonin_test_helpers)
