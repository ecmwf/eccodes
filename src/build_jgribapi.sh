gcc  -o libjgribapi.so -shared -Wl,-soname,jgribapi.so  -I$JAVA_HOME/include -I$JAVA_HOME/include/linux grib_java_interface.c  -static -lc -L. -lgrib_api
