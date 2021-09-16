dnl ieee big endian double and float compliancy
dnl Enrico Fucile 12-2008
AC_DEFUN([AC_IEEE_BE],
[
  AC_MSG_CHECKING([if double and float are ieee big endian])
  AC_LANG(C)
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
     int compare(unsigned char* a,unsigned char* b) {
       while(*a != 0) if (*(b++)!=*(a++)) return 1;
       return 0;
     }
     int main(int argc,char** argv) {
       unsigned char dc[[]]={0x30,0x61,0xDE,0x80,0x93,0x67,0xCC,0xD9,0};
       double da=1.23456789e-75;
       unsigned char* ca;
     
       unsigned char fc[[]]={0x05,0x83,0x48,0x22,0};
       float fa=1.23456789e-35;
     
       if (sizeof(double)!=8) return 1;
     
       ca=(unsigned char*)&da;
       if (compare(dc,ca)) return 1;

       if (sizeof(float)!=4) return 1;

       ca=(unsigned char*)&fa;
       if (compare(fc,ca)) return 1;
     
       return 0;
     }
    ])
  ],
  [AS_VAR_SET(IS_IEEE_BE, 1)],
  [AS_VAR_SET(IS_IEEE_BE, 0)],
  [])
  if test $IS_IEEE_BE = 0
  then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
]
)

dnl ieee little endian double and float compliancy
dnl Enrico Fucile 12-2008
AC_DEFUN([AC_IEEE_LE],
[
  AC_MSG_CHECKING([if double and float are ieee little endian])
  AC_LANG(C)
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
     int compare(unsigned char* a,unsigned char* b) {
       while(*a != 0) if (*(b++)!=*(a++)) return 1;
       return 0;
     }
     int main(int argc,char** argv) {
       unsigned char dc[[]]={0xD9,0xCC,0x67,0x93,0x80,0xDE,0x61,0x30,0};
       double da=1.23456789e-75;
       unsigned char* ca;
     
       unsigned char fc[[]]={0x22,0x48,0x83,0x05,0};
       float fa=1.23456789e-35;
     
       if (sizeof(double)!=8) return 1;
     
       ca=(unsigned char*)&da;
       if (compare(dc,ca)) return 1;

       if (sizeof(float)!=4) return 1;

       ca=(unsigned char*)&fa;
       if (compare(fc,ca)) return 1;
     
       return 0;
     }
    ])
  ],
  [AS_VAR_SET(IS_IEEE_LE, 1)],
  [AS_VAR_SET(IS_IEEE_LE, 0)],
  [])
  if test $IS_IEEE_LE = 0
  then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
]
)

dnl pthreads
dnl Enrico Fucile 06-2008
AC_DEFUN([AC_GRIB_PTHREADS],
[
  AC_MSG_CHECKING([if pthreads available])
  AC_LANG(C)
  OLDLIBS=$LIBS
  LIBS="$LIBS -lpthread"
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
       #include <pthread.h>
       #include <stdio.h>

       #define NUMTHRDS 4
       static int count;
       
       static pthread_once_t once  = PTHREAD_ONCE_INIT;
       static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
       static pthread_t callThd[[NUMTHRDS]];
       
       static void init() {
         pthread_mutexattr_t attr;
       
         pthread_mutexattr_init(&attr);
         pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
         pthread_mutex_init(&mutex,&attr);
         pthread_mutexattr_destroy(&attr);
       }
       
       void* increment(void* arg);
       
       int main(int argc,char** argv){
         long i;
         void* status=0;
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         count=0;
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
         for (i=0;i<NUMTHRDS;i++) {
           pthread_create( &callThd[[i]], &attr, increment, (void *)i);
         }
         pthread_attr_destroy(&attr);
         for(i=0; i<NUMTHRDS; i++)
             pthread_join( callThd[[i]], &status);
         return 0;
       }
       
       void* increment(void* arg) {
         int i=(long)arg;
         pthread_once(&once,&init);
         pthread_mutex_lock(&mutex);
         count+=i;
         pthread_mutex_unlock(&mutex);
		 pthread_exit((void*) 0);
       }
    ])
  ],
  [AS_VAR_SET(GRIB_PTHREADS, 1)],
  [AS_VAR_SET(GRIB_PTHREADS, 0)],
  [])
  if test $GRIB_PTHREADS = 0
  then
    AC_MSG_RESULT(no)
	LIBS=$OLDLIBS
  else
    AC_MSG_RESULT(yes)
  fi
]
)

