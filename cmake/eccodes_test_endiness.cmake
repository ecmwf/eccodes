# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

test_big_endian( _BIG_ENDIAN )

if( _BIG_ENDIAN )
        set( ECCODES_BIG_ENDIAN    1 )
        set( ECCODES_LITTLE_ENDIAN 0 )
else()
        set( ECCODES_BIG_ENDIAN    0 )
        set( ECCODES_LITTLE_ENDIAN 1 )
endif()

set( ECCODES_BIG_ENDIAN    ${ECCODES_BIG_ENDIAN}    )
set( ECCODES_LITTLE_ENDIAN ${ECCODES_LITTLE_ENDIAN} )

if( NOT DEFINED IEEE_BE )
    check_c_source_runs(
       "int compare(unsigned char* a,unsigned char* b) {
         while(*a != 0) if (*(b++)!=*(a++)) return 1;
         return 0;
       }
       int main(int argc,char** argv) {
         unsigned char dc[]={0x30,0x61,0xDE,0x80,0x93,0x67,0xCC,0xD9,0};
         double da=1.23456789e-75;
         unsigned char* ca;

         unsigned char fc[]={0x05,0x83,0x48,0x22,0};
         float fa=1.23456789e-35;

         if (sizeof(double)!=8) return 1;

         ca=(unsigned char*)&da;
         if (compare(dc,ca)) return 1;

         if (sizeof(float)!=4) return 1;

         ca=(unsigned char*)&fa;
         if (compare(fc,ca)) return 1;

         return 0;
       }" IEEE_BE )

    if( "${IEEE_BE}" STREQUAL "" )
      set( IEEE_BE 0 CACHE INTERNAL "Test IEEE_BE")
    endif()

endif()

if( ECCODES_BIG_ENDIAN AND NOT IEEE_BE )
    ecbuild_critical("Failed to sanity check on endiness: OS should be Big-Endian but compiled code runs differently -- to ignore this pass -DIEEE_BE=0 to CMake/ecBuild")
endif()

if( NOT DEFINED IEEE_LE )
    check_c_source_runs(
       "int compare(unsigned char* a,unsigned char* b) {
         while(*a != 0) if (*(b++)!=*(a++)) return 1;
         return 0;
       }
       int main(int argc,char** argv) {
         unsigned char dc[]={0xD9,0xCC,0x67,0x93,0x80,0xDE,0x61,0x30,0};
         double da=1.23456789e-75;
         unsigned char* ca;

         unsigned char fc[]={0x22,0x48,0x83,0x05,0};
         float fa=1.23456789e-35;

         if (sizeof(double)!=8) return 1;

         ca=(unsigned char*)&da;
         if (compare(dc,ca)) return 1;

         if (sizeof(float)!=4) return 1;

         ca=(unsigned char*)&fa;
         if (compare(fc,ca)) return 1;

         return 0;
       }" IEEE_LE )

    if( "${IEEE_LE}" STREQUAL "" )
      set( IEEE_LE 0 CACHE INTERNAL "Test IEEE_LE")
    endif()
endif()

if( ECCODES_LITTLE_ENDIAN AND NOT IEEE_LE )
    ecbuild_critical("Failed to sanity check on endiness: OS should be Little-Endian but compiled code runs differently -- to ignore this pass -DIEEE_LE=0 to CMake/ecBuild")
endif()
