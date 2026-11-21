! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: Single precision (real4) get/set
!   1. Spectral GRIB: setting values as real4 gives the same message as setting them as real8
!   2. BUFR: real4 values equal real8 values; CODES_MISSING_FLOAT sets a missing value
!
program codes_real4_get_set
   use eccodes
   implicit none
   integer :: igrib, ig4, ig8, ifile, ibufr, iret, n, i
   real(kind=4), dimension(:), allocatable :: v4
   real(kind=8), dimension(:), allocatable :: v8
   character(len=1), dimension(:), allocatable :: m4, m8
   integer(kind=kindOfSize) :: s4, s8
   real(kind=4) :: f4

   ! --- 1. Spectral GRIB (spectral_complex)
   call codes_grib_new_from_samples(igrib, 'sh_ml_grib2')
   call codes_get_size(igrib, 'values', n)
   allocate (v4(n), v8(n))
   call codes_get(igrib, 'values', v4)
   v4 = v4*1.5
   v8 = v4   ! exact widening
   call codes_clone(igrib, ig4)
   call codes_clone(igrib, ig8)
   call codes_set(ig4, 'values', v4)
   call codes_set(ig8, 'values', v8)
   call codes_get_message_size(ig4, s4)
   call codes_get_message_size(ig8, s8)
   if (s4 /= s8) call codes_check(-2, 'real4 and real8 messages have different sizes', '')
   allocate (m4(s4), m8(s8))
   call codes_copy_message(ig4, m4)
   call codes_copy_message(ig8, m8)
   if (any(m4 /= m8)) call codes_check(-2, 'real4 and real8 messages differ', '')
   print *, 'spectral: real4 and real8 encoding give identical messages, size=', s4
   deallocate (v4, v8, m4, m8)
   call codes_release(ig4)
   call codes_release(ig8)
   call codes_release(igrib)

   ! --- 2. BUFR
   call codes_open_file(ifile, '../../data/bufr/temp_101.bufr', 'r')
   call codes_bufr_new_from_file(ifile, ibufr, iret)
   call codes_set(ibufr, 'unpack', 1)
   call codes_get_size(ibufr, 'airTemperature', n)
   allocate (v4(n), v8(n))
   call codes_get(ibufr, 'airTemperature', v4)
   call codes_get(ibufr, 'airTemperature', v8)
   do i = 1, n
      if (v8(i) == CODES_MISSING_DOUBLE) then
         if (v4(i) /= CODES_MISSING_FLOAT) call codes_check(-2, 'missing value not mapped to CODES_MISSING_FLOAT', '')
      else
         if (v4(i) /= real(v8(i), 4)) call codes_check(-2, 'real4 /= real(real8,4)', '')
      end if
   end do
   print *, 'bufr: airTemperature', n, 'values, real4 == real8'

   ! Set the first one to missing with CODES_MISSING_FLOAT and the second one to a new value
   call codes_set(ibufr, '#1#airTemperature', CODES_MISSING_FLOAT)
   call codes_set(ibufr, '#2#airTemperature', 250.5)
   call codes_set(ibufr, 'pack', 1)
   call codes_set(ibufr, 'unpack', 1)
   call codes_get(ibufr, '#1#airTemperature', v8(1))
   if (v8(1) /= CODES_MISSING_DOUBLE) call codes_check(-2, 'CODES_MISSING_FLOAT did not set a missing value', '')
   call codes_get(ibufr, '#2#airTemperature', f4)
   if (f4 /= 250.5) call codes_check(-2, 'real4 value not set', '')
   print *, 'bufr: CODES_MISSING_FLOAT sets missing, new value=', f4

   deallocate (v4, v8)
   call codes_release(ibufr)
   call codes_close_file(ifile)
end program codes_real4_get_set