dnl pthreads
dnl Enrico Fucile 06-2008
AC_DEFUN([AC_GRIB_LINUX_PTHREADS],
[
  AC_MSG_CHECKING([if Linux pthreads available])
  AC_LANG(C)
  OLDLIBS=$LIBS
  LIBS="$LIBS -lpthread"
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
       #include <pthread.h>
       #include <stdio.h>

       #define NUMTHRDS 4
       static int count;
	   #define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
	   extern int pthread_mutexattr_settype(pthread_mutexattr_t* attr,int type);
       
       static pthread_once_t once  = PTHREAD_ONCE_INIT;
       static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
       static pthread_t callThd[[NUMTHRDS]];
       
       static void init() {
         pthread_mutexattr_t attr;
		 pthread_mutexattr_init(&attr);
		 pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
         pthread_mutex_init(&mutex,&attr);
         pthread_mutexattr_destroy(&attr);
       }
       
       void* increment(void* arg);
       
       int main(int argc,char** argv){
         long i;
         void* status=0;
         pthread_attr_t attr;
         pthread_attr_init(&attr);
         count=0;
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
         for (i=0;i<NUMTHRDS;i++) {
           pthread_create( &callThd[[i]], &attr, increment, (void *)i);
         }
         pthread_attr_destroy(&attr);
         for(i=0; i<NUMTHRDS; i++)
             pthread_join( callThd[[i]], &status);
         return 0;
       }
       
       void* increment(void* arg) {
         int i=(long)arg;
         pthread_once(&once,&init);
         pthread_mutex_lock(&mutex);
         count+=i;
         pthread_mutex_unlock(&mutex);
		 pthread_exit((void*) 0);
       }
    ])
  ],
  [AS_VAR_SET(GRIB_LINUX_PTHREADS, 1)
   AS_VAR_SET(GRIB_PTHREADS, 1)],
  [AS_VAR_SET(GRIB_LINUX_PTHREADS, 0)],
  [])
  if test $GRIB_LINUX_PTHREADS = 0
  then
    AC_MSG_RESULT(no)
	LIBS=$OLDLIBS
  else
    AC_MSG_RESULT(yes)
  fi
]
)

dnl endianness checking
dnl Enrico Fucile 02-2008
AC_DEFUN([AC_BIG_ENDIAN],
[
  AC_MSG_CHECKING([if Big Endian])
  AC_LANG(C)
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
      int main(int argc,char** argv){
        long one= 1;
        return !(*((char *)(&one)));
      }
    ])
  ],
  [AS_VAR_SET(IS_BIG_ENDIAN, 0)],
  [AS_VAR_SET(IS_BIG_ENDIAN, 1)],
  [])
  if test $IS_BIG_ENDIAN = 0
  then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
]
)

dnl inline available in C
dnl Enrico Fucile 03-2008
AC_DEFUN([AC_INLINE],
[
  AC_MSG_CHECKING([if inline in C])
  AC_LANG(C)
  AC_COMPILE_IFELSE(
  [
    AC_LANG_SOURCE([
	  inline int x(int a) {return a;}
      int main(int argc,char** argv){
	    int a=1;
        return x(a);
      }
    ])
  ],
  [AS_VAR_SET(HAS_INLINE, inline)],
  [AS_VAR_SET(HAS_INLINE, )],
  [])
  if test x$HAS_INLINE = "x"
  then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
]
)


dnl memory alignement
dnl Enrico Fucile 04-2008
AC_DEFUN([AC_ALIGN],
[
  AC_MSG_CHECKING([if bus error on unaligned pointers])
  AC_LANG(C)
  AC_RUN_IFELSE(
  [
    AC_LANG_SOURCE([
	  void foo(char* p) {long x=*((long*)p)+1;}
	  int main(int argc,char** argv) {char* p="xxxxxxxxx";foo(++p);return 0;}
    ])
  ],
  [AS_VAR_SET(MEM_ALIGN,0)],
  [AS_VAR_SET(MEM_ALIGN,1)],
  [])
  if test $MEM_ALIGN = "0"
  then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
]
)



dnl grib_api_PROG_CC_WARNING_PEDANTIC([-Werror -Wall...])
dnl   Sets WARN_PEDANTIC to "-pedantic" + the argument, if the compiler
dnl   accepts all of those options simultaneously, otherwise do nothing.
AC_DEFUN([grib_api_PROG_CC_WARNING_PEDANTIC],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_SUBST([WARN_PEDANTIC])dnl
AS_VAR_PUSHDEF([grib_api_Pedantic], [grib_api_cv_prog_cc_pedantic_$1])dnl
WARN_PEDANTIC=
AC_CACHE_CHECK([whether $CC supports -pedantic $1], grib_api_Pedantic,
[save_CFLAGS="$CFLAGS"
CFLAGS="-pedantic $1"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
   [AS_VAR_SET(grib_api_Pedantic, yes)],
   [AS_VAR_SET(grib_api_Pedantic, no)])
CFLAGS="$save_CFLAGS"])
AS_IF([test AS_VAR_GET(grib_api_Pedantic) = yes],
      [WARN_PEDANTIC="-pedantic $1"])
AS_VAR_POPDEF([grib_api_Pedantic])dnl
])
dnl grib_api_PROG_CC_WARNING_PEDANTIC

