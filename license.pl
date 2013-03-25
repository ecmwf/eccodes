#!/usr/bin/perl
use strict;

my $APACHE = <<"EOF";
Copyright 2005-2007 ECMWF

Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain a
copy of the License at http://www.apache.org/licenses/LICENSE-2.0 Unless
required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the License
for the specific language governing permissions and limitations under
the License.
EOF

my $LGPL = <<"EOF";
Copyright 2005-2007 ECMWF

Licensed under the GNU Lesser General Public License which
incorporates the terms and conditions of version 3 of the GNU
General Public License.
See LICENSE and gpl-3.0.txt for details.
EOF

my $GPL = <<"EOF";
Copyright 2005-2007 ECMWF

Licensed under the GNU General Public License version 3.
See LICENSE for details.
EOF


my $LICENSE = $LGPL;

my %COMMENTS = (

		java       => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		xml        => { start => "<!--\n", end => "-->\n\n" , after => "\<\?xml[^<]*\>" },
#		xsd        => { start => "<!--\n", end => "-->\n\n" },
#		jsp        => { start => "<!--\n", end => "-->\n\n" },
		sh         => { comment => "# ", end => "\n", after => "^#!/.*\n" },
		pl         => { comment => "# ", end => "\n", after => "^#!/.*\n" },
		js         => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		c          => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		h          => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		l          => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		y          => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		css        => { start => "/**\n"  , end => "*/\n\n"  , comment => "* " },
		sql        => { comment => "-- ", end => "\n" },
		properties => { comment => "# ", end => "\n" },
		def        => { comment => "# ", end => "\n" },
		F          => { comment => "C ", end => "C\n\n " },
		f          => { comment => "C ", end => "C\n\n" },

		);


foreach my $file ( @ARGV )
{
    my $doit=1;

	$file =~ /\.(\w+)/;
	my $ext = $1;

	my $c = $COMMENTS{$ext};

	unless($c)
	{
		print "$file: unsupported extension. File ignored\n";
		next;
	}

	open(IN,"<$file") or die "$file: $!";
	my @text = <IN>;
	close(IN);

	if(join("",@text) =~ /Licensed under the/gs) 
	{
		print "$file: License already stated. File ignored\n";
		next;
	}

	open(OUT,">$file.tmp") or die "$file.tmp: $!";

	if($c->{after})
	{
		my @x;
		my $re = $c->{after};
		loop: while(@text)
		{
			if($text[0] =~ m/$re/)
			{
				print OUT @x, shift @text;
				@x = ();
				last loop;
			}
			push @x,shift @text;
		}
		@text = (@x,@text);
	}

	print OUT $c->{start};
	foreach my $line ( split("\n",$LICENSE) )
	{
		print OUT $c->{comment}, $line,"\n";
	}
	print OUT $c->{end};

	print OUT @text;
	close(OUT) or die "$file: $!";

    if($doit) {
        use File::Copy qw(cp);
        use File::Compare qw(compare_text compare);

        if(compare_text("$file.tmp",$file)) {
            print "UPDATING file $file\n";
            system("p4 edit $file") unless(-w $file);
            cp($file,"$file.old") or die "cp($file,$file.old): $!";
            cp("$file.tmp",$file) or die "cp($file.tmp,$file): $!";
        }
    } else {
        print "IGNORING file $file\n";
    }

    unlink("$file.tmp");


}

