DIR=.
MARS=../../../mars/client/main/src
MARS=.
PGI=/usr/local/apps/pgi/current
FORTRAN=-L$(PGI)/linux86/lib  -lpgf90 -lpgf90_rpm1 -lpgf902 -lpgf90rtl -lpgftnrtl -lpghpf -lpgc -lpgf90 -lstd -Wl,-rpath $(PGI)/linux86/lib


CFLAGS=-I../src -I$(MARS) -g -Wall -Werror -m32
#JASPER=/usr/lib/libjasper.a
# JASPER=/tmp/mab/degrib/src/jpeg2000/src/libjasper/.libs/libjasper.a
JASPER=-ljasper -lpng
EMOSLIB=-lemos.new.R64.D64.I32
EMOSLIB=/home/ma/maf/public/libemosR64.a
#FORTRAN=-lxlf90 -lxlf -lc -lm
EMOSLIB=/home/ma/emos/lib/$(ARCH)/000300/libemos.R64.D64.I32.a

PROGS=values packing compatibility chemical sample1 # memory request
# filter read_any iterator
LIBS=../src/libgrib_api.a $(EMOSLIB)  $(FORTRAN) -lm $(JASPER)

# Netcdf
NCINCDIR=/home/ma/mac/netcdf/netcdf-3.6.1/include
NCLIBDIR=/home/ma/mac/netcdf/netcdf-3.6.1/lib

start : all packing

test tests: all
	./test.ksh

fail: all
	./test.ksh -k

all::
	cd ../src; make

all:: $(PROGS)

OBJ=compare.c

iterator: $(OBJ) iterator.c ../src/libgrib_api.a
	$(CC) -o iterator $(OBJ) iterator.c $(CFLAGS) $(LIBS)

sample: $(OBJ) sample.c ../src/libgrib_api.a
	$(CC) -o sample $(OBJ) sample.c $(CFLAGS) $(LIBS)

sample1: $(OBJ) sample1.c ../src/libgrib_api.a
	$(CC) -o sample1 $(OBJ) sample1.c $(CFLAGS) $(LIBS)

chemical: $(OBJ) chemical.c ../src/libgrib_api.a
	$(CC) -o chemical $(OBJ) chemical.c -I$(NCINCDIR) $(CFLAGS) $(LIBS) -L$(NCLIBDIR) -lnetcdf

read_any: $(OBJ) read_any.c ../src/libgrib_api.a
	$(CC) -o read_any $(OBJ) read_any.c $(CFLAGS) $(LIBS)

values: $(OBJ) values.c ../src/libgrib_api.a
	$(CC) -o values $(OBJ) values.c $(CFLAGS) $(LIBS)

packing: $(OBJ) packing.c ../src/libgrib_api.a
	$(CC) -o packing $(OBJ) packing.c $(CFLAGS) $(LIBS)

memory: $(OBJ) memory.c ../src/libgrib_api.a
	$(CC) -o memory $(OBJ) memory.c $(CFLAGS) $(LIBS)

request: $(OBJ) request.c ../src/libgrib_api.a
	$(CC) -o request $(OBJ) request.c $(CFLAGS) $(LIBS)

dump: $(OBJ) dump.c ../src/libgrib_api.a
	$(CC) -o dump $(OBJ) dump.c $(CFLAGS) $(LIBS)

compatibility: $(OBJ) compatibility.c ../src/libgrib_api.a
	$(CC) -o compatibility $(OBJ) compatibility.c $(CFLAGS) $(LIBS)

compatibility_ex: $(OBJ) compatibility_ex.c ../src/libgrib_api.a
	$(CC) -o compatibility_ex $(OBJ) compatibility_ex.c $(CFLAGS) $(LIBS)

grib1to2: $(OBJ) grib1to2.c ../src/libgrib_api.a
	$(CC) -o grib1to2 $(OBJ) grib1to2.c $(CFLAGS) $(LIBS)

.PRECIOUS: fail

clean:
	rm -f *.o core $(PROGS)