dnl   enable -Werror despite compiler version
AC_DEFUN([grib_api_ENABLE_WARNINGS_ARE_ERRORS],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_SUBST([WERROR])dnl
WERROR=
AC_ARG_ENABLE(werror-always, 
    AS_HELP_STRING([--enable-werror-always],
		   [enable -Werror despite compiler version]),
[], [enable_werror_always=no])
AS_IF([test $enable_werror_always = yes],
      [WERROR=-Werror])
])

dnl AC_PROG_FC_MOD
dnl ---------------
AC_DEFUN([AC_PROG_FC_UPPERCASE_MOD],
[
AC_LANG_PUSH(Fortran)
AC_MSG_CHECKING([if Fortran 90 compiler capitalizes .mod filenames])
cat <<EOF >conftest.f90
    module conftest
    end module conftest
EOF

ac_try='$FC $FCFLAGS -c conftest.f90 >&AS_MESSAGE_LOG_FD'
if AC_TRY_EVAL(ac_try) && test -f CONFTEST.mod ; then
   ac_cv_prog_f90_uppercase_mod=yes
   rm -f CONFTEST.mod
else
   ac_cv_prog_f90_uppercase_mod=no
fi
AC_MSG_RESULT($ac_cv_prog_f90_uppercase_mod)
#rm -f conftest*
AC_LANG_POP(Fortran)
])

# ===========================================================================
#    http://www.gnu.org/software/autoconf-archive/ax_f90_module_flag.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_F90_MODULE_FLAG
#
# DESCRIPTION
#
#   Find Fortran 90 modules inclusion flag. The module inclusion flag is
#   stored in the cached variable ax_f90_modflag. An error is triggered if
#   the flag cannot be found. Supported are the -I GNU compilers flag, the
#   -M SUN compilers flag, and the -p Absoft Pro Fortran compiler flag.
#
# LICENSE
#
#   Copyright (c) 2009 Luc Maisonobe <luc@spaceroots.org>
#   Copyright (c) 2009 Julian C. Cummings <cummings@cacr.caltech.edu>
#   Copyright (c) 2009 Alexander Pletzer <pletzer@txcorp.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

##serial 12

AC_DEFUN([AX_F90_MODULE_FLAG],[
AC_CACHE_CHECK([fortran 90 modules inclusion flag],
ax_cv_f90_modflag,
[AC_LANG_PUSH(Fortran)
i=0
while test \( -f tmpdir_$i \) -o \( -d tmpdir_$i \) ; do
  i=`expr $i + 1`
done
mkdir tmpdir_$i
cd tmpdir_$i
AC_COMPILE_IFELSE([
!234567
      module conftest_module
      contains
      subroutine conftest_routine
      write(*,'(a)') 'gotcha!'
      end subroutine conftest_routine
      end module conftest_module
  ],[],[])
cd ..
ax_cv_f90_modflag="not found"
for ax_flag in "-I" "-M" "-p"; do
  if test "$ax_cv_f90_modflag" = "not found" ; then
    ax_save_FCFLAGS="$FCFLAGS"
    FCFLAGS="$ax_save_FCFLAGS ${ax_flag}tmpdir_$i"
    AC_COMPILE_IFELSE([
!234567
      program conftest_program
      use conftest_module
      call conftest_routine
      end program conftest_program
      ],[ax_cv_f90_modflag="$ax_flag"],[])
    FCFLAGS="$ax_save_FCFLAGS"
  fi
done
rm -fr tmpdir_$i
#if test "$ax_cv_f90_modflag" = "not found" ; then
#  AC_MSG_ERROR([unable to find compiler flag for modules inclusion])
#fi
AC_LANG_POP(Fortran)
])])

dnl AC_PROG_FC_DEBUG_IN_MODULE
dnl Daniel Varela 2011-03-25
dnl ---------------
AC_DEFUN([AC_PROG_FC_DEBUG_IN_MODULE],
[
AC_LANG_PUSH(Fortran)
AC_MSG_CHECKING([if Fortran 90 can resolve debug symbols in modules])
cat <<EOF >conftest-module.f90
    module conftest
    end module conftest
EOF
cat <<EOF >conftest.f90
	program f90usemodule
	use CONFTEST
	end program f90usemodule
EOF

ac_compile_module='$FC -g -c conftest-module.f90 >&AS_MESSAGE_LOG_FD'
ac_link_program='$FC -g -o conftest -I. conftest.f90 >&AS_MESSAGE_LOG_FD'

if AC_TRY_EVAL(ac_compile_module) && AC_TRY_EVAL(ac_link_program) && test -f conftest ; then
   ac_cv_prog_f90_debug_in_module=yes
   rm -f conftest
else
   ac_cv_prog_f90_debug_in_module=no
fi
AC_MSG_RESULT($ac_cv_prog_f90_debug_in_module)
#rm -f conftest*
AC_LANG_POP(Fortran)
])


