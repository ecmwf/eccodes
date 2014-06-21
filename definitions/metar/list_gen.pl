use strict;

my $end=90;
my $i;

#print "//|Missing\n";
print "00|0.0\n";
for ($i=1; $i<=$end; $i++) {
				printf("%02d|%g\n",$i,$i/100);
}
print "92|10";
print "93|15";
print "94|20";
print "95|25";
print "96|30";
print "97|35";
print "98|40 or more";

