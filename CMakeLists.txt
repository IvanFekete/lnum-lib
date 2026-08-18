cmake_minimum_required(VERSION 3.14)
project(lnum-lib VERSION 1.0.0 LANGUAGES CXX)

add_library(lnum-lib INTERFACE)
add_library(lnum::lnum-lib ALIAS lnum-lib)

target_include_directories(lnum-lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(lnum-lib INTERFACE cxx_std_11)

include(GNUInstallDirs)

install(TARGETS lnum-lib EXPORT lnum-lib-targets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT lnum-lib-targets
    FILE lnum-lib-targets.cmake
    NAMESPACE lnum::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lnum-lib)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/lnum-lib-config-version.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lnum-lib-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lnum-lib-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lnum-lib)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/lnum-lib-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lnum-lib-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lnum-lib)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LNUM_LIB_IS_TOP_LEVEL ON)
else()
    set(LNUM_LIB_IS_TOP_LEVEL OFF)
endif()
option(LNUM_LIB_BUILD_TESTS "Build lnum-lib's own test suite" ${LNUM_LIB_IS_TOP_LEVEL})
if(LNUM_LIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
