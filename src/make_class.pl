#!/usr/bin/perl
use strict;
use Data::Dumper;

my $name;
my $verbose = 0;

foreach $name ( @ARGV )
{
    next unless($name =~ /\.c$/);
    print "$name\n" if ($verbose);

    open(IN,"<$name") or die "$name: $!";
    open(OUT,">$name.tmp") or die "$name.tmp: $!";
    my $class;
    my $tmp;
    my $out = 1;
    my $doit = 0;

    while(<IN>)
    {
        if(/^\s*START_CLASS_DEF/) 
        {
            $tmp = { NAME => $name };
            $tmp->{NAME} =~ s/\..*//;
            $tmp->{NAME} =~ s/.*_class_//;
        }

        if(/^\s*END_CLASS_DEF/)
        {
            $class = $tmp;
            $tmp   = undef;
        }

        if(/^\s*([A-Z]+)\s*=\s*(.*)/ && $tmp)
        {
            if(exists $tmp->{$1})
            {
                $tmp->{$1} = join(";",$tmp->{$1},$2)
            }
            else
            {
                $tmp->{$1} = $2;
            }
        }

        if(/^\/\*\s*START_CLASS_IMP/)
        {
            print OUT $_;
            $out = 0;
            output($name,$class);
        }

        if(/^\/\*\s*END_CLASS_IMP/)
        {
            $out = 1;
            $doit=1;
        }

        print  OUT $_ if($out);

    }

    close(OUT) or die "$name.tmp: $!";

    if($doit)
    {
        use File::Copy qw(cp);
        use File::Compare qw(compare_text compare);

        if(compare_text("$name.tmp",$name))
        {
            print "UPDATING file $name\n";
            system("chmod +w $name") unless(-w $name);
            cp($name,"$name.old") or die "cp($name,$name.old): $!";
            cp("$name.tmp",$name) or die "cp($name.tmp,$name): $!";
        }
    }
    else
    {
        print "IGNORING file $name\n" if ($verbose);
    }

    unlink("$name.tmp");

}

sub cleanup {
    my ($x) = @_;
    $x =~ s/^\s*//; 
    $x =~ s/\s*$//;
    return $x;
}

sub super_members {
    my ($super,$list) = @_;
    $super = cleanup($super);
    my $s = parse_super($super);

    my $class = cleanup($s->{NAME});

    my @members    = map { cleanup($_);} split(";",$s->{MEMBERS});
    unshift @members, "/* Members defined in $class */";

    unshift @{$list}, @members;

    super_members($s->{SUPER},$list) if($s->{SUPER});
}

