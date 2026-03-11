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

echo "All new unit tests passed."
