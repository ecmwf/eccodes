#!/usr/bin/perl
use strict;

# ----------------------------------------------------------------------
# Convert GRIB edition 2 multi-field (several fields per message) files
# to single-field (one field per message)
# ----------------------------------------------------------------------

my $debug = 0;
my $total = 0;
my $previous_bitmap;

if ($ARGV[0] eq "-d")
{
    $debug = 1;
    shift @ARGV;
}

my $out = pop @ARGV;

unless(@ARGV >= 1 && defined $out)
{
    print STDERR "Usage: $0 [-d] in1 [in2 ...]  out\n";
    exit(1);
}

open(OUT,">$out") || die "$out: $!";

foreach my $in ( @ARGV )
{
    my $gribs = 0;
    my $cnt   = 0;
    open(IN,"<$in")   || die "$in: $!";

    while(1)
    {
        my @sections = ();
        # Find header

        print "Looking for GRIB\n" if ($debug);
        my $grib;
        while((read(IN,$grib,4) == 4) && ($grib ne "GRIB"))
        {
            seek(IN,-3,1);
        }

        unless($grib eq "GRIB")
        {
            print "No more GRIBs\n" if ($debug);
            last;
        }

        my $here = tell(IN);
        my $start = $here - 4;
        $gribs++;

        print "$grib found at offset $start\n" if ($debug);

        my ($reserved,$discipline,$edition);

        die "$in: $!" unless(read(IN,$reserved,2) == 2);
        die "$in: $!" unless(read(IN,$discipline,1) == 1);
        die "$in: $!" unless(read(IN,$edition,1) == 1);

        my $grib_edition = ord($edition);
        die "Un-supported edition $grib_edition" unless($grib_edition == 2);

        my ($len1,$len2);
        die "$in: $!" unless(read(IN,$len1,4) == 4);
        die "$in: $!" unless(read(IN,$len2,4) == 4);

        my $msglen1 = unpack("N",$len1); die "GRIB too large" if ($msglen1);
        my $msglen2 = unpack("N",$len2);
        my $msglen = $msglen2;
        print "Message length $msglen\n" if ($debug);

        while(1)
        {
            $here = tell(IN);
            my ($seclen,$section);

            die "$in: $!" unless(read(IN,$seclen,4) == 4);

            if ($seclen eq "7777")
            {
                print "7777 found at offset $here\n" if ($debug);
                die "Invalid message lenth $msglen"
                    unless($here+4-$start == $msglen);
                last;
            }

            $seclen = unpack("N",$seclen);
            die "$in: $!" unless(read(IN,$section,1) == 1);
            $section = ord($section);
            die "Invalid section number $section" unless ($section >= 1 && $section <=7);
            print "Section $section found at offset $here length $seclen\n" if ($debug);
            seek(IN,-5,1) or die "$in: $!";

            my $data;
            die "$in: $!" unless(read(IN,$data,$seclen) == $seclen);

            if ($section == 6)
            {
                # Special case for inherited bitmaps
                if (unpack('C',substr($data,5,1)) == 254)
                {
                    die "No previous bitmap" unless(defined $previous_bitmap);
                    $data = $previous_bitmap;
                }
                else
                {
                    $previous_bitmap = $data;
                }
            }

            splice(@sections,$section+1);
            $sections[$section] = $data;

            if ($section == 7)
            {
                print OUT "GRIB";
                print OUT $reserved;
                print OUT $discipline;
                print OUT $edition;
                print OUT $len1;

                $len2 = 16 + 4;
                for(my $i = 1; $i <= 7 ; $i++)
                {
                    $len2 += length($sections[$i])
                        if (defined $sections[$i]);
                }

                $cnt++;
                print "GRIB $cnt is $len2\n" if ($debug);
                $len2 = pack("N",$len2);
                print OUT $len2;

                for(my $i = 1; $i <= 7 ; $i++)
                {
                    next unless(defined $sections[$i]);
                    print OUT $sections[$i];
                }
                print OUT "7777";
            }
        }
    }

    print "$in: $cnt field(s) found in $gribs GRIB(s)\n";
    $total += $cnt;
}

close(OUT) or die "$out: $!";

print "$out: $total field(s) written\n";
