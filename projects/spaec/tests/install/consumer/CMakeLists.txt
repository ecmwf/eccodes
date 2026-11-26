# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# A user project of the install test (install_test.cmake): finds the installed package
cmake_minimum_required(VERSION 3.19)
project(spaec_consumer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(spaec ${REQUIRED_VERSION} REQUIRED)
add_executable(consumer main.cc)
target_link_libraries(consumer PRIVATE spaec::spaec)
