include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11)
# doctest v2.4.11's own CMakeLists.txt declares cmake_minimum_required(2.8.11),
# which CMake >= 4 refuses to configure at all; raise the floor just for this
# fetched dependency rather than for our own project.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
# It's a test-only dependency: don't let its own install() rules leak into
# `cmake --install` for lnum-lib.
set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(doctest)
unset(CMAKE_POLICY_VERSION_MINIMUM)

add_executable(lnum-lib-tests test_lnum.cpp)
target_link_libraries(lnum-lib-tests PRIVATE lnum::lnum-lib doctest::doctest)
target_compile_features(lnum-lib-tests PRIVATE cxx_std_11)
# Only our own sources, not the fetched doctest target, are held to this bar.
target_compile_options(lnum-lib-tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(lnum-lib-tests)
