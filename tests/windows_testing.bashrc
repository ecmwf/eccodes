# bashrc to be used for running tests on windows.
# In order for the functions to be usable by the tests, we need to use the -i option.
# Typical usage:
#   bash --rcfile windows_testing.bashrc -ci "some/test/script.sh"

# During testing we diff Windows files with Unix files => need to ignore the carriage
# return.
function diff {
    command diff --strip-trailing-cr "$@"
}
export -f diff
