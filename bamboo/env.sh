#!/bin/bash

# Disable parallel builds in bamboo due to occasional failure
# GCC Parallel error: file not recognized: File truncated
#  https://seiscode.iris.washington.edu/issues/587

make_parallel="no"
