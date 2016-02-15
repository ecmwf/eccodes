#!/usr/bin/perl
# Copyright 2005-2016 ECMWF.
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

write_public($public);
#write_internal($internal);
write_C_errors($public,$internal);
write_F90_errors($public);
write_python_errors($public);

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

    open(F,">grib_api_constants.h.new") or die "grib_api_constants.h.new: $!";

    foreach (sort {$a<=>$b} keys %{$errdict}){
        printf F "  integer, parameter,public :: %-50s = %d\n",
            $errdict->{$_}{name},$_;
    }
    printf F "  integer, parameter,public :: %-50s = %d\n","GRIB_NULL",-1;

    close(F);
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
    open(H,">grib_api.h.new") or die "grib_api.h.new: $!";
    open(IN,"<grib_api.h.in") or die "grib_api.h.in: $!";

    foreach (<GAH>) {
        last if /This part is automatically generated/;
        print H;
    }

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

    close(GAH);
    close(H);
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
