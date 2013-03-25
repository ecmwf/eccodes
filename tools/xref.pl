#!/usr/bin/perl
use Data::Dumper;
use File::Find ();
use File::Basename;
use DBI;

use strict;

my $host="wrep-db-misc-prod";
my $user="ecmwf";
my $pass="";

my $dbh  = DBI->connect("dbi:mysql:;host=$host",$user,$pass) or die $DBI::errstr;

sub execute_query {
	my @lines = split(/;/,$_[0]);

	foreach (@lines)
	{
		next unless($_ =~ /\w/);
		my $s = $dbh->prepare($_);
		$s->execute;
		if($_=~/\s*select\b/)
		{
			while(my @x = $s->fetchrow_array)
			{
				print "@x\n";
			}
		}
	}
}

my $DEFINITIONS = "../definitions";
my @definitions;
my %definitions;


sub createdb { 
	print "creating xref database...\n";
	my $qh=$dbh->prepare("create database xref");
	$qh->execute() or die $DBI::errstr;
	$dbh->disconnect();
	$dbh  = DBI->connect("dbi:mysql:database=xref;host=$host",$user,$pass) or die $DBI::errstr;
	my $query="create table xref (
								  keyname varchar(100),
								  type varchar(100),
								  size int not null,
								  position int not null,
								  msgtype varchar(50),
								  file varchar(250),
								  flag varchar(50),
								  ref varchar(100));
		  alter table xref add primary key (keyname,type,size,position,msgtype,file,flag,ref);
		  alter table xref add index (keyname);
		  alter table xref add index (type);
		  alter table xref add index (size);
		  alter table xref add index (position);
		  alter table xref add index (msgtype);
		  alter table xref add index (file);
		  alter table xref add index (flag);
		  alter table xref add index (ref);";
	execute_query($query);
}

my $q=$dbh->prepare("use xref");
$q->{HandleError}=\&createdb;
$q->execute();


File::Find::find({wanted => sub {push @definitions, $File::Find::name if(/\.def$/) } }, $DEFINITIONS);


