#!/usr/bin/perl
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
use Data::Dumper;

use strict; use warnings;

my $internal = get_internal_errors();
my $public = get_public_errors();
#print "DUMP,\t", Data::Dumper->Dump([\$public], [" "]);

write_public($public);
#write_internal($internal);
write_C_errors($public,$internal);
write_F90_errors($public);
write_python_errors($public);
write_python_exceptions($public);

sub write_python_exceptions {
    my $errdict = shift;
    my %errmap = (); # code -> class name
    open(H,">errors.py.new") or die "errors.py.new: $!";

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
    print H $header;

    foreach (sort {$a<=>$b} keys %{$errdict}){
        my $code = $_;
        my $name = $errdict->{$_}{name};
        my $text = $errdict->{$_}{text};
        # Convert name e.g. GRIB_WRONG_ARRAY_SIZE -> WrongArraySizeError
        $name =~ s/GRIB_//;         # Remove GRIB_
        my $name_lc = $name;
        $name_lc =~ s/_/ /g;        # Remove underscores
        $name_lc = lc $name_lc;     # lowercase
        $name_lc =~ s/(\w+)/\u$1/g; # First letter uppercase
        $name_lc =~ s/ //g;         # Remove spaces
        $name = $name_lc;
        if ($name !~ /Error$/) {    # Append Error (unless already there)
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

        if ($code != 0) { # Ignore SUCCESS
            print H "class ${name}(GribInternalError):\n";  ## $name,$_;
            print H "    \"\"\"${text}.\"\"\"\n";

            $errmap{$code} = $name; # store for next loop
        }
    }

    print H "\nERROR_MAP = {\n";
    my $i = 0;
    my $size = keys %errmap;
    for my $code (sort { $a <=> $b } keys %errmap) {
        next if ($code == 0);
        $i++;
        my $ktext = sprintf("%-3d", $code);
        print H "    $ktext : $errmap{$code}";
        print H ",\n" if ($i < $size);
    }
    print H "\n}\n\n";

    # Footer
    my $footer = <<'END_FOOTER';

def raise_grib_error(errid):
    """
    Raise the GribInternalError corresponding to ``errid``.
    """
    raise ERROR_MAP[errid](errid)
END_FOOTER
    print H $footer;

    close(H);
}


sub write_python_errors {
    my $errdict = shift;

    open(H,">grib_errors.h.new") or die "grib_errors.h.new: $!";
    open(IN,"<grib_api.h.in") or die "grib_api.h.in: $!";

    foreach (<IN>) {
        if (/^!ERRORS/) {
            foreach my $code (reverse sort {$a<=>$b} keys %{$errdict}) {
                my $desc = $errdict->{$code};
                print H "/** $desc->{text} */\n";
                print H "#define $desc->{name}\t\t$code\n";
            }
        } else {
            print H;
        }
    }

    close(H);
    close(IN);
}

sub write_F90_errors {
    my $errdict = shift;

    open(F1,">grib_api_constants.h.new") or die "grib_api_constants.h.new: $!";
    open(F2,">eccodes_constants.h.new") or die "eccodes_constants.h.new: $!";

    foreach (sort {$a<=>$b} keys %{$errdict}){
        my $name = $errdict->{$_}{name};
        printf F1 "  integer, parameter,public :: %-50s = %d\n", $name,$_;
        (my $eccodes_name = $name) =~ s/GRIB_/CODES_/;
        printf F2 "  integer, parameter,public :: %-50s = %d\n", $eccodes_name,$_;
    }
    printf F1 "  integer, parameter,public :: %-50s = %d\n","GRIB_NULL",-1;
    printf F2 "  integer, parameter,public :: %-50s = %d\n","CODES_NULL",-1;
    close(F1);
    close(F2);
}

sub write_C_errors {
    my ($public,$internal) = @_;

    open(C,">grib_errors.c.new") or die "grib_errors.c.new: $!";
    open(IN,"<grib_errors.c.in") or die "grib_errors.c.in: $!";

    foreach (<IN>) {
        if (/^!ERRORS/) {
            foreach (reverse sort {$a<=>$b} keys %{$public}) {
                my $desc = $public->{$_};
                print C "\"$desc->{text}\",\t\t/* $_ $desc->{name} */\n";
            }

            foreach (sort {$a<=>$b} keys %{$internal}) {
                my $desc = $internal->{$_};
                print C "\"$desc->{text}\",\t\t/* $_ $desc->{name} */\n";
            }
        } else {
            print C;
        }
    }

    close(C);
    close(IN);
}

sub write_public {
    my $errdict = shift;

    open(GAH,"<grib_api.h") or die "grib_api.h: $!";
    open(ECH,"<eccodes.h") or die "eccodes.h: $!";

    open(H1,">grib_api.h.new") or die "grib_api.h.new: $!";
    open(H2,">eccodes.h.new") or die "eccodes.h.new: $!";

    open(IN,"<grib_api.h.in") or die "grib_api.h.in: $!";

    foreach (<GAH>) {
        last if /This part is automatically generated/;
        print H1;
    }
    foreach (<ECH>) {
        last if /This part is automatically generated/;
        print H2;
    }

    foreach (<IN>) {
        if (/^!ERRORS/) {
            foreach my $code (reverse sort {$a<=>$b} keys %{$errdict}) {
                my $desc = $errdict->{$code};
                my $name = $desc->{name};
                print H1 "/** $desc->{text} */\n";
                print H1 "#define ${name}\t\t$code\n";

                print H2 "/** $desc->{text} */\n";
                (my $ecc_name = $name) =~ s/GRIB_/CODES_/;
                print H2 "#define ${ecc_name}\t\t${name}\n";
            }
        } else {
            print H1;
            # For eccodes.h, replace grib_api with eccodes
            s/grib_errors_H/eccodes_errors_H/ if (/grib_errors_H/);
            s/grib_api functions/eccodes functions/ if (/returned by the grib_api functions/);
            print H2;
        }
    }

    close(GAH);
    close(ECH);
    close(H1);
    close(H2);
    close(IN);
}

sub write_internal {
    my $errdict = shift;

    open(GAH,"<grib_api_internal.h") or die "grib_api_internal.h: $!";
    open(H,">grib_api_internal.h.new") or die "grib_api_internal.h.new: $!";
    open(IN,"<grib_api_internal.h.in") or die "grib_api_internal.h.in: $!";

    foreach (<GAH>) {
        last if /This part is automatically generated/;
        print H;
    }

    foreach (<IN>) {
        if (/^!ERRORS/) {
            foreach my $code (sort {$a<=>$b} keys %{$errdict}) {
                my $desc = $errdict->{$code};
                print H "/** $desc->{text} */\n";
                print H "#define $desc->{name}\t\t$code\n";
            }
        } else {
            print H;
        }
    }

    close(GAH);
    close(H);
    close(IN);
}

sub get_public_errors {
    open(IN,"<grib_errors.txt") or die "grib_errors.txt: $!";
    my %temp = ();
    my $code = 0;
    while (<IN>) {
        next unless(/^GRIB_/);
        chomp;
        my ($name,$text) = split(" ",$_,2);
        $temp{$code}{name} = $name;
        $temp{$code}{text} = $text;
        $code--;
    }
    close(IN);
    return \%temp;
}

sub get_internal_errors {
    open(INI,"<grib_errors_internal.txt") or die "grib_errors_internal.txt: $!";
    my %temp = ();
    my $code = 1;
    while (<INI>) {
        next unless(/^GRIB_/);
        chomp;
        my ($name,$text) = split(" ",$_,2);
        $temp{$code}{name} = $name;
        $temp{$code}{text} = $text;
        $code++;
    }
    close(INI);
    return \%temp;
}
