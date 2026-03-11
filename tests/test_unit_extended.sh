#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Wrapper script for C++ unit test executables

echo "Running test_bits..."
$EXEC ${test_dir}/test_bits
echo ""

echo "Running test_float_conversions..."
$EXEC ${test_dir}/test_float_conversions
echo ""

echo "Running test_value_api..."
$EXEC ${test_dir}/test_value_api
echo ""

echo "Running test_expression..."
$EXEC ${test_dir}/test_expression
echo ""

echo "Running test_data_structures..."
$EXEC ${test_dir}/test_data_structures
echo ""

echo "Running test_geo..."
$EXEC ${test_dir}/test_geo
echo ""

echo "Running test_io..."
$EXEC ${test_dir}/test_io
echo ""

echo "Running test_trie..."
$EXEC ${test_dir}/test_trie
echo ""

echo "Running test_keys_iterator..."
$EXEC ${test_dir}/test_keys_iterator
echo ""

echo "Running test_handle_lifecycle..."
$EXEC ${test_dir}/test_handle_lifecycle
echo ""

echo "Running test_util..."
$EXEC ${test_dir}/test_util
echo ""

echo "Running test_bufr_descriptors..."
$EXEC ${test_dir}/test_bufr_descriptors
echo ""

echo "Running test_bufr_api..."
$EXEC ${test_dir}/test_bufr_api
echo ""

echo "All new unit tests passed."