foreach my $name (  @definitions )
{
	my $def = get_definition($name);
	open(IN,"<$name") or "die $name: $!";
	while(<IN>)
	{
		next if(/^\s*#/);
		if(/include\s+\"(.*)\"/)
		{
			my $dir = dirname($name);
			my $inc = get_definition("$dir/$1");
			push @{$def->{includes}},   $inc;		
			push @{$inc->{included_by}},$def;		
		}
	}
	close(IN);
}



# First report: inclusions

if(0)  { 

foreach my $p ( sort keys %definitions )
{
	my $def = $definitions{$p};

	print "$def->{name}\n";
	foreach my $x ( sort @{$def->{includes}} )
	{
		print "   includes   : $x->{name}\n";
	}

	foreach my $x ( sort @{$def->{included_by}} )
	{
		print "   included by: $x->{name}\n";
	}

}
}

print "\n\n";


# Get the definition object from a path
sub get_definition {
	my ($path) = @_;


	my $def = $definitions{$path} ;  
	unless(defined $def) {
		my $name = $path;
		$name =~ s/^$DEFINITIONS\/?//o;
		$def = bless( {name => $name, path => $path,
				includes => [], included_by => [] 
				}, "xref::definition") ;
		$definitions{$path} = $def;
	}
	return $def;
}

sub long     { @_; }
sub access   { @_; }
sub string   { @_; }
sub double   { @_; }
sub unop     { @_; }
sub binop    { @_; }
sub missing  { "missing"; }

@definitions = ();
foreach my $p ( sort keys %definitions )
{
	my $def = $definitions{$p};
	push @definitions, $def->{path} unless(@{$def->{included_by}});
}

local ($/);
open(IN,"./xref @definitions|") or die "xref: $!";
my $x = <IN>;
close(IN);

my  $xref;
eval "\$xref=[$x]";

my %xref;
foreach my $x ( @{$xref} )
{
	unless($x->isa("xref::object")) {
		my $p =  ref $x;
		eval "package $p; use base qw(xref::object);";
	}
	push @{$xref{$x->name}}, $x;
}

foreach my $name ( sort { lc $a cmp lc $b } keys %xref )
{
	my $visible = 0;
	my %edition;

	my %type;

	foreach my $x (  @{$xref{$name}} )
	{
		$visible ++ if($x->visible);
		$edition{$x->edition}++;
		$type{$x->type}++;
		
	}

	next unless($visible);

	my @edition = sort keys %edition;
	my @type    = sort keys %type;

	print "\n",$name," [@edition] [@type]\n\n";	
	foreach my $x ( sort { $a->{path} cmp $b->{path} } @{$xref{$name}} )
	{
		print "   ", $x->path, " " , $x->type , "[", $x->size, "] ";
		$x->show;
		print "\n";
		$x->insert;
	}
}


package xref::object;
sub name { my ($self) = @_; return $self->{name}; }
sub path { my ($self) = @_; return main::get_definition($self->{path})->{name} ; }
#sub visible { my ($self) = @_; return !exists $self->{flags}->{GRIB_ACCESSOR_FLAG_HIDDEN}; } 
sub visible { my ($self) = @_; return 1; } 

sub insert {
    my $insert = "insert into xref (keyname,type,size,position,msgtype,file,flag,ref) values ";
	my ($self) = @_;
	my @flags = map { s/GRIB_ACCESSOR_FLAG_//; $_; } sort keys %{$self->{flags}};
	if (!@flags) { @flags = ("none"); };
	my @params = @{$self->{params}} unless (!$self->{params}) ;
	if ($self->type eq "alias" ) {@params = ( $self->{target} )};
	if (!@params) { @params = ("none"); };
	my @values;
	foreach my $flag (@flags) {
	  foreach my $param (@params) {
	    my $value=" (\"".$self->{name}."\",\"".$self->type."\",".$self->size.",".$self->position.",\"".$self->edition."\",\"".$self->path."\",\"$flag\",\"$param\")\n";
		push (@values,$value);
	  }
	}
    $insert .= join(",",@values);
	#print "$insert\n";
	my $qh=$dbh->prepare($insert);
	$qh->{RaiseError}=0;
	$qh->{PrintError}=0;
	$qh->{HandleError}=sub {};
	$qh->execute();
}

sub show {
	my ($self) = @_;
	my @flags = map { s/GRIB_ACCESSOR_FLAG_//; $_; } sort keys %{$self->{flags}};
	my @params = @{$self->{params}};
	print join(",",@flags), " (", join(",",@params), ")";
}

sub size {
	my ($self) = @_;
	return $self->{size};
}

sub position {
	my ($self) = @_;
	if ($self->{position}) { 
	  return $self->{position};
	} else { return -1 ; }
}

sub edition {
	my ($self) = @_;
	my ($e)  = split("/",main::get_definition($self->{path})->{name});
	return $e;

}

sub type {
        my ($self) = @_;
		my $t = ref($self);
		$t =~ s/.*:://;
		return $t;
}


package xref::alias;
use base qw(xref::object);
sub show {
	my ($self) = @_;
	print "-> $self->{target}";
}

sub size {
	return -1;
}

package xref::unalias;
use base qw(xref::object);
sub show {
        my ($self) = @_;
}


package xref::label;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }

package xref::position;
use base qw(xref::object);
#sub visible { my ($self) = @_; return 0; }

package xref::constant;
use base qw(xref::object);

package xref::pad;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::padto;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::padtoeven;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::padtomultiple;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::section_padding;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::iterator;
use base qw(xref::object);
sub visible { my ($self) = @_; return 0; }
sub position { my ($self) = @_; return -1; }

package xref::bytes;
use base qw(xref::object);

package xref::section_length;
use base qw(xref::object);

package xref::scale;
use base qw(xref::object);
sub type { "real"; }
sub position { my ($self) = @_; return -1; }

package xref::ibmfloat;
use base qw(xref::object);
sub type { "real"; }

package xref::ieeefloat;
use base qw(xref::object);
sub type { "real"; }
