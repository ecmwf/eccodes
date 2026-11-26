# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# Install test of spaec (cmake -P): installs the build into a temporary prefix, checks the installed
# files, builds and runs a user project that finds the package with find_package(spaec <version>),
# and checks that a request for the next minor version is rejected (SameMinorVersion before 1.0.0).
# Variables: BUILD_DIR, WORK_DIR, CONSUMER_DIR, GENERATOR, CXX_COMPILER, VERSION, CONFIG

function(run what)
  execute_process(COMMAND ${ARGN} RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT rc EQUAL 0)
    message(FATAL_ERROR "${what} failed (${rc}):\n${out}\n${err}")
  endif()
  set(run_output "${out}" PARENT_SCOPE)
endfunction()

set(prefix "${WORK_DIR}/prefix")
file(REMOVE_RECURSE "${WORK_DIR}")
set(config_args "")
if(CONFIG)
  set(config_args --config "${CONFIG}")
endif()
run("Installing" "${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${prefix}" ${config_args})

foreach(file include/spaec/spaec.h include/spaec/version.h share/doc/spaec/LICENSE share/doc/spaec/CHANGELOG.md)
  if(NOT EXISTS "${prefix}/${file}")
    message(FATAL_ERROR "Not installed: ${file}")
  endif()
endforeach()
file(GLOB_RECURSE config_files "${prefix}/*/spaecConfigVersion.cmake")
if(NOT config_files)
  message(FATAL_ERROR "Not installed: spaecConfigVersion.cmake")
endif()

# The user project with the version of the build
set(consumer "${WORK_DIR}/consumer")
run("Configuring the user project" "${CMAKE_COMMAND}" -S "${CONSUMER_DIR}" -B "${consumer}" -G "${GENERATOR}"
    "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}" "-DCMAKE_PREFIX_PATH=${prefix}" "-DREQUIRED_VERSION=${VERSION}" -DCMAKE_BUILD_TYPE=Release)
run("Building the user project" "${CMAKE_COMMAND}" --build "${consumer}" --config Release)
file(GLOB_RECURSE programs "${consumer}/consumer" "${consumer}/consumer.exe")
if(NOT programs)
  message(FATAL_ERROR "The user program was not built")
endif()
list(GET programs 0 program)
run("Running the user program" "${program}")
if(NOT run_output MATCHES "OK spaec ${VERSION}")
  message(FATAL_ERROR "Unexpected output of the user program: ${run_output}")
endif()

# The next minor version must not be accepted
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" _ "${VERSION}")
math(EXPR next_minor "${CMAKE_MATCH_2} + 1")
set(next "${CMAKE_MATCH_1}.${next_minor}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${CONSUMER_DIR}" -B "${WORK_DIR}/consumer_next" -G "${GENERATOR}"
                "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}" "-DCMAKE_PREFIX_PATH=${prefix}" "-DREQUIRED_VERSION=${next}"
                RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
if(rc EQUAL 0)
  message(FATAL_ERROR "find_package(spaec ${next}) accepted version ${VERSION}")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
message(STATUS "OK: spaec ${VERSION} installed, found and used; version ${next} rejected")
