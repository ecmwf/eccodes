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

PROGS= gaussian

LIBS=../src/libgrib_api.a $(EMOSLIB)  $(FORTRAN) -lm $(JASPER)

# Netcdf
NCINCDIR=/home/ma/mac/netcdf/netcdf-3.6.1/include
NCLIBDIR=/home/ma/mac/netcdf/netcdf-3.6.1/lib

start : all

all::
	cd ../src; make

all:: $(PROGS)


gaussian: $(OBJ) gaussian.c gaussian.h ../src/libgrib_api.a
	$(CC) -o gaussian $(OBJ) gaussian.c $(CFLAGS) $(LIBS)

.PRECIOUS: fail


clean:
	rm -f *.o core $(PROGS)
