add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
  COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/memfs.py
    ${PROJECT_SOURCE_DIR}/definitions
    ${PROJECT_SOURCE_DIR}/samples
    ${PROJECT_SOURCE_DIR}/ifs_samples
    ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
  DEPENDS ${PROJECT_SOURCE_DIR}/memfs.py )

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/memfs.c PROPERTIES OBJECT_DEPENDS
    "${PROJECT_SOURCE_DIR}/memfs.py"
    # "${PROJECT_SOURCE_DIR}/memfs.py" ${definition_files}"
    )

ecbuild_add_library(TARGET    eccodes_memfs
                    CONDITION HAVE_MEMFS
                    SOURCES   ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
                    GENERATED ${CMAKE_CURRENT_BINARY_DIR}/memfs.c )
