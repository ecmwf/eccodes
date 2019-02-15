! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to control decimal precision when packing fields.
!
!
program precision
  use eccodes
  implicit none
  integer(kind = 4)                             :: size
  integer                                       :: infile,outfile
  integer                                       :: igrib
  real(kind = 8), dimension(:), allocatable     :: values1
  real(kind = 8), dimension(:), allocatable     :: values2
  real(kind = 8)                                ::  maxa,a,maxv,minv,maxr,r
  integer( kind = 4)                            :: decimalPrecision,bitsPerValue1,bitsPerValue2
  integer                                       :: i, iret

  call codes_open_file(infile, &
       '../../data/regular_latlon_surface_constant.grib1','r')

  call codes_open_file(outfile, &
       '../../data/regular_latlon_surface_prec.grib1','w')

  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib)

  !     bitsPerValue before changing the packing parameters
  call codes_get(igrib,'bitsPerValue',bitsPerValue1)

  !     get the size of the values array
  call codes_get_size(igrib,"values",size)

  allocate(values1(size), stat=iret)
  allocate(values2(size), stat=iret)
  !     get data values before changing the packing parameters*/
  call codes_get(igrib,"values",values1)

  !     setting decimal precision=2 means that 2 decimal digits
  !     are preserved when packing.
  decimalPrecision=2
  call codes_set(igrib,"setDecimalPrecision", &
       decimalPrecision)

  !     bitsPerValue after changing the packing parameters
  call codes_get(igrib,"bitsPerValue",bitsPerValue2)

  !     get data values after changing the packing parameters
  call codes_get(igrib,"values",values2)

  !     computing error
  maxa=0
  maxr=0
  maxv=values2(1)
  minv=maxv
  do i=1,size
    a=abs(values2(i)-values1(i))
    if ( values2(i) .gt. maxv ) maxv=values2(i)
    if ( values2(i) .lt. maxv ) minv=values2(i)
    if ( values2(i) .ne. 0 ) then
      r=abs((values2(i)-values1(i))/values2(i))
    endif
    if ( a .gt. maxa ) maxa=a
    if ( r .gt. maxr ) maxr=r
  enddo
  write(*,*) "max absolute error = ",maxa
  write(*,*) "max relative error = ",maxr
  write(*,*) "min value = ",minv
  write(*,*) "max value = ",maxv

  write(*,*) "old number of bits per value=",bitsPerValue1
  write(*,*) "new number of bits per value=",bitsPerValue2

  !     write modified message to a file
  call codes_write(igrib,outfile)

  call codes_release(igrib)

  call codes_close_file(infile)

  call codes_close_file(outfile)

  deallocate(values1)
  deallocate(values2)
end program precision
