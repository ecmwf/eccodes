#!/usr/bin/env perl

# This script is used to generate the file "python/gribapi/errors.py"
# It reads "src/grib_errors.c" which it expects to be up-to-date with
# all the error codes.

$|=1;

my $input = '../src/grib_errors.c';
my $print_class = 1;
my %errmap = ();

# Print header
# -------------------------------------
my $header = <<'END_HEADER';
"""
Exception class hierarchy
"""

import gribapi_swig as _internal


class GribInternalError(Exception):
    """
    @brief Wrap errors coming from the C API in a Python exception object.

    Base class for all exceptions
    """

    def __init__(self, value):
        # Call the base class constructor with the parameters it needs
        Exception.__init__(self, value)
        if type(value) is int:
            err, self.msg = _internal.grib_c_get_error_string(value, 1024)
            assert err == 0
        else:
            self.msg = value

    def __str__(self):
        return self.msg


END_HEADER
print $header;

# Print the exception classes
# -------------------------------------
while (<>) {
    # Example of input line
    #  "Passed array is too small",   /* -6 GRIB_ARRAY_TOO_SMALL */
    if (/^"(.*)",\s+\/\* (.*) (.*) \*\//) {
        $desc = $1;
        $code = $2;
        $name = $3;
        next if ($code == 0);

        $name =~ s/GRIB_//;
        $name_lc = $name;
        $name_lc =~ s/_/ /g;
        $name_lc = lc $name_lc;
        $name_lc =~ s/(\w+)/\u$1/g;
        $name_lc =~ s/ //g;

        $name = $name_lc;
        if ($name !~ /Error$/) {
            $name = $name . "Error";
        }
        
        $name = 'FunctionNotImplementedError' if ($name eq 'NotImplementedError');
        $name = 'MessageEndNotFoundError'     if ($name eq '7777NotFoundError');
        $name = 'IOProblemError'              if ($name eq 'IoProblemError');
        $name = 'MessageInvalidError'         if ($name eq 'InvalidMessageError');
        $name = 'GeocalculusError'            if ($name eq 'GeocalculusProblemError');
        $name = 'InvalidOrderByError'         if ($name eq 'InvalidOrderbyError');
        $name = 'InvalidBitsPerValueError'    if ($name eq 'InvalidBpvError');
        $name = 'KeyValueNotFoundError'       if ($name eq 'NotFoundError');
        $name = 'MemoryAllocationError'       if ($name eq 'OutOfMemoryError');

        # Print the class declaration
        print "class ${name}(GribInternalError):\n";
        print "    \"\"\"${desc}.\"\"\"\n";

        $errmap{$code} = $name; # store for later
    }
}

# Print the map
# -------------------------------------
my $size = keys %errmap;
print "\nERROR_MAP = {\n";
my $i = 0;
for $key (sort { $a <=> $b } keys %errmap) {
    $i++;
    my $ktext = sprintf("%-3d", $key);
    print "    $ktext : $errmap{$key}";
    print ",\n" if ($i < $size);
}
print "\n}\n\n";


# Print footer
# -------------------------------------
my $footer = <<'END_FOOTER';

def raise_grib_error(errid):
    """
    Raise the GribInternalError corresponding to ``errid``.
    """
    raise ERROR_MAP[errid](errid)
END_FOOTER
print $footer;
