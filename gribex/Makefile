LIB = /usr/local/lib/metaps/lib/000381/libemos.R64.D64.I32.a
INSTALL_DIR=/usr/local/lib/metaps/bin
VERSION=1.9.9
GRIB_API=-L/usr/local/lib/metaps/lib/grib_api/${VERSION}/lib -lgrib_api_f90 -lgrib_api -L/usr/local/apps/jasper/1.900.0/LP64/lib -ljasper -lm -I/usr/local/lib/metaps/lib/grib_api/${VERSION}/include -I/usr/local/apps/jasper/1.900.0/LP64/include

FFLAGS=-O
EXECS   = gribex_count grib_api_count
all: $(EXECS)

grib_api_count: grib_api_count.c
	xlc_r -o $@ grib_api_count.c $(GRIB_API)
	
gribex_count: gribex_count.o  gribex_count.F
	$(FC) $(FFLAGS) -o $@ gribex_count.o $(LIB)

install: grib_api_count gribex_count gribex_check.ksh gribex_check_no_fail.ksh
	cp $(INSTALL_DIR)/grib_api_count $(INSTALL_DIR)/grib_api_count.old
	cp grib_api_count $(INSTALL_DIR)/grib_api_count
	cp $(INSTALL_DIR)/gribex_count $(INSTALL_DIR)/gribex_count.old
	cp gribex_count $(INSTALL_DIR)/gribex_count
	cp $(INSTALL_DIR)/gribex_check.ksh $(INSTALL_DIR)/gribex_check.ksh.old
	cp gribex_check.ksh $(INSTALL_DIR)/gribex_check.ksh
	cp $(INSTALL_DIR)/gribex_check_no_fail.ksh $(INSTALL_DIR)/gribex_check_no_fail.ksh.old
	cp gribex_check_no_fail.ksh $(INSTALL_DIR)/gribex_check_no_fail.ksh
	
clean:
	@for name in $(EXECS) ; do\
	(rm -f $$name $$name.o ); \
	done
