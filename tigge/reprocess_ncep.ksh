#!/bin/ksh

set -e # Stop on error
set -u # Catche undefined variable
set +x # Trace

# During development

PATH=$PATH:/var/tmp/mab/p4/grib_api/main/tigge:/var/tmp/mab/p4/grib_api/main/tools
RULES=/var/tmp/mab/p4/grib_api/main/tigge/ncep.rules

# Cycle should be an argument

cycle=2007051300

# This should match the value of productionStatusOfProcessedData in $RULES (prod=4, test=5)
prod=prod 

# Where the tar files are
datadir=`pwd`/data

# Point to some work directory
workdir=`pwd`/work

# Where the z_c_tigge... goes
output=`pwd`/output


cd $workdir

# Extract cf and pf data from tar files
echo $(date +'%F %X') Extract from tar files

if [[ ! -f done.$cycle.step0 ]]
then
	rm -f  done.$cycle.step[1-9]
	rm -fr tmp.data.$cycle
	mkdir  tmp.data.$cycle 
	for n in a c 
	do
		cd tmp.data.$cycle
		tar xf $datadir/$cycle.pgrb2$n.tar
		cd $workdir
	done

	echo $(date +'%F %X') Find CF and PF data

	find tmp.data.$cycle -type f | egrep '/ge[pc][0-9]+\.t[0-9]+z\.pgrb2[a-z]+[0-9]+$' | xargs cat > tmp.all.$cycle
	rm -fr tmp.data.$cycle
	touch done.$cycle.step0
fi

# Split in TP, TCC and "other"
# TCC will be discarded
# TP split into individual files 

echo $(date +'%F %X') Sort fields

if [[ ! -f done.$cycle.step1 ]]
then
	rm -f done.$cycle.step[2-9]

	rm -f tmp.data.$cycle.*

	cat > tmp.$cycle.filter <<@

	if(discipline==0 && parameterCategory == 6 && parameterNumber == 1 && typeOfFirstFixedSurface == 200)
	{
		# Ignore TCC for now
		write "tmp.data.$cycle.discarded_tcc";
	}
	else {
			if(discipline==0 && parameterCategory == 1 && parameterNumber == 8 && typeOfFirstFixedSurface == 1)
			{
				if(marsEndStep - marsStartStep == 6)
				{
					write "tmp.data.$cycle.tp.[step].[number]";
				}
				else
				{
					write "tmp.data.$cycle.discarded_tp";
				}
			}
			else {
				write "tmp.data.$cycle.other";
		}
	}

@

	grib_filter tmp.$cycle.filter tmp.all.$cycle

	touch done.$cycle.step1

	rm -f tmp.$cycle.filter tmp.all.$cycle 
	rm -f tmp.data.$cycle.discarded_tcc tmp.data.$cycle.discarded_tp
fi

# Create accumulations from TP

echo $(date +'%F %X') Compute accumulations

if [[ ! -f done.$cycle.step2 ]]
then
	rm -f done.$cycle.step[3-9]

	# Get the greatest ensemble number and the greatest step

	set $(ls -1 tmp.data.$cycle.tp.* | awk -F. '
		BEGIN { s= 0; e = 0; }
		{ 
			if($5>s) { s=$5;}
			if($6>e) { e=$6;}
		}
		END { printf("%d %d\n",s,e); }
	')

	last_s=$1
	last_e=$2

	# Compute accumulations

	e=0
	while [[ $e -le $last_e ]]
	do
		rm -f tmp.data.$cycle.precip.$e
		s=6
		while [[ $s -le $last_s ]]
		do
		touch tmp.data.$cycle.tp.$s.$e
			cat tmp.data.$cycle.tp.$s.$e >> tmp.data.$cycle.precip.$e
			s=$((s+6))
		done
		set +e
		tigge_accumulations tmp.data.$cycle.precip.$e tmp.data.$cycle.precip.$e.accumulated
		set -e
		rm tmp.data.$cycle.precip.$e
		e=$((e+1))
	done

	cat tmp.data.$cycle.precip.*.accumulated > tmp.data.$cycle.precip_accumulated

	# Cleanup

	rm -f tmp.data.$cycle.precip.*.accumulated
	rm -f tmp.data.$cycle.tp.*

	touch done.$cycle.step2
fi

# Run grib_conver to convert to TIGGE complient fields
echo $(date +'%F %X') Patch headers

if [[ ! -f done.$cycle.step3 ]]
then
	rm -f done.$cycle.step[4-9]

	for n in tmp.data.$cycle.precip_accumulated tmp.data.$cycle.other
	do
		grib_convert $RULES $n $n.tigge
	done

	rm -f tmp.data.$cycle.precip_accumulated tmp.data.$cycle.other
	touch done.$cycle.step3
fi

# Run tigge_check to weed out the unwanted fields

echo $(date +'%F %X') Remove unwanted fields

if [[ ! -f done.$cycle.step4 ]]
then
	rm -f done.$cycle.step[5-9]

	# -z means return 'zero' to the shell even if some fields are not TIGGE complient
	tigge_check -z -g tmp.data.$cycle.good tmp.data.$cycle.precip_accumulated.tigge tmp.data.$cycle.other.tigge > /dev/null

	rm -f tmp.data.$cycle.precip_accumulated.tigge tmp.data.$cycle.other.tigge 

	touch done.$cycle.step4
fi

# Split into individual fields and create manifest

echo $(date +'%F %X') Create TIGGE files

if [[ ! -f done.$cycle.step5 ]]
then
	rm -f done.$cycle.step[6-9]

	rm -fr $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod
	mkdir  $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod
	cd     $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod

	# -p means 'print' names. Switch off if the manifest is done by another script
	# -u means 'unique', checks that there are not two fields with the same name. Switch off once script is debugged

	#tigge_split -p -u $workdir/tmp.data.$cycle.good > $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod.manifest
	tigge_split -p $workdir/tmp.data.$cycle.good > $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod.manifest

	cd $workdir
	rm -f tmp.data.$cycle.good 
	rm -f tmp.data.$cycle.* 
	touch done.$cycle.step5
fi

# Remove check points
rm -f done.$cycle.step*

n=$(wc -l $output/z_tigge_c_kwbc_${cycle}0000_glob_$prod.manifest | awk '{print $1;}')
echo $(date +'%F %X') Recovered $n fields
