
# we use pre-generated files from SWIG - to recreate them use: 

swig -python -module gribapi_swig -o swig_wrap_numpy.c gribapi_swig.i
cp gribapi_swig.py swig_wrap_numpy.py
