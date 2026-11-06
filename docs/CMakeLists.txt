# SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Optional target; CI builds the docs with docs/Makefile instead.

include(ProcessorCount)
ProcessorCount(nproc)
find_package(Doxygen 1.9 REQUIRED)
find_program(SPHINX_EXECUTABLE NAMES sphinx-build REQUIRED)

file(GLOB_RECURSE SPHINX_INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)
set(SPHINX_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/html)
set(SPHINX_INDEX_FILE ${SPHINX_OUTPUT_DIR}/index.html)

add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    COMMAND ${SPHINX_EXECUTABLE} -j ${nproc} -W --keep-going ${CMAKE_CURRENT_SOURCE_DIR} ${SPHINX_OUTPUT_DIR}
    DEPENDS
        ${PROJECT_SOURCE_DIR}/src/eccodes/eccodes.h
        ${SPHINX_INPUT_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation"
)

add_custom_target(eccodes-doc ALL DEPENDS ${SPHINX_INDEX_FILE})
