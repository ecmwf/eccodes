package GRIB::API;

use 5.006001;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use GRIB::API ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('GRIB::API', $VERSION);

# Preloaded methods go here.

sub new {
	my ($class,$arg) = @_;
	return Read($arg)      if(ref($arg) =~ /GLOB/);
	return create($arg)    if(substr($arg,0,4) =~ /(GRIB|BUDG|TIDE)/);
	return template($arg);
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

GRIB::API - Perl extension for blah blah blah

=head1 SYNOPSIS

  use GRIB::API;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for GRIB::API, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Baudouin Raoult

=head1 COPYRIGHT AND LICENSE
Copyright 2005-2016 ECMWF.

This software is licensed under the terms of the Apache Licence Version 2.0
which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

=cut
