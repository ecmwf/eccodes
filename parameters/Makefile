all : ../definitions/grib2/tigge_name.def ../tigge/tigge_check.h db

# ./tigge_def.pl produces 3 files. The rule is only for tigge_name.def
# ../definitions/grib2/tigge_parameter.def
# ../definitions/grib2/tigge_short_name.def

../definitions/grib2/tigge_name.def: master.info tigge_def.pl
	-p4 edit ../definitions/grib2/tigge_name.def
	-p4 edit ../definitions/grib2/tigge_parameter.def
	-p4 edit ../definitions/grib2/tigge_short_name.def
	./tigge_def.pl master.info
	cp tigge_name.def ../definitions/grib2/tigge_name.def
	cp tigge_parameter.def ../definitions/grib2/tigge_parameter.def
	cp tigge_short_name.def ../definitions/grib2/tigge_short_name.def

../tigge/tigge_check.h: master.info tigge_check.info tigge_check.pl
	-p4 edit ../tigge/tigge_check.h
	./tigge_check.pl master.info tigge_check.info > ../tigge/tigge_check.h

db : master.info feed_metdb.pl
	./feed_metdb.pl
	./feed_tigge.pl prod
	touch db
