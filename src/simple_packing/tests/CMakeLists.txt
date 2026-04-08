# Include directories for all simple_packing tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/simple)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/simple/codec)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/simple/optimizer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/simple/simple)

# Always compile simple_packing tests with -march=native -mtune=native -O3 (release mode)
add_compile_options(-march=native -mtune=native -O3)
add_compile_definitions(NDEBUG)

# Prefix for test names to avoid collisions when built as sub-project
set(SP_TEST_PREFIX "simple_packing_")

# --- Tests ---

add_executable(sp_check_encode
  check_encode.cc
  ../src/simple/LibEccodes.cc
  )
target_link_libraries(sp_check_encode simple_packing_lib)

foreach (i 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64)
    add_test(NAME ${SP_TEST_PREFIX}check_encode_${i} COMMAND sp_check_encode 10000 ${i})
endforeach ()

### --------------------------------

add_executable(sp_check_array_codec
  check_array_codec.cc
  ../src/simple/LibEccodes.cc
  )
target_link_libraries(sp_check_array_codec simple_packing_lib)

add_test(NAME ${SP_TEST_PREFIX}check_array_codec_16 COMMAND sp_check_array_codec double 10000 16)

### --------------------------------

add_executable(sp_bit_packing
  bit_packing.cc
  )
target_link_libraries(sp_bit_packing simple_packing_lib)

foreach (i 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64)
    add_test(NAME ${SP_TEST_PREFIX}bit_packing_${i} COMMAND sp_bit_packing ${i})
endforeach ()

### --------------------------------

add_executable(sp_dec_params
  dec_params.cc
  )
target_link_libraries(sp_dec_params simple_packing_lib)

add_test(NAME ${SP_TEST_PREFIX}dec_params COMMAND sp_dec_params)

### --------------------------------

add_executable(sp_u128
  u128.cc
  )
target_link_libraries(sp_u128 simple_packing_lib)

add_test(NAME ${SP_TEST_PREFIX}u128 COMMAND sp_u128)

### --------------------------------

add_executable(sp_u128_add_sub
  u128_add_sub.cc
  )

add_test(NAME ${SP_TEST_PREFIX}u128_add_sub COMMAND sp_u128_add_sub)

### --------------------------------

add_executable(sp_power
  power.cc
  )

add_test(NAME ${SP_TEST_PREFIX}power COMMAND sp_power)

### --------------------------------

add_executable(sp_bit_packing_element
  bit_packing_element.cc
  )

add_test(NAME ${SP_TEST_PREFIX}bit_packing_element COMMAND sp_bit_packing_element 10)