sub output {
    my ($file,$args) = @_;
    local $_;

    my %delegates;

    my $class = cleanup($args->{CLASS});

    my $name = cleanup($args->{NAME});
    my @members    = map { cleanup($_);} split(";",$args->{MEMBERS});
    unshift @members, "/* Members defined in $name */";

    my @implements = map { cleanup($_);} split(";",$args->{IMPLEMENTS});

 
     push @implements, "init_class";

    my %implements;
    @implements{@implements} = (1)x@implements;

    my @inherits = map { cleanup($_);} split(";",$args->{INHERITS});
    my %inherits;
    @inherits{@inherits} = (1)x@inherits;

    super_members($args->{SUPER},\@members) if($args->{SUPER});

    print OUT <<"EOF";

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "$class.class" and rerun $0

*/

EOF

    my @procs;

    open(CLASS,"<$class.class") or die "$class.class: $!";

    while(<CLASS>)
    {
        if(/^(static\s+\w+\s*\*?\s*(\w+)\s*\(.*);/)
        {
            if(exists $args->{DELEGATE})
            {
                $delegates{$2}  = $1  unless($implements{$2});
                $implements{$2} = 1;
            }
            next unless($implements{$2});
        }

        if(/&SUPER/)
        {
            s/&SUPER/0/ unless($args->{SUPER});
            s/&SUPER/&grib_SUPER/ unless($args->{SUPER} =~ /^grib_/);
        }

        if(/extern.*SUPER/)
        {
            next unless($args->{SUPER});
            s/extern(.*)SUPER/extern${1}grib_SUPER/ unless($args->{SUPER} =~ /^grib_/);
        }

        if(/&\s*([a-z_]+)\s*,/)
        {
            push @procs, $1 unless($1 eq "SUPER");
            s/&\s*\w+/0/         unless($implements{$1});
            s/&\s*\w+/&SUPER.$1/ if($inherits{$1});
        }

        if(/MEMBERS/)
        {
            foreach my $m ( @members )
            {
                if($m =~ /\/\*/)
                {
                    print OUT "$m\n";
                }
                else
                {
                    print OUT "\t$m;\n";
                }
            }
            next;
        }

        s/([A-Z]+)/exists $args->{$1}?cleanup($args->{$1}):$1/eg;

        if(/^ADD_TO_FILE\s+([\w\.]*)\s+(.*)/)
        {
            chomp;
            my %text;
            my $file = $1;
            my $more = $2;

            $text{$more}++;

            if(-f $file)
            {
                local $_;
                system("chmmod +w $file") unless(-w $file);
                open(F,"<$file") or die "$file: $!";

                while(<F>)
                {
                    chomp;
                    next unless($_);
                    $text{$_}++ unless(/^\/\*/);
                }

                close(F);
            }

            open(F,">$file") or die "$file: $!";
            print F "/* This file is automatically generated by $0, do not edit */\n";
            foreach my $t ( sort keys %text )
            {
                print F $t,"\n";
            }
            close(F) or die "$file: $!";

            next;

        }

        if(/^INIT/)
        {
            next unless($args->{SUPER});

            foreach my $c ( grep { !$implements{$_} } grep { ! /\binit\b|\bdestroy\b|\bpost_init\b/ } @procs )
            {
                print OUT "\tc->$c\t=\t(*(c->super))->$c;\n";
            }
            next;
        }

        print OUT $_;
    }

  # disabled for the moment
  # the problem with this is that for pointers we need proper clone, not an assignement
  if (0) {
    foreach my $proc ( grep { /clone/ } @procs ) { 
        my $done=0;
        print OUT "static grib_$class* clone(grib_$class* s) {\n";
        print OUT "\tgrib_${class}_$name* c=grib_context_alloc_clear(a->parent->h,sizeof(grib_${class}_$name));\n";
        foreach my $m ( @members ) {
            if ( $m =~ /\/\*/ ) {next;}
            print OUT "\n\tgrib_${class}_$name* self=(grib_${class}_$name*)s;\n\n" unless ($done);
            $done=1;
            my $is_pointer=0;
            my @ma=split(/ /,$m);
            my $mname=pop @ma;
            if ($mname=~ /\*/) { $is_pointer=1; $mname =~ s/\*//; }
            my $mtype=pop @ma;
            if ($mtype=~ /\*/) { $is_pointer=1; $mtype =~ s/\*//; }

            if ($is_pointer) { 
                if ($mtype =~ "\bchar\b") {
                        print OUT "\tif (self->$mname) \n\t\tc->$mname=grib_context_strdup(a->parent->h,self->$mname);\n\n";
                }
                if ($mtype =~ "\b(double|long|int|float)\b") {
                        print OUT "\tif (self->".$mname."_size) {\n";
                        print OUT "\tint i=0;\n";
                        print OUT "\tc->$mname=grib_context_alloc_clear(a->parent->h,self->".$mname."_size*sizeof($mtype));";
                        print OUT "\tfor (i=0;i<self->${mname}_size;i++) c->".$mname."[i]=self->".$mname."[i];";
                        print OUT "}\n";
                }
            } else { print OUT "\tc->$mname=self->$mname;\n\n"; }
        }
        print OUT "\treturn (grib_$class*)c;\n}\n\n"
    }
  }

    delete $delegates{init};
    delete $delegates{post_init};
    delete $delegates{destroy};

    if(%delegates)
    {
        print OUT  "\n/* Start delegate methods */ \n";

        print OUT "\nstatic grib_accessor_class* $args->{DELEGATE}(accessor*);\n\n";

        foreach my $d ( sort keys %delegates )
        {
            $delegates{$d} =~ /static\s+(.*)\s+(\w+)\s*\((.*)\)/;
            my $return = $1 eq "void" ? "" : "return ";
            my $ret = $1;

            my @args = map { s/^ +//; s/ +$//; $_; } split(",",$3);

            my $i = 0;
            my @types;
            local $" = " ";

            foreach my $a ( @args )
            {
                my @x = split(" ",$a);

                $a = $x[-1];
                if(@x == 1 || $a =~ /\*$/)
                {
                    $a = "arg" . $i++ ;
                    push @x,$a;
                }
                push @types, join(" ",@x);
                $a =~ s/^\*//;
            }

            local $" = ",";

            print OUT <<"EOF";

static $ret $d(@types)
{
    ${return}target($args[0])->$d(@args);
}
EOF
        }

        print OUT  "\n/* End delegate methods */ \n";
    }

    close(CLASS);
}


sub parse_super {

    my ($name) = @_;

    local $_;

    # Make sure it's up-to-date
    system($0,"$name.c");

    open(S,"<$name.c") or die "$name.c: $!";

#    print "SUPER: $name\n";

    my $class;
    my $tmp;

    while(<S>)
    {
        if(/^\s*START_CLASS_DEF/) 
        {
            $tmp = { NAME => $name };
            $tmp->{NAME} =~ s/\..*//;
            $tmp->{NAME} =~ s/.*_class_//;
        }

        if(/^\s*END_CLASS_DEF/)
        {
            $class = $tmp;
            last;
        }

        if(/^\s*([A-Z]+)\s*=\s*(.*)/ && $tmp)
        {
            if(exists $tmp->{$1})
            {
                $tmp->{$1} = join(";",$tmp->{$1},$2)
            }
            else
            {
                $tmp->{$1} = $2;
            }
        }

    }

    close(S);

    return $tmp;

}
